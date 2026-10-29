// B14 / TC-NFR-02 (RT-NF002): seat availability must remain consistent
// after booking and cancellation, including after reloading from disk.
#include "BookingModule.h"
#include "FareModule.h"
#include "FileModule.h"
#include "TicketModule.h"
#include "TrainModule.h"
#include "ValidationModule.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace {

int failures = 0;

void check(bool condition, const std::string& message) {
    if (!condition) {
        ++failures;
        std::cerr << "FAIL: " << message << '\n';
    }
}

Passenger passenger(const std::string& name = "Asha Rao") {
    Passenger p;
    p.name = name;
    p.age = 25;
    p.gender = 'F';
    p.contact = "9876543210";
    return p;
}

int availableOf(const TrainModule& trains, int trainNo) {
    Train t;
    return trains.findByNumber(trainNo, t) == Status::OK ? t.availableSeats : -1;
}

// Invariant: availableSeats == totalSeats - confirmed bookings, and no two
// confirmed bookings on the same train share a seat number.
void checkInvariant(const TrainModule& trains,
                    const std::vector<Booking>& bookings,
                    const std::string& when) {
    for (const Train& train : trains.list()) {
        std::set<int> seats;
        int confirmed = 0;
        for (const Booking& b : bookings) {
            if (b.trainNo == train.trainNo && b.status == "CONFIRMED") {
                ++confirmed;
                check(b.seatNo >= 1 && b.seatNo <= train.totalSeats,
                      when + ": seat number within train capacity");
                check(seats.insert(b.seatNo).second,
                      when + ": no duplicate confirmed seat on train " +
                          std::to_string(train.trainNo));
            }
        }
        check(train.availableSeats == train.totalSeats - confirmed,
              when + ": available == total - confirmed on train " +
                  std::to_string(train.trainNo));
        check(train.availableSeats >= 0 &&
                  train.availableSeats <= train.totalSeats,
              when + ": availability stays within 0..total");
    }
}

}  // namespace

int main() {
    namespace fs = std::filesystem;
    const fs::path original = fs::current_path();
    const fs::path temp = fs::temp_directory_path() /
        ("railway_nfr_seats_" +
         std::to_string(std::chrono::steady_clock::now()
                            .time_since_epoch().count()));
    fs::create_directories(temp);
    fs::current_path(temp);

    {
        std::ofstream trainFile("trains.txt");
        trainFile << "111|Alpha Express|Bengaluru|Chennai|10:00|16:00|500|5\n"
                  << "222|Beta Express|Bengaluru|Mysuru|08:00|10:30|200|3\n";
    }
    { std::ofstream bookingFile("bookings.txt"); }

    FileModule files;
    std::vector<Train> loaded;
    std::vector<Booking> bookings;
    files.loadAll(loaded, bookings);

    TrainModule trains(std::move(loaded));
    ValidationModule validation;
    FareModule fares(trains);
    TicketModule tickets(bookings);
    BookingModule module(trains, bookings, validation, fares, tickets, files);

    // 1. Booking reduces availability one seat at a time.
    std::vector<std::string> pnrs;
    for (int i = 1; i <= 5; ++i) {
        Booking b;
        check(module.createBooking(111, passenger(), b) == Status::OK,
              "booking " + std::to_string(i) + " succeeds");
        check(b.seatNo == i, "seats are allocated lowest-first");
        check(availableOf(trains, 111) == 5 - i,
              "availability drops by one after each booking");
        pnrs.push_back(b.pnr);
    }
    checkInvariant(trains, bookings, "after filling train 111");

    // 2. A full train rejects further bookings and stays at zero.
    Booking rejected;
    check(module.createBooking(111, passenger(), rejected) == Status::NO_SEATS,
          "full train returns NO_SEATS");
    check(availableOf(trains, 111) == 0, "full train reports 0 available");

    // 3. Cancelling releases exactly one seat, and it is reused.
    check(module.cancelBooking(pnrs[2]) == Status::OK, "cancel seat 3");
    check(availableOf(trains, 111) == 1, "cancel releases one seat");
    checkInvariant(trains, bookings, "after one cancellation");

    Booking reused;
    check(module.createBooking(111, passenger(), reused) == Status::OK,
          "rebook after cancellation");
    check(reused.seatNo == 3, "released seat 3 is reallocated");
    check(availableOf(trains, 111) == 0, "train is full again");

    // 4. Repeat cancellation must not release a second seat.
    check(module.cancelBooking(pnrs[0]) == Status::OK, "cancel seat 1");
    check(module.cancelBooking(pnrs[0]) == Status::ALREADY_CANCELLED,
          "second cancel of same PNR is rejected");
    check(availableOf(trains, 111) == 1,
          "repeat cancellation does not release extra seats");
    checkInvariant(trains, bookings, "after repeat cancellation");

    // 5. Invalid cancellations leave availability unchanged.
    check(module.cancelBooking("123") == Status::INVALID_PNR,
          "malformed PNR rejected");
    check(module.cancelBooking("9999999999") == Status::PNR_NOT_FOUND,
          "unknown PNR rejected");
    check(availableOf(trains, 111) == 1,
          "invalid cancellations do not change availability");

    // 6. Bookings on one train never affect another train.
    check(availableOf(trains, 222) == 3, "train 222 untouched");
    Booking other;
    check(module.createBooking(222, passenger("Ravi Kumar"), other) ==
              Status::OK, "book on train 222");
    check(other.seatNo == 1, "train 222 numbers its own seats from 1");
    check(availableOf(trains, 111) == 1, "train 111 unaffected");

    // 7. Rejected bookings do not consume seats.
    Booking bad;
    Passenger invalid = passenger();
    invalid.age = 0;
    check(module.createBooking(111, invalid, bad) == Status::INVALID_INPUT,
          "invalid passenger rejected");
    check(module.createBooking(404, passenger(), bad) ==
              Status::TRAIN_NOT_FOUND, "unknown train rejected");
    check(availableOf(trains, 111) == 1,
          "rejected bookings do not consume seats");

    // 8. Persisted data reproduces the same availability after restart.
    {
        FileModule reloadFiles;
        std::vector<Train> reloadedTrains;
        std::vector<Booking> reloadedBookings;
        reloadFiles.loadAll(reloadedTrains, reloadedBookings);
        TrainModule reloaded(std::move(reloadedTrains));
        check(reloadedBookings.size() == bookings.size(),
              "all bookings persisted");
        check(availableOf(reloaded, 111) == availableOf(trains, 111),
              "train 111 availability matches after reload");
        check(availableOf(reloaded, 222) == availableOf(trains, 222),
              "train 222 availability matches after reload");
        checkInvariant(reloaded, reloadedBookings, "after reload");
    }

    // 9. Deterministic stress: 600 mixed operations keep the invariant.
    unsigned state = 12345;
    auto next = [&state]() {
        state = state * 1103515245u + 12345u;
        return (state >> 16) & 0x7fff;
    };
    for (int step = 0; step < 600; ++step) {
        const int trainNo = (next() % 2 == 0) ? 111 : 222;
        if (next() % 3 != 0) {
            Booking b;
            module.createBooking(trainNo, passenger(), b);
        } else {
            std::vector<std::string> confirmed;
            for (const Booking& b : bookings) {
                if (b.status == "CONFIRMED") confirmed.push_back(b.pnr);
            }
            if (!confirmed.empty()) {
                module.cancelBooking(confirmed[next() % confirmed.size()]);
            }
        }
        if (step % 50 == 0) {
            checkInvariant(trains, bookings,
                           "stress step " + std::to_string(step));
        }
    }
    checkInvariant(trains, bookings, "after stress run");

    fs::current_path(original);
    std::error_code ec;
    fs::remove_all(temp, ec);

    if (failures != 0) {
        std::cerr << failures << " check(s) failed.\n";
        return 1;
    }
    std::cout << "All TC-NFR-02 seat consistency checks passed.\n";
    return 0;
}

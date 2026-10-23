#include <filesystem>
#include "BookingModule.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace {

int failures = 0;

void check(bool condition, const std::string& message) {
    if (!condition) {
        ++failures;
        std::cerr << "FAIL: " << message << '\n';
    }
}

Passenger validPassenger() {
    Passenger passenger;
    passenger.name = "Asha Rao";
    passenger.age = 25;
    passenger.gender = 'F';
    passenger.contact = "9876543210";
    return passenger;
}

}  // namespace

int main() {
    namespace fs = std::filesystem;

    const fs::path originalDirectory = fs::current_path();
    const auto stamp =
        std::chrono::steady_clock::now().time_since_epoch().count();

    const fs::path tempDirectory =
        fs::temp_directory_path() /
        ("railway_b06_test_" + std::to_string(stamp));

    try {
        fs::create_directories(tempDirectory);
        fs::current_path(tempDirectory);

        {
            std::ofstream trainFile("trains.txt");
            trainFile
                << "123|Test Express|Bengaluru|Chennai|10:00|16:00|500|2\n";
        }

        {
            std::ofstream bookingFile("bookings.txt");
        }

        FileModule files;
        std::vector<Train> loadedTrains;
        std::vector<Booking> bookings;

        const LoadReport report =
            files.loadAll(loadedTrains, bookings);

        check(report.trainsLoaded == 1, "load one test train");

        TrainModule trains(std::move(loadedTrains));
        ValidationModule validation;
        FareModule fares;
        TicketModule tickets;

        BookingModule module(
            trains,
            bookings,
            validation,
            fares,
            tickets,
            files
        );

        Booking first;
        Status result = module.createBooking(
            123, validPassenger(), first
        );

        check(result == Status::OK, "first booking succeeds");
        check(first.seatNo == 1, "first booking receives seat 1");
        check(first.pnr.size() == 10, "first booking receives a 10-character PNR");
        check(bookings.size() == 1, "first booking is recorded");

        Passenger invalid = validPassenger();
        invalid.name = "";

        Booking rejected;
        result = module.createBooking(123, invalid, rejected);

        check(result == Status::INVALID_INPUT,
              "invalid passenger is rejected");
        check(bookings.size() == 1,
              "invalid passenger does not create a booking");

        result = module.createBooking(999, validPassenger(), rejected);

        check(result == Status::TRAIN_NOT_FOUND,
              "unknown train is rejected");

        Booking second;
        result = module.createBooking(
            123, validPassenger(), second
        );

        check(result == Status::OK, "second booking succeeds");
        check(second.seatNo == 2, "second booking receives seat 2");
        check(first.pnr != second.pnr, "PNRs are unique");

        result = module.createBooking(123, validPassenger(), rejected);

        check(result == Status::NO_SEATS,
              "booking is rejected when the train is full");
        check(bookings.size() == 2,
              "full train does not receive another booking");

        Train updatedTrain;
        check(trains.findByNumber(123, updatedTrain) == Status::OK,
              "train remains available in the train module");
        check(updatedTrain.availableSeats == 0,
              "seat availability reaches zero after two bookings");

        const fs::path savedDirectory = fs::current_path();
        fs::current_path(originalDirectory);
        fs::remove_all(savedDirectory);
    } catch (const std::exception& error) {
        fs::current_path(originalDirectory);
        std::cerr << "Test setup/runtime error: "
                  << error.what() << '\n';
        return 1;
    }

    if (failures != 0) {
        std::cerr << failures << " test(s) failed.\n";
        return 1;
    }

    std::cout << "All BookingModule tests passed.\n";
    return 0;
}

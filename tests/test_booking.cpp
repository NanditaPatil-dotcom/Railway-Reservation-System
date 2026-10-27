#include "BookingModule.h"
#include "FileModule.h"
#include "FareModule.h"
#include "TicketModule.h"
#include "TrainModule.h"
#include "ValidationModule.h"

#include <chrono>
#include <filesystem>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <system_error>
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
        std::chrono::steady_clock::now()
            .time_since_epoch()
            .count();

    const fs::path tempDirectory =
        fs::temp_directory_path() /
        ("railway_b09_test_" + std::to_string(stamp));

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

        check(report.trainsLoaded == 1,
              "load one test train");

        TrainModule trains(std::move(loadedTrains));
        ValidationModule validation;
        FareModule fares(trains);
        TicketModule tickets(bookings);

        BookingModule module(
            trains,
            bookings,
            validation,
            fares,
            tickets,
            files
        );

        // B07: Verify fare retrieval.
        check(fares.calculateFare(123) == 500.0,
              "fare matches the predefined train fare");

        check(fares.calculateFare(999) < 0.0,
              "unknown train has no valid fare");

        // Test the first booking.
        Booking first;
        Status result = module.createBooking(
            123,
            validPassenger(),
            first
        );

        check(result == Status::OK,
              "first booking succeeds");

        check(first.seatNo == 1,
              "first booking receives seat 1");

        check(first.pnr.size() == 10,
              "first booking receives a 10-character PNR");

        check(first.pnr == "1000000001",
              "first booking receives the starting PNR");

        check(first.fare == 500.0,
              "first booking stores the predefined fare");

        check(first.status == "CONFIRMED",
              "first booking has confirmed status");

        check(bookings.size() == 1,
              "first booking is recorded");

        // B08: Retrieve an existing booking using its PNR.
        Booking retrieved;
        result = tickets.find(first.pnr, retrieved);

        check(result == Status::OK,
              "existing booking can be retrieved by PNR");

        check(retrieved.pnr == first.pnr,
              "retrieved booking has the correct PNR");

        check(retrieved.passenger.name == first.passenger.name,
              "retrieved booking has the correct passenger");

        check(retrieved.fare == first.fare,
              "retrieved booking has the correct fare");

        check(retrieved.seatNo == first.seatNo,
              "retrieved booking has the correct seat");

        check(retrieved.status == first.status,
              "retrieved booking has the correct status");

        // B08: Invalid and unknown PNRs must be rejected.
        check(
            tickets.find("123", retrieved) == Status::INVALID_PNR,
            "invalid PNR format is rejected"
        );

        check(
            tickets.find("9999999999", retrieved)
                == Status::PNR_NOT_FOUND,
            "unknown valid-format PNR returns booking not found"
        );

        // Invalid passenger details must not create a booking.
        Passenger invalid = validPassenger();
        invalid.name = "";

        Booking rejected;
        result = module.createBooking(
            123,
            invalid,
            rejected
        );

        check(result == Status::INVALID_INPUT,
              "invalid passenger is rejected");

        check(bookings.size() == 1,
              "invalid passenger does not create a booking");

        // Unknown trains must be rejected.
        result = module.createBooking(
            999,
            validPassenger(),
            rejected
        );

        check(result == Status::TRAIN_NOT_FOUND,
              "unknown train is rejected");

        // Test a second booking.
        Booking second;
        result = module.createBooking(
            123,
            validPassenger(),
            second
        );

        check(result == Status::OK,
              "second booking succeeds");

        check(second.seatNo == 2,
              "second booking receives seat 2");

        check(second.pnr.size() == 10,
              "second booking receives a 10-character PNR");

        check(second.pnr == "1000000002",
              "second booking receives the next PNR");

        check(first.pnr != second.pnr,
              "PNRs are unique");

        check(second.fare == 500.0,
              "second booking stores the predefined fare");

        check(bookings.size() == 2,
              "both valid bookings are recorded");

        // A full train must reject another booking.
        result = module.createBooking(
            123,
            validPassenger(),
            rejected
        );

        check(result == Status::NO_SEATS,
              "booking is rejected when the train is full");

        check(bookings.size() == 2,
              "full train does not receive another booking");

        Train updatedTrain;

        check(
            trains.findByNumber(123, updatedTrain) == Status::OK,
            "train remains available in the train module"
        );

        check(updatedTrain.availableSeats == 0,
              "seat availability reaches zero after two bookings");

        // B09: Cancel a confirmed booking.
        result = module.cancelBooking(first.pnr);

        check(result == Status::OK,
              "confirmed booking can be cancelled");

        // Verify that the booking status changes.
        Booking cancelledBooking;

        check(
            tickets.find(first.pnr, cancelledBooking) == Status::OK,
            "cancelled booking can still be retrieved"
        );

        check(
            cancelledBooking.status == "CANCELLED",
            "cancelled booking has cancelled status"
        );

        // Verify that the released seat becomes available.
        Train afterCancellation;

        check(
            trains.findByNumber(123, afterCancellation) == Status::OK,
            "train can be found after cancellation"
        );

        check(
            afterCancellation.availableSeats == 1,
            "cancellation releases one seat"
        );

        // Cancelling the same booking twice must be rejected.
        check(
            module.cancelBooking(first.pnr) == Status::ALREADY_CANCELLED,
            "repeated cancellation is rejected"
        );

        // Invalid and nonexistent PNRs must be rejected.
        check(
            module.cancelBooking("123") == Status::INVALID_PNR,
            "cancellation rejects an invalid PNR format"
        );

        check(
            module.cancelBooking("9999999999") == Status::PNR_NOT_FOUND,
            "cancellation rejects an unknown PNR"
        );

        // A new booking should be able to reuse the released seat.
        Booking replacement;
        result = module.createBooking(
            123,
            validPassenger(),
            replacement
        );

        check(
            result == Status::OK,
            "new booking succeeds after cancellation"
        );

        check(
            replacement.seatNo == 1,
            "new booking reuses the released seat"
        );

        check(
            replacement.pnr == "1000000003",
            "replacement booking receives the next PNR"
        );

        check(
            bookings.size() == 3,
            "cancelled and confirmed bookings remain recorded"
        );

        Train afterReplacement;

        check(
            trains.findByNumber(123, afterReplacement) == Status::OK,
            "train can be found after replacement booking"
        );

        check(
            afterReplacement.availableSeats == 0,
            "availability becomes zero after the released seat is rebooked"
        );

        // Reload persisted bookings to verify cancellation and PNR continuity.
        std::vector<Train> reloadedTrains;
        std::vector<Booking> reloadedBookings;

        FileModule restartedFiles;
        const LoadReport restartReport =
            restartedFiles.loadAll(
                reloadedTrains,
                reloadedBookings
            );

        check(restartReport.bookingsLoaded == 3,
              "all bookings can be loaded from persistent storage");

        // B08: Retrieve a booking after reloading saved records.
        TicketModule restartedTickets(reloadedBookings);

        Booking reloadedBooking;

        check(
            restartedTickets.find(first.pnr, reloadedBooking)
                == Status::OK,
            "booking can be retrieved after reloading saved records"
        );

        check(
            reloadedBooking.pnr == first.pnr,
            "reloaded booking retains its original PNR"
        );

        // B09: Cancellation status must persist after reload.
        check(
            reloadedBooking.status == "CANCELLED",
            "cancellation status persists after reloading"
        );

        // The first booking is cancelled and the replacement is confirmed.
        // The second booking is also confirmed, so both seats are occupied.
        check(
            !reloadedTrains.empty() &&
                reloadedTrains[0].availableSeats == 0,
            "reloaded seat availability reflects confirmed bookings"
        );

        check(
            restartedTickets.generatePNR() == "1000000004",
            "PNR numbering continues after reloading saved bookings"
        );

        // Restore the original working directory before cleanup.
        fs::current_path(originalDirectory);
        fs::remove_all(tempDirectory);

    } catch (const std::exception& error) {
        fs::current_path(originalDirectory);
        std::error_code cleanupError;
        fs::remove_all(tempDirectory, cleanupError);
        std::cerr << "Test setup/runtime error: "
                  << error.what() << '\n';
        return 1;
    }

    if (failures != 0) {
        std::cerr << failures << " test(s) failed.\n";
        return 1;
    }

    std::cout
        << "All BookingModule, B07, B08 and B09 tests passed.\n";
    return 0;
}
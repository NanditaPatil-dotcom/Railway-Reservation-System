// B12: administrator train add/modify acceptance coverage.
// Build and run from the repository root:
//   g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude tests/test_admin_trains.cpp src/AdminModule.cpp src/TrainModule.cpp src/FileModule.cpp -o test_admin_trains && ./test_admin_trains

#include "AdminModule.h"
#include "FileModule.h"
#include "TrainModule.h"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace {

int failures = 0;

void check(bool condition, const std::string& label) {
    if (!condition) {
        ++failures;
        std::cout << "FAIL: " << label << '\n';
    }
}

class TemporaryWorkingDirectory {
public:
    TemporaryWorkingDirectory()
        : original_(std::filesystem::current_path()),
          temporary_(std::filesystem::temp_directory_path() /
                     ("railway_admin_train_tests_" +
                      std::to_string(std::chrono::steady_clock::now()
                                         .time_since_epoch()
                                         .count()))) {
        std::filesystem::create_directories(temporary_);
        std::filesystem::current_path(temporary_);
    }

    ~TemporaryWorkingDirectory() {
        std::filesystem::current_path(original_);
        std::filesystem::remove_all(temporary_);
    }

private:
    std::filesystem::path original_;
    std::filesystem::path temporary_;
};

Train sampleTrain() {
    Train train;
    train.trainNo = 45001;
    train.name = "Coastal Express";
    train.source = "Bengaluru";
    train.destination = "Mangaluru";
    train.departure = "08:00";
    train.arrival = "15:30";
    train.fare = 500.0;
    train.totalSeats = 10;
    train.availableSeats = 10;
    return train;
}

}  // namespace

int main() {
    TemporaryWorkingDirectory temporaryDirectory;
    FileModule files;
    std::vector<Train> loadedTrains;
    std::vector<Booking> bookings;
    files.loadAll(loadedTrains, bookings);

    TrainModule trains(std::move(loadedTrains));
    Booking first;
    first.pnr = "1234567890";
    first.trainNo = 45001;
    first.fare = 500.0;
    first.status = "CONFIRMED";
    Booking second = first;
    second.pnr = "1234567891";
    second.fare = 450.0;
    Booking cancelled = first;
    cancelled.pnr = "1234567892";
    cancelled.status = "CANCELLED";
    bookings = {first, second, cancelled};
    files.saveBookings(bookings);

    AdminModule admin(trains, bookings, files);
    check(admin.login("admin", "Admin@123") == Status::OK,
          "administrator can log in");

    Train train = sampleTrain();
    check(admin.addTrain(train) == Status::OK,
          "new train can be added");
    check(admin.addTrain(train) == Status::DUPLICATE_TRAIN_NO,
          "duplicate train number is rejected");

    Train updated = train;
    updated.name = "Coastal Limited";
    updated.fare = 650.0;
    updated.totalSeats = 1;
    check(admin.modifyTrain(train.trainNo, updated) == Status::INVALID_CAPACITY,
          "capacity cannot be reduced below confirmed bookings");

    updated.totalSeats = 4;
    check(admin.modifyTrain(train.trainNo, updated) == Status::OK,
          "existing train can be modified and saved");

    Train modified;
    check(trains.findByNumber(train.trainNo, modified) == Status::OK &&
              modified.name == "Coastal Limited" && modified.fare == 650.0 &&
              modified.totalSeats == 4 && modified.availableSeats == 2,
          "modified fields and availability are reflected in memory");
    check(bookings.size() == 3 && bookings[0].fare == 500.0 &&
              bookings[1].fare == 450.0 && bookings[2].fare == 500.0,
          "existing booking fares remain unchanged");

    std::vector<Train> persistedTrains;
    std::vector<Booking> persistedBookings;
    FileModule reloadedFiles;
    reloadedFiles.loadAll(persistedTrains, persistedBookings);
    check(persistedTrains.size() == 1 &&
              persistedTrains[0].name == "Coastal Limited" &&
              persistedTrains[0].fare == 650.0 &&
              persistedTrains[0].totalSeats == 4,
          "modified train record survives reload");
    check(persistedBookings.size() == 3 &&
              persistedBookings[0].fare == 500.0 &&
              persistedBookings[1].fare == 450.0 &&
              persistedBookings[2].fare == 500.0,
          "stored booking fares survive train modification and reload");

    if (failures == 0) {
        std::cout << "All B12 administrator train tests passed.\n";
        return 0;
    }
    std::cout << failures << " test(s) failed.\n";
    return 1;
}

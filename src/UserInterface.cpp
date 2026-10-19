#include "UserInterface.h"

#include "FileModule.h"
#include "SearchModule.h"
#include "TrainModule.h"

#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {
void displayTrains(const std::vector<Train>& trains) {
    if (trains.empty()) {
        std::cout << "No trains found. Check the search or train records and try again.\n";
        return;
    }

    std::cout << std::left << std::setw(11) << "Train No."
              << std::setw(24) << "Train Name"
              << std::setw(18) << "Source"
              << std::setw(18) << "Destination"
              << std::setw(12) << "Departure"
              << std::setw(12) << "Arrival"
              << std::setw(10) << "Fare"
              << std::setw(13) << "Total Seats"
              << "Available Seats\n";
    std::cout << std::string(120, '-') << '\n';
    for (const Train& train : trains) {
        std::cout << std::left << std::setw(11) << train.trainNo
                  << std::setw(24) << train.name
                  << std::setw(18) << train.source
                  << std::setw(18) << train.destination
                  << std::setw(12) << train.departure
                  << std::setw(12) << train.arrival
                  << std::setw(10) << std::fixed << std::setprecision(2) << train.fare
                  << std::setw(13) << train.totalSeats
                  << train.availableSeats << '\n';
    }
}

bool readInteger(const std::string& prompt, int& value) {
    std::cout << prompt;
    std::string input;
    if (!std::getline(std::cin, input)) return false;
    std::istringstream parser(input);
    char extra = '\0';
    if (!(parser >> value) || (parser >> extra)) {
        std::cout << "Please enter a valid whole number.\n";
        return false;
    }
    return true;
}
} // namespace

void UserInterface::run() {
    FileModule files;
    std::vector<Train> trains;
    std::vector<Booking> bookings;
    const LoadReport report = files.loadAll(trains, bookings);
    TrainModule trainModule(std::move(trains));
    SearchModule search(trainModule);

    std::cout << "Railway Reservation System\n"
              << "Loaded " << report.trainsLoaded << " train record(s) and "
              << report.bookingsLoaded << " booking record(s).";
    if (report.linesSkipped > 0) {
        std::cout << " Skipped " << report.linesSkipped << " malformed record(s).";
    }
    std::cout << "\n";

    while (true) {
        std::cout << "\n1. View all trains\n"
                  << "2. Search by source and destination\n"
                  << "3. Search by train number\n"
                  << "0. Exit\n";
        int choice = -1;
        if (!readInteger("Choose an option: ", choice)) {
            if (!std::cin) return;
            continue;
        }
        if (choice == 0) return;
        if (choice == 1) {
            displayTrains(trainModule.list());
        } else if (choice == 2) {
            std::string source;
            std::string destination;
            std::cout << "Source: ";
            if (!std::getline(std::cin, source)) return;
            std::cout << "Destination: ";
            if (!std::getline(std::cin, destination)) return;
            if (source.find('|') != std::string::npos ||
                destination.find('|') != std::string::npos ||
                source.empty() || destination.empty()) {
                std::cout << "Enter a non-empty source and destination.\n";
                continue;
            }
            displayTrains(search.bySourceDest(source, destination));
        } else if (choice == 3) {
            int trainNo = 0;
            if (!readInteger("Train number: ", trainNo)) continue;
            if (trainNo <= 0) {
                std::cout << "Train number must be a positive whole number.\n";
                continue;
            }
            displayTrains(search.byTrainNo(trainNo));
        } else {
            std::cout << "Choose 0, 1, 2, or 3.\n";
        }
    }
}

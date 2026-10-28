#include "UserInterface.h"

#include "AdminModule.h"
#include "BookingModule.h"
#include "FareModule.h"
#include "FileModule.h"
#include "SearchModule.h"
#include "TicketModule.h"
#include "TrainModule.h"
#include "ValidationModule.h"

#include <cctype>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {

void displayTrains(const std::vector<Train>& trains) {
    if (trains.empty()) {
        std::cout
            << "No trains found. Check the search or train records and try again.\n";
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
                  << std::setw(10) << std::fixed
                  << std::setprecision(2) << train.fare
                  << std::setw(13) << train.totalSeats
                  << train.availableSeats << '\n';
    }
}

void displayTrainDetails(const Train& train) {
    const int booked = train.totalSeats - train.availableSeats;

    std::cout << "\nTrain Details\n"
              << std::string(40, '-') << '\n'
              << std::left
              << std::setw(18) << "Train Number" << ": "
              << train.trainNo << '\n'
              << std::setw(18) << "Train Name" << ": "
              << train.name << '\n'
              << std::setw(18) << "Route" << ": "
              << train.source << " -> " << train.destination << '\n'
              << std::setw(18) << "Departure" << ": "
              << train.departure << '\n'
              << std::setw(18) << "Arrival" << ": "
              << train.arrival << '\n'
              << std::setw(18) << "Fare per seat" << ": Rs. "
              << std::fixed << std::setprecision(2)
              << train.fare << '\n'
              << std::string(40, '-') << '\n'
              << std::setw(18) << "Total seats" << ": "
              << train.totalSeats << '\n'
              << std::setw(18) << "Booked seats" << ": "
              << booked << '\n'
              << std::setw(18) << "Available seats" << ": "
              << train.availableSeats << '\n';

    if (train.availableSeats > 0) {
        std::cout << std::setw(18) << "Availability"
                  << ": Seats available\n";
    } else {
        std::cout << std::setw(18) << "Availability"
                  << ": Sold out - no seats available\n";
    }
}

bool readInteger(const std::string& prompt, int& value) {
    std::cout << prompt;

    std::string input;
    if (!std::getline(std::cin, input)) {
        return false;
    }

    std::istringstream parser(input);
    char extra = '\0';

    if (!(parser >> value) || (parser >> extra)) {
        std::cout << "Please enter a valid whole number.\n";
        return false;
    }

    return true;
}

bool readAmount(const std::string& prompt, double& value) {
    std::cout << prompt;
    std::string input;
    if (!std::getline(std::cin, input)) {
        return false;
    }
    std::istringstream parser(input);
    char extra = '\0';
    if (!(parser >> value) || (parser >> extra) || !std::isfinite(value)) {
        std::cout << "Please enter a valid amount.\n";
        return false;
    }
    return true;
}

bool readTrainFields(Train& train, bool readNumber) {
    if (readNumber && !readInteger("Train number: ", train.trainNo)) {
        return false;
    }
    std::cout << "Train name: ";
    if (!std::getline(std::cin, train.name)) return false;
    std::cout << "Source station: ";
    if (!std::getline(std::cin, train.source)) return false;
    std::cout << "Destination station: ";
    if (!std::getline(std::cin, train.destination)) return false;
    std::cout << "Departure time: ";
    if (!std::getline(std::cin, train.departure)) return false;
    std::cout << "Arrival time: ";
    if (!std::getline(std::cin, train.arrival)) return false;
    if (!readAmount("Fare: Rs. ", train.fare) ||
        !readInteger("Total seats: ", train.totalSeats)) {
        return false;
    }
    return true;
}

void reportTrainChange(Status status, const std::string& action) {
    if (status == Status::OK) {
        std::cout << "Train " << action << " successfully.\n";
    } else if (status == Status::DUPLICATE_TRAIN_NO) {
        std::cout << "That train number already exists.\n";
    } else if (status == Status::TRAIN_NOT_FOUND) {
        std::cout << "Train not found. Check the train number.\n";
    } else if (status == Status::INVALID_CAPACITY) {
        std::cout << "Capacity cannot be lower than the confirmed bookings.\n";
    } else if (status == Status::INVALID_INPUT) {
        std::cout << "Train details are invalid. Check the values and try again.\n";
    } else if (status == Status::FILE_ERROR) {
        std::cout << "Train could not be saved. Please retry.\n";
    } else {
        std::cout << "Train " << action << " failed.\n";
    }
}

void handleAddTrain(AdminModule& admin) {
    Train train;
    if (!readTrainFields(train, true)) {
        if (std::cin) std::cout << "Train entry cancelled.\n";
        return;
    }
    train.availableSeats = train.totalSeats;
    reportTrainChange(admin.addTrain(train), "added");
}

void handleModifyTrain(AdminModule& admin, const TrainModule& trains) {
    int trainNo = 0;
    if (!readInteger("Train number to modify: ", trainNo)) {
        return;
    }
    Train updated;
    if (trains.findByNumber(trainNo, updated) != Status::OK) {
        reportTrainChange(Status::TRAIN_NOT_FOUND, "modified");
        return;
    }
    std::cout << "Enter the updated train details. Train number remains "
              << trainNo << ".\n";
    if (!readTrainFields(updated, false)) {
        if (std::cin) std::cout << "Train update cancelled.\n";
        return;
    }
    reportTrainChange(admin.modifyTrain(trainNo, updated), "modified");
}

bool readPassenger(Passenger& passenger) {
    std::cout << "Passenger name: ";
    if (!std::getline(std::cin, passenger.name)) {
        return false;
    }

    if (!readInteger("Passenger age: ", passenger.age)) {
        return false;
    }

    std::string gender;
    std::cout << "Gender (M/F/O): ";

    if (!std::getline(std::cin, gender)) {
        return false;
    }

    if (gender.size() != 1) {
        std::cout << "Enter M, F, or O.\n";
        return false;
    }

    passenger.gender = static_cast<char>(
        std::toupper(static_cast<unsigned char>(gender[0]))
    );

    std::cout << "Mobile number: ";
    if (!std::getline(std::cin, passenger.contact)) {
        return false;
    }

    return true;
}

// B07: Display the complete confirmation after a successful booking.
void handleBooking(
    BookingModule& bookingModule,
    const TrainModule& trainModule
) {
    int trainNo = 0;

    if (!readInteger("Train number: ", trainNo)) {
        return;
    }

    if (trainNo <= 0) {
        std::cout << "Train number must be positive.\n";
        return;
    }

    Passenger passenger;

    if (!readPassenger(passenger)) {
        if (!std::cin) {
            return;
        }

        std::cout << "Invalid passenger input. Please try again.\n";
        return;
    }

    Booking createdBooking;

    const Status status = bookingModule.createBooking(
        trainNo,
        passenger,
        createdBooking
    );

    if (status == Status::OK) {
        Train bookedTrain;

        const bool trainFound =
            trainModule.findByNumber(
                createdBooking.trainNo,
                bookedTrain
            ) == Status::OK;

        std::cout
            << "\n========================================\n"
            << "           BOOKING CONFIRMED\n"
            << "========================================\n"
            << "PNR           : " << createdBooking.pnr << '\n'
            << "Passenger     : "
            << createdBooking.passenger.name << '\n'
            << "Age           : "
            << createdBooking.passenger.age << '\n'
            << "Gender        : "
            << createdBooking.passenger.gender << '\n'
            << "Contact       : "
            << createdBooking.passenger.contact << '\n'
            << "Train Number  : "
            << createdBooking.trainNo << '\n';

        if (trainFound) {
            std::cout
                << "Train Name    : " << bookedTrain.name << '\n'
                << "Source        : " << bookedTrain.source << '\n'
                << "Destination   : " << bookedTrain.destination << '\n'
                << "Departure     : " << bookedTrain.departure << '\n'
                << "Arrival       : " << bookedTrain.arrival << '\n';
        }

        std::cout
            << "Seat Number   : " << createdBooking.seatNo << '\n'
            << "Booking Date  : " << createdBooking.bookingDate << '\n'
            << "Fare          : Rs. "
            << std::fixed << std::setprecision(2)
            << createdBooking.fare << '\n'
            << "Status        : " << createdBooking.status << '\n'
            << "========================================\n";

    } else if (status == Status::INVALID_INPUT) {
        std::cout
            << "Invalid train number or passenger details.\n";

    } else if (status == Status::TRAIN_NOT_FOUND) {
        std::cout
            << "Train not found. Check the train number.\n";

    } else if (status == Status::NO_SEATS) {
        std::cout
            << "No seats are available on this train.\n";

    } else if (status == Status::PNR_LIMIT_REACHED) {
        std::cout
            << "Booking could not be completed because no new PNRs are available.\n";

    } else if (status == Status::FILE_ERROR) {
        std::cout
            << "Booking could not be saved. Please retry.\n";

    } else {
        std::cout
            << "Booking failed. Status code: "
            << static_cast<int>(status) << '\n';
    }
}

// B08: Display the details of an existing booking.
void displayBookingDetails(
    const Booking& booking,
    const TrainModule& trainModule
) {
    std::cout
        << "\n========================================\n"
        << "           BOOKING DETAILS\n"
        << "========================================\n"
        << "PNR           : " << booking.pnr << '\n'
        << "Passenger     : " << booking.passenger.name << '\n'
        << "Age           : " << booking.passenger.age << '\n'
        << "Gender        : " << booking.passenger.gender << '\n'
        << "Contact       : " << booking.passenger.contact << '\n'
        << "Train Number  : " << booking.trainNo << '\n';

    Train train;

    if (trainModule.findByNumber(booking.trainNo, train) == Status::OK) {
        std::cout
            << "Train Name    : " << train.name << '\n'
            << "Source        : " << train.source << '\n'
            << "Destination   : " << train.destination << '\n'
            << "Departure     : " << train.departure << '\n'
            << "Arrival       : " << train.arrival << '\n';
    }

    std::cout
        << "Seat Number   : " << booking.seatNo << '\n'
        << "Booking Date  : " << booking.bookingDate << '\n'
        << "Fare          : Rs. "
        << std::fixed << std::setprecision(2)
        << booking.fare << '\n'
        << "Status        : " << booking.status << '\n';

    if (booking.status == "CANCELLED") {
        std::cout
            << "Refund        : Rs. "
            << std::fixed << std::setprecision(2)
            << booking.refund << '\n';
    }

    std::cout << "========================================\n";
}

// B08: Retrieve a booking using its PNR.
void handleViewBooking(
    const TicketModule& tickets,
    const TrainModule& trainModule
) {
    std::string pnr;

    std::cout << "Enter PNR (10 digits): ";

    if (!std::getline(std::cin, pnr)) {
        return;
    }

    Booking booking;
    const Status status = tickets.find(pnr, booking);

    if (status == Status::OK) {
        displayBookingDetails(booking, trainModule);

    } else if (status == Status::INVALID_PNR) {
        std::cout
            << "Invalid PNR. Please enter a valid 10-digit PNR.\n";

    } else if (status == Status::PNR_NOT_FOUND) {
        std::cout
            << "No booking found for the entered PNR.\n";

    } else {
        std::cout
            << "Unable to retrieve booking details.\n";
    }
}
// B09: Cancel a booking using its PNR.
void handleCancelBooking(
    BookingModule& bookingModule,
    const TicketModule& tickets
) {
    std::string pnr;

    std::cout << "Enter PNR to cancel (10 digits): ";

    if (!std::getline(std::cin, pnr)) {
        return;
    }

    const Status status = bookingModule.cancelBooking(pnr);

    if (status == Status::OK) {
        std::cout
            << "Booking cancelled successfully.\n"
            << "The seat has been released for future bookings.\n";

        Booking cancelledBooking;
        if (tickets.find(pnr, cancelledBooking) == Status::OK) {
            std::cout
                << std::fixed << std::setprecision(2)
                << "Original fare: Rs. "
                << cancelledBooking.fare << '\n'
                << "Cancellation charge (10%): Rs. "
                << cancelledBooking.fare * 0.10 << '\n'
                << "Refund amount (90%): Rs. "
                << cancelledBooking.refund << '\n';
        }

    } else if (status == Status::INVALID_PNR) {
        std::cout
            << "Invalid PNR. Please enter a valid 10-digit PNR.\n";

    } else if (status == Status::PNR_NOT_FOUND) {
        std::cout
            << "No booking found for the entered PNR.\n";

    } else if (status == Status::ALREADY_CANCELLED) {
        std::cout
            << "This booking has already been cancelled.\n";

    } else if (status == Status::TRAIN_NOT_FOUND) {
        std::cout
            << "The associated train could not be found. "
            << "The booking was not cancelled.\n";

    } else if (status == Status::FILE_ERROR) {
        std::cout
            << "Cancellation could not be saved. Please try again.\n";

    } else {
        std::cout
            << "Cancellation failed. Please try again.\n";
    }
}
void runAdminMenu(AdminModule& admin, const TrainModule& trains) {
    while (admin.isAuthenticated()) {
        std::cout << "\nAdministrator Menu\n"
                  << "1. Add train\n"
                  << "2. Modify train\n"
                  << "0. Logout\n";

        int choice = -1;

        if (!readInteger("Choose an option: ", choice)) {
            if (!std::cin) {
                break;
            }

            continue;
        }

        if (choice == 0) {
            break;
        } else if (choice == 1) {
            handleAddTrain(admin);
        } else if (choice == 2) {
            handleModifyTrain(admin, trains);
        } else {
            std::cout << "Choose 0, 1, or 2.\n";
        }
    }

    // Always end the session when leaving the administrator menu.
    admin.logout();
    std::cout << "Logged out.\n";
}

void handleAdminLogin(AdminModule& admin, const TrainModule& trains) {
    for (int attempt = 1;
         attempt <= AdminModule::kMaxLoginAttempts;
         ++attempt) {
        std::string username;
        std::string password;

        std::cout << "Administrator username: ";
        if (!std::getline(std::cin, username)) {
            return;
        }

        std::cout << "Password: ";
        if (!std::getline(std::cin, password)) {
            return;
        }

        if (admin.login(username, password) == Status::OK) {
            std::cout << "Login successful.\n";
            runAdminMenu(admin, trains);
            return;
        }

        const int remaining = AdminModule::kMaxLoginAttempts - attempt;
        if (remaining > 0) {
            std::cout << "Invalid credentials. " << remaining
                      << " attempt(s) remaining.\n";
        }
    }

    std::cout << "Too many failed login attempts. "
              << "Returning to the main menu.\n";
}

}  // namespace

void UserInterface::run() {
    FileModule files;

    std::vector<Train> trains;
    std::vector<Booking> bookings;

    const LoadReport report = files.loadAll(trains, bookings);

    TrainModule trainModule(std::move(trains));
    SearchModule search(trainModule);

    AdminModule admin(trainModule, bookings, files);
    ValidationModule validation;
    FareModule fares(trainModule);
    TicketModule tickets(bookings);

    BookingModule bookingModule(
        trainModule,
        bookings,
        validation,
        fares,
        tickets,
        files
    );

    std::cout << "Railway Reservation System\n"
              << "Loaded " << report.trainsLoaded
              << " train record(s) and "
              << report.bookingsLoaded
              << " booking record(s).";

    if (report.linesSkipped > 0) {
        std::cout << " Skipped " << report.linesSkipped
                  << " malformed record(s).";
    }

    std::cout << '\n';

    while (true) {
        std::cout << "\n1. View all trains\n"
                  << "2. Search by source and destination\n"
                  << "3. Search by train number\n"
                  << "4. View train details and seat availability\n"
                  << "5. Book a ticket\n"
                  << "6. View booking by PNR\n"
		  << "7. Cancel booking\n"
		  << "8. Administrator login\n"
                  << "0. Exit\n";

        int choice = -1;

        if (!readInteger("Choose an option: ", choice)) {
            if (!std::cin) {
                return;
            }

            continue;
        }

        if (choice == 0) {
            return;
        }

        if (choice == 1) {
            displayTrains(trainModule.list());

        } else if (choice == 2) {
            std::string source;
            std::string destination;

            std::cout << "Source (blank = any): ";
            if (!std::getline(std::cin, source)) {
                return;
            }

            std::cout << "Destination (blank = any): ";
            if (!std::getline(std::cin, destination)) {
                return;
            }

            if (source.find('|') != std::string::npos ||
                destination.find('|') != std::string::npos ||
                (source.empty() && destination.empty())) {
                std::cout
                    << "Enter a source, a destination, or both, "
                    << "without the '|' character.\n";
                continue;
            }

            displayTrains(search.bySourceDest(source, destination));

        } else if (choice == 3) {
            int trainNo = 0;

            if (!readInteger("Train number: ", trainNo)) {
                continue;
            }

            if (trainNo <= 0) {
                std::cout
                    << "Train number must be a positive whole number.\n";
                continue;
            }

            displayTrains(search.byTrainNo(trainNo));

        } else if (choice == 4) {
            int trainNo = 0;

            if (!readInteger("Train number: ", trainNo)) {
                continue;
            }

            if (trainNo <= 0) {
                std::cout
                    << "Train number must be a positive whole number.\n";
                continue;
            }

            Train train;

            if (trainModule.findByNumber(trainNo, train) != Status::OK) {
                std::cout << "Train " << trainNo
                          << " was not found. Check the train number "
                          << "and try again.\n";
                continue;
            }

            displayTrainDetails(train);

        } else if (choice == 5) {
            handleBooking(bookingModule, trainModule);

            if (!std::cin) {
                return;
            }

        } else if (choice == 6) {
            handleViewBooking(tickets, trainModule);

            if (!std::cin) {
                return;
            }
        } else if (choice == 7) {
            handleCancelBooking(bookingModule, tickets);

            if (!std::cin) {
                return;
            }

        } else if (choice == 8) {
            handleAdminLogin(admin, trainModule);

            if (!std::cin) {
                return;
            }

        } else {
            std::cout
                << "Choose 0, 1, 2, 3, 4, 5, 6, 7, or 8.\n";
        }
    }
}

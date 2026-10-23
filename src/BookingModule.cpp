
#include "BookingModule.h"

#include <atomic>
#include <chrono>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <set>
#include <sstream>
#include <string>
#include <utility>

namespace {

bool pnrExists(const std::vector<Booking>& bookings,
               const std::string& pnr) {
    for (const Booking& booking : bookings) {
        if (booking.pnr == pnr) {
            return true;
        }
    }
    return false;
}

std::string generateFallbackPNR(
    const std::vector<Booking>& bookings
) {
    static std::atomic<unsigned long long> sequence{0};

    for (int attempt = 0; attempt < 1000; ++attempt) {
        const auto now =
            std::chrono::steady_clock::now()
                .time_since_epoch()
                .count();

        const unsigned long long value =
            (static_cast<unsigned long long>(now) +
             sequence.fetch_add(1)) % 10000000000ULL;

        std::ostringstream output;
        output << std::setw(10) << std::setfill('0') << value;

        const std::string candidate = output.str();

        if (!pnrExists(bookings, candidate)) {
            return candidate;
        }
    }

    return {};
}

std::string currentDate() {
    const std::time_t now = std::time(nullptr);
    const std::tm* local = std::localtime(&now);

    if (local == nullptr) {
        return "1970-01-01";
    }

    std::ostringstream output;
    output << std::put_time(local, "%Y-%m-%d");
    return output.str();
}

std::set<int> occupiedSeats(
    const std::vector<Booking>& bookings,
    int trainNo,
    int totalSeats
) {
    std::set<int> occupied;

    for (const Booking& booking : bookings) {
        if (booking.trainNo == trainNo &&
            booking.status == "CONFIRMED" &&
            booking.seatNo >= 1 &&
            booking.seatNo <= totalSeats) {
            occupied.insert(booking.seatNo);
        }
    }

    return occupied;
}

}  // namespace

BookingModule::BookingModule(
    TrainModule& trains,
    std::vector<Booking>& bookings,
    const ValidationModule& validation,
    const FareModule& fares,
    const TicketModule& tickets,
    FileModule& files
)
    : trains_(trains),
      bookings_(bookings),
      validation_(validation),
      fares_(fares),
      tickets_(tickets),
      files_(files) {
}

Status BookingModule::createBooking(
    int trainNo,
    const Passenger& passenger,
    Booking& createdBooking
) {
    // 1. Validate the train number and passenger details.
    if (validation_.validateTrainNo(std::to_string(trainNo)) !=
        Status::OK) {
        return Status::INVALID_INPUT;
    }

    if (validation_.validatePassenger(passenger) != Status::OK) {
        return Status::INVALID_INPUT;
    }

    // 2. Confirm that the train exists.
    Train train;
    const Status trainStatus =
        trains_.findByNumber(trainNo, train);

    if (trainStatus != Status::OK) {
        return Status::TRAIN_NOT_FOUND;
    }

    if (train.totalSeats <= 0) {
        return Status::NO_SEATS;
    }

    // 3. Calculate occupied seats from confirmed bookings.
    const std::set<int> occupied =
        occupiedSeats(bookings_, trainNo, train.totalSeats);

    if (static_cast<int>(occupied.size()) >= train.totalSeats) {
        train.availableSeats = 0;
        trains_.modify(trainNo, train);
        return Status::NO_SEATS;
    }

    // 4. Allocate the lowest available seat number.
    int seatNo = 0;

    for (int candidate = 1; candidate <= train.totalSeats; ++candidate) {
        if (occupied.find(candidate) == occupied.end()) {
            seatNo = candidate;
            break;
        }
    }

    if (seatNo == 0) {
        return Status::NO_SEATS;
    }

    // 5. Use the ticket module's PNR generator when available.
    std::string pnr = tickets_.generatePNR();

    if (validation_.validatePNR(pnr) != Status::OK ||
        pnrExists(bookings_, pnr)) {
        pnr = generateFallbackPNR(bookings_);
    }

    if (pnr.empty() ||
        validation_.validatePNR(pnr) != Status::OK ||
        pnrExists(bookings_, pnr)) {
        return Status::PNR_LIMIT_REACHED;
    }

    // 6. Calculate the fare. Fall back to the train's stored fare
    // while FareModule is still a stub.
    double fare = fares_.calculateFare(trainNo);

    if (!std::isfinite(fare) || fare < 0.0) {
        return Status::INVALID_INPUT;
    }

    if (fare == 0.0 && train.fare > 0.0) {
        fare = train.fare;
    }

    // 7. Prepare the booking record.
    Booking candidate;
    candidate.pnr = pnr;
    candidate.trainNo = trainNo;
    candidate.passenger = passenger;
    candidate.seatNo = seatNo;
    candidate.fare = fare;
    candidate.bookingDate = currentDate();
    candidate.status = "CONFIRMED";
    candidate.refund = 0.0;

    // 8. Persist the complete booking list before reporting success.
    bookings_.push_back(candidate);

    const Status saveStatus = files_.saveBookings(bookings_);

    if (saveStatus != Status::OK) {
        bookings_.pop_back();
        return Status::FILE_ERROR;
    }

    // 9. Keep in-memory seat availability consistent.
    train.availableSeats =
        train.totalSeats - static_cast<int>(occupied.size()) - 1;

    const Status updateStatus = trains_.modify(trainNo, train);

    if (updateStatus != Status::OK) {
        // The train was found earlier, so this should not normally occur.
        // Do not claim the booking failed after it has been persisted.
        createdBooking = candidate;
        return Status::OK;
    }

    createdBooking = candidate;
    return Status::OK;
}

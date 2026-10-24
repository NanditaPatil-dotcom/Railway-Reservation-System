#include "TicketModule.h"

#include <algorithm>
#include <cctype>
#include <string>

namespace {

bool isValidPNRFormat(const std::string& pnr) {
    return pnr.size() == 10 &&
        std::all_of(
            pnr.begin(),
            pnr.end(),
            [](unsigned char c) {
                return std::isdigit(c) != 0;
            }
        );
}

}  // namespace

TicketModule::TicketModule(std::vector<Booking>& bookings)
    : bookings_(bookings) {
}

std::string TicketModule::generatePNR() const {
    constexpr unsigned long long FIRST_PNR = 1000000001ULL;
    constexpr unsigned long long MAX_PNR = 9999999999ULL;

    unsigned long long highestPNR = FIRST_PNR - 1;

    for (const Booking& booking : bookings_) {
        if (!isValidPNRFormat(booking.pnr)) {
            continue;
        }

        try {
            const unsigned long long value =
                std::stoull(booking.pnr);

            if (value > highestPNR && value <= MAX_PNR) {
                highestPNR = value;
            }
        } catch (...) {
            // Ignore invalid stored PNR values.
        }
    }

    if (highestPNR >= MAX_PNR) {
        return {};
    }

    unsigned long long candidate = highestPNR + 1;

    while (candidate <= MAX_PNR) {
        const std::string pnr = std::to_string(candidate);

        const auto existing = std::find_if(
            bookings_.begin(),
            bookings_.end(),
            [&pnr](const Booking& booking) {
                return booking.pnr == pnr;
            }
        );

        if (existing == bookings_.end()) {
            return pnr;
        }

        ++candidate;
    }

    return {};
}

Status TicketModule::find(
    const std::string& pnr,
    Booking& booking
) const {
    if (!isValidPNRFormat(pnr)) {
        return Status::INVALID_PNR;
    }

    for (const Booking& saved : bookings_) {
        if (saved.pnr == pnr) {
            booking = saved;
            return Status::OK;
        }
    }

    return Status::PNR_NOT_FOUND;
}

Status TicketModule::setStatus(
    const std::string& pnr,
    const std::string& status
) {
    if (!isValidPNRFormat(pnr)) {
        return Status::INVALID_PNR;
    }

    if (status != "CONFIRMED" && status != "CANCELLED") {
        return Status::INVALID_INPUT;
    }

    for (Booking& booking : bookings_) {
        if (booking.pnr == pnr) {
            booking.status = status;
            return Status::OK;
        }
    }

    return Status::PNR_NOT_FOUND;
}

const std::vector<Booking>& TicketModule::listBookings() const {
    return bookings_;
}
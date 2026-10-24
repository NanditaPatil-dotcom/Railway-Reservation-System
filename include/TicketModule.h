#pragma once

#include "Models.h"
#include "Status.h"

#include <string>
#include <vector>

class TicketModule {
public:
    explicit TicketModule(std::vector<Booking>& bookings);

    std::string generatePNR() const;

    Status find(const std::string& pnr, Booking& booking) const;

    Status setStatus(
        const std::string& pnr,
        const std::string& status
    );

    const std::vector<Booking>& listBookings() const;

private:
    std::vector<Booking>& bookings_;
};
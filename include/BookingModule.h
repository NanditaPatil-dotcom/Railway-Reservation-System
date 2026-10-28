
#pragma once

#include "Models.h"
#include "Status.h"
#include "TrainModule.h"
#include "ValidationModule.h"
#include "FareModule.h"
#include "TicketModule.h"
#include "FileModule.h"

#include <vector>
#include <string>

class BookingModule {
public:
    BookingModule(
        TrainModule& trains,
        std::vector<Booking>& bookings,
        const ValidationModule& validation,
        const FareModule& fares,
        TicketModule& tickets,
        FileModule& files
    );

    Status createBooking(
        int trainNo,
        const Passenger& passenger,
        Booking& createdBooking
    );
    Status cancelBooking(const std::string& pnr);

private:
    TrainModule& trains_;
    std::vector<Booking>& bookings_;
    const ValidationModule& validation_;
    const FareModule& fares_;
    TicketModule& tickets_;
    FileModule& files_;
};

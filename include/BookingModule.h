
#pragma once

#include "Models.h"
#include "Status.h"
#include "TrainModule.h"
#include "ValidationModule.h"
#include "FareModule.h"
#include "TicketModule.h"
#include "FileModule.h"

#include <vector>

class BookingModule {
public:
    BookingModule(
        TrainModule& trains,
        std::vector<Booking>& bookings,
        const ValidationModule& validation,
        const FareModule& fares,
        const TicketModule& tickets,
        FileModule& files
    );

    Status createBooking(
        int trainNo,
        const Passenger& passenger,
        Booking& createdBooking
    );

private:
    TrainModule& trains_;
    std::vector<Booking>& bookings_;
    const ValidationModule& validation_;
    const FareModule& fares_;
    const TicketModule& tickets_;
    FileModule& files_;
};

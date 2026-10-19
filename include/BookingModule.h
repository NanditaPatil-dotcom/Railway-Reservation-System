#pragma once

#include "Models.h"
#include "Status.h"

class BookingModule {
public:
    Status createBooking(
        int trainNo,
        const Passenger& passenger,
        Booking& createdBooking
    );
};
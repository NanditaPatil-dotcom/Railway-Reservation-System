#pragma once

#include "Models.h"
#include "Status.h"

#include <vector>

class FileModule {
public:
    LoadReport loadAll(
        std::vector<Train>& trains,
        std::vector<Booking>& bookings
    );

    Status saveTrains(const std::vector<Train>& trains);

    Status saveBookings(const std::vector<Booking>& bookings);
};

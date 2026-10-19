#pragma once

#include "Models.h"
#include "Status.h"

#include <vector>

class FileModule {
public:
    LoadReport loadAll();

    Status saveTrains(const std::vector<Train>& trains);

    Status saveBookings(const std::vector<Booking>& bookings);
};
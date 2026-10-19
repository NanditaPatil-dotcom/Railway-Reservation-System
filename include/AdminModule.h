#pragma once

#include "Models.h"
#include "Status.h"

#include <string>
#include <vector>

class AdminModule {
public:
    Status login(
        const std::string& username,
        const std::string& password
    );

    Status addTrain(const Train& train);

    Status modifyTrain(
        int trainNo,
        const Train& updatedTrain
    );

    Status deleteTrain(int trainNo);

    Status viewTrains(std::vector<Train>& trains) const;

    Status viewBookings(std::vector<Booking>& bookings) const;
};
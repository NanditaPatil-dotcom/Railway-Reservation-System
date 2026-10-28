#pragma once

#include "Models.h"
#include "Status.h"
#include "FileModule.h"
#include "TrainModule.h"

#include <string>
#include <vector>

class AdminModule {
public:
    // Number of failed logins allowed before the user is returned to the
    // main menu (SAD requirement). The UI uses this to limit attempts.
    static constexpr int kMaxLoginAttempts = 3;

    AdminModule() = default;
    AdminModule(
        TrainModule& trains,
        std::vector<Booking>& bookings,
        FileModule& files
    );

    // Returns OK and starts an administrator session when the credentials
    // match; otherwise returns NOT_AUTHENTICATED and leaves the module
    // logged out.
    Status login(
        const std::string& username,
        const std::string& password
    );

    void logout();

    bool isAuthenticated() const;

    Status addTrain(const Train& train);

    Status modifyTrain(
        int trainNo,
        const Train& updatedTrain
    );

    Status deleteTrain(int trainNo);

    Status viewTrains(std::vector<Train>& trains) const;

    Status viewBookings(std::vector<Booking>& bookings) const;

private:
    // OK when an administrator is logged in, otherwise NOT_AUTHENTICATED.
    // Every administrative operation must call this before doing any work.
    Status requireLogin() const;

    bool authenticated_ = false;
    TrainModule* trains_ = nullptr;
    std::vector<Booking>* bookings_ = nullptr;
    FileModule* files_ = nullptr;
};

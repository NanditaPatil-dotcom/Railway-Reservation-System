#include "AdminModule.h"

#include <cstddef>

namespace {

// Credentials for the console application's single administrator account.
const std::string kAdminUsername = "admin";
const std::string kAdminPassword = "Admin@123";

// Compares two strings without stopping at the first difference, so the
// time taken does not reveal how much of a guess was correct.
bool constantTimeEquals(const std::string& left, const std::string& right) {
    unsigned char difference = (left.size() == right.size()) ? 0 : 1;
    const std::size_t common =
        (left.size() < right.size()) ? left.size() : right.size();

    for (std::size_t i = 0; i < common; ++i) {
        difference = static_cast<unsigned char>(
            difference | static_cast<unsigned char>(left[i] ^ right[i])
        );
    }

    return difference == 0;
}

}  // namespace

Status AdminModule::login(
    const std::string& username,
    const std::string& password
) {
    // Any login attempt ends the previous session first, so a failed
    // attempt can never leave an old session open.
    authenticated_ = false;

    // Check both fields every time so the result does not reveal which
    // one was wrong.
    const bool usernameMatches = constantTimeEquals(username, kAdminUsername);
    const bool passwordMatches = constantTimeEquals(password, kAdminPassword);

    if (usernameMatches && passwordMatches) {
        authenticated_ = true;
        return Status::OK;
    }

    return Status::NOT_AUTHENTICATED;
}

void AdminModule::logout() {
    authenticated_ = false;
}

bool AdminModule::isAuthenticated() const {
    return authenticated_;
}

Status AdminModule::requireLogin() const {
    return authenticated_ ? Status::OK : Status::NOT_AUTHENTICATED;
}

// Administrative operations. Each one is denied unless an administrator is
// logged in, even when called directly instead of through the menu. The
// work itself belongs to B12 (add/modify trains) and B13 (delete trains,
// view trains and bookings); until then a logged-in call reports
// NOT_IMPLEMENTED. Keep the requireLogin() check when replacing the bodies.

Status AdminModule::addTrain(const Train&) {
    const Status access = requireLogin();
    if (access != Status::OK) {
        return access;
    }

    return Status::NOT_IMPLEMENTED;  // B12
}

Status AdminModule::modifyTrain(int, const Train&) {
    const Status access = requireLogin();
    if (access != Status::OK) {
        return access;
    }

    return Status::NOT_IMPLEMENTED;  // B12
}

Status AdminModule::deleteTrain(int) {
    const Status access = requireLogin();
    if (access != Status::OK) {
        return access;
    }

    return Status::NOT_IMPLEMENTED;  // B13
}

Status AdminModule::viewTrains(std::vector<Train>&) const {
    const Status access = requireLogin();
    if (access != Status::OK) {
        return access;
    }

    return Status::NOT_IMPLEMENTED;  // B13
}

Status AdminModule::viewBookings(std::vector<Booking>&) const {
    const Status access = requireLogin();
    if (access != Status::OK) {
        return access;
    }

    return Status::NOT_IMPLEMENTED;  // B13
}

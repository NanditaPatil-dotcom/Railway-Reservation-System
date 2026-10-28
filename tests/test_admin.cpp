// Module-level tests for AdminModule (B11): login, denial, session
// handling, and protection of the administrative operations.
// Build and run from the repository root (one command):
//   g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude tests/test_admin.cpp src/AdminModule.cpp -o test_admin && ./test_admin

#include "AdminModule.h"

#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace {

int failures = 0;

void check(bool condition, const std::string& label) {
    if (!condition) {
        ++failures;
        std::cout << "FAIL: " << label << '\n';
    }
}

const std::string kUser = "admin";
const std::string kPass = "Admin@123";

// True when every administrative operation is refused with
// NOT_AUTHENTICATED.
bool allOperationsDenied(AdminModule& admin) {
    Train train;
    train.trainNo = 99999;
    std::vector<Train> trains;
    std::vector<Booking> bookings;

    return admin.addTrain(train) == Status::NOT_AUTHENTICATED &&
           admin.modifyTrain(99999, train) == Status::NOT_AUTHENTICATED &&
           admin.deleteTrain(99999) == Status::NOT_AUTHENTICATED &&
           admin.viewTrains(trains) == Status::NOT_AUTHENTICATED &&
           admin.viewBookings(bookings) == Status::NOT_AUTHENTICATED;
}

// True when no administrative operation is refused for lack of login.
bool noOperationDenied(AdminModule& admin) {
    Train train;
    train.trainNo = 99999;
    std::vector<Train> trains;
    std::vector<Booking> bookings;

    return admin.addTrain(train) != Status::NOT_AUTHENTICATED &&
           admin.modifyTrain(99999, train) != Status::NOT_AUTHENTICATED &&
           admin.deleteTrain(99999) != Status::NOT_AUTHENTICATED &&
           admin.viewTrains(trains) != Status::NOT_AUTHENTICATED &&
           admin.viewBookings(bookings) != Status::NOT_AUTHENTICATED;
}

}  // namespace

int main() {
    // ---- Valid credentials allow access ----
    {
        AdminModule admin;
        check(!admin.isAuthenticated(), "new module starts logged out");
        check(admin.login(kUser, kPass) == Status::OK, "valid login succeeds");
        check(admin.isAuthenticated(), "session active after valid login");
    }

    // ---- Invalid credentials deny access ----
    const std::vector<std::pair<std::string, std::string>> badLogins = {
        {kUser, "wrong"},
        {"root", kPass},
        {"root", "wrong"},
        {"", ""},
        {kUser, ""},
        {"", kPass},
        {"Admin", kPass},                 // username is case-sensitive
        {kUser, "admin@123"},             // password is case-sensitive
        {kUser, kPass + "x"},             // extra character
        {kUser, kPass.substr(0, 8)},      // truncated password
        {" " + kUser, kPass},             // leading space
        {kUser, kPass + " "},             // trailing space
        {kUser + "|" + kPass, kPass},     // delimiter injection
    };
    for (const auto& attempt : badLogins) {
        AdminModule admin;
        check(
            admin.login(attempt.first, attempt.second) ==
                Status::NOT_AUTHENTICATED,
            "bad login rejected: [" + attempt.first + "] / [" +
                attempt.second + "]"
        );
        check(!admin.isAuthenticated(), "no session after bad login");
    }

    // ---- Operations are protected outside the menu flow ----
    {
        AdminModule admin;
        check(allOperationsDenied(admin), "operations denied before any login");

        admin.login(kUser, "wrong");
        check(allOperationsDenied(admin), "operations denied after failed login");

        admin.login(kUser, kPass);
        check(noOperationDenied(admin), "operations allowed while logged in");

        admin.logout();
        check(!admin.isAuthenticated(), "logout ends the session");
        check(allOperationsDenied(admin), "operations denied after logout");
    }

    // ---- A failed attempt never leaves an old session open ----
    {
        AdminModule admin;
        admin.login(kUser, kPass);
        check(admin.login(kUser, "wrong") == Status::NOT_AUTHENTICATED,
              "failed re-login rejected");
        check(!admin.isAuthenticated(), "failed re-login ends the old session");
        check(allOperationsDenied(admin), "operations denied after failed re-login");
    }

    // ---- Logging in again after logout works ----
    {
        AdminModule admin;
        admin.login(kUser, kPass);
        admin.logout();
        check(admin.login(kUser, kPass) == Status::OK, "login works after logout");
        check(admin.isAuthenticated(), "session active after second login");
    }

    // ---- Attempt limit required by the SAD ----
    check(AdminModule::kMaxLoginAttempts == 3, "three login attempts allowed");

    if (failures == 0) {
        std::cout << "All AdminModule tests passed.\n";
        return 0;
    }
    std::cout << failures << " test(s) failed.\n";
    return 1;
}

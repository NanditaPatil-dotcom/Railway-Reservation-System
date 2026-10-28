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

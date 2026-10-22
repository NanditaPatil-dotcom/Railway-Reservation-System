#include "ValidationModule.h"

#include <cctype>
#include <cstddef>

namespace {
constexpr std::size_t kMaxNameLength = 50;
constexpr int kMinAge = 1;
constexpr int kMaxAge = 120;
constexpr std::size_t kContactLength = 10;
constexpr std::size_t kPnrLength = 10;
constexpr std::size_t kMaxIntegerDigits = 9;  // always fits in a 32-bit int

bool isDigit(char c) {
    return std::isdigit(static_cast<unsigned char>(c)) != 0;
}

bool isAlpha(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) != 0;
}

bool isAlnum(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) != 0;
}

// True if text is one or more digits and nothing else (no sign, no spaces).
// At most kMaxIntegerDigits long, so the caller can convert it safely.
bool isPlainNumber(const std::string& text) {
    if (text.empty() || text.size() > kMaxIntegerDigits) {
        return false;
    }
    for (char c : text) {
        if (!isDigit(c)) {
            return false;
        }
    }
    return true;
}

bool isValidName(const std::string& name) {
    if (name.empty() || name.size() > kMaxNameLength) {
        return false;
    }
    // Must start with a letter and end with a letter or '.' (as in "Jr."),
    // so blank and space-padded names are rejected.
    if (!isAlpha(name.front()) || !(isAlpha(name.back()) || name.back() == '.')) {
        return false;
    }
    for (char c : name) {
        const bool allowed = isAlpha(c) || c == ' ' || c == '.' ||
                             c == '-' || c == '\'';
        if (!allowed) {
            return false;
        }
    }
    return true;
}

// Indian mobile number: exactly 10 digits, first digit 6-9.
bool isValidContact(const std::string& contact) {
    if (contact.size() != kContactLength) {
        return false;
    }
    for (char c : contact) {
        if (!isDigit(c)) {
            return false;
        }
    }
    return contact.front() >= '6' && contact.front() <= '9';
}
}  // namespace

Status ValidationModule::validatePassenger(const Passenger& passenger) const {
    if (!isValidName(passenger.name)) {
        return Status::INVALID_INPUT;
    }
    if (passenger.age < kMinAge || passenger.age > kMaxAge) {
        return Status::INVALID_INPUT;
    }
    if (passenger.gender != 'M' && passenger.gender != 'F' &&
        passenger.gender != 'O') {
        return Status::INVALID_INPUT;
    }
    if (!isValidContact(passenger.contact)) {
        return Status::INVALID_INPUT;
    }
    return Status::OK;
}

Status ValidationModule::validateTrainNo(const std::string& trainNo) const {
    if (!isPlainNumber(trainNo)) {
        return Status::INVALID_INPUT;
    }
    // Train numbers are positive (FileModule rejects trainNo <= 0).
    if (std::stoi(trainNo) <= 0) {
        return Status::INVALID_INPUT;
    }
    return Status::OK;
}

Status ValidationModule::validatePNR(const std::string& pnr) const {
    // FileModule only accepts 10-character PNRs, so enforce the same length.
    if (pnr.size() != kPnrLength) {
        return Status::INVALID_PNR;
    }
    for (char c : pnr) {
        if (!isAlnum(c)) {
            return Status::INVALID_PNR;
        }
    }
    return Status::OK;
}

Status ValidationModule::readMenuChoice(
    const std::string& input,
    int min,
    int max
) const {
    if (min > max || !isPlainNumber(input)) {
        return Status::INVALID_INPUT;
    }
    const int choice = std::stoi(input);
    if (choice < min || choice > max) {
        return Status::INVALID_INPUT;
    }
    return Status::OK;
}

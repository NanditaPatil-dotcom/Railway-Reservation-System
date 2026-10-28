#include "AdminModule.h"

#include <cmath>
#include <cstddef>
#include <utility>

namespace {

// Credentials for the console application's single administrator account.
const std::string kAdminUsername = "admin";
const std::string kAdminPassword = "Admin@123";

bool isPersistableTrain(const Train& train) {
    const std::string* fields[] = {
        &train.name, &train.source, &train.destination,
        &train.departure, &train.arrival
    };
    if (train.trainNo <= 0 || train.fare < 0.0 || !std::isfinite(train.fare) ||
        train.totalSeats < 0) {
        return false;
    }
    for (const std::string* field : fields) {
        if (field->empty() || field->find('|') != std::string::npos ||
            field->find('\n') != std::string::npos ||
            field->find('\r') != std::string::npos) {
            return false;
        }
    }
    return true;
}

int confirmedBookingsFor(const std::vector<Booking>& bookings, int trainNo) {
    int confirmed = 0;
    for (const Booking& booking : bookings) {
        if (booking.trainNo == trainNo && booking.status == "CONFIRMED") {
            ++confirmed;
        }
    }
    return confirmed;
}

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

AdminModule::AdminModule(
    TrainModule& trains,
    std::vector<Booking>& bookings,
    FileModule& files
)
    : trains_(&trains), bookings_(&bookings), files_(&files) {
}

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

// Administrative operations are denied unless an administrator is logged in,
// even when called directly instead of through the menu.

Status AdminModule::addTrain(const Train& train) {
    const Status access = requireLogin();
    if (access != Status::OK) {
        return access;
    }

    if (trains_ == nullptr || files_ == nullptr) {
        return Status::NOT_IMPLEMENTED;
    }
    if (!isPersistableTrain(train)) {
        return Status::INVALID_INPUT;
    }

    Train candidate = train;
    candidate.availableSeats = candidate.totalSeats;
    const Status addStatus = trains_->add(candidate);
    if (addStatus != Status::OK) {
        return addStatus;
    }
    if (files_->saveTrains(trains_->list()) != Status::OK) {
        trains_->remove(candidate.trainNo);
        return Status::FILE_ERROR;
    }
    return Status::OK;
}

Status AdminModule::modifyTrain(int trainNo, const Train& updatedTrain) {
    const Status access = requireLogin();
    if (access != Status::OK) {
        return access;
    }

    if (trains_ == nullptr || bookings_ == nullptr || files_ == nullptr) {
        return Status::NOT_IMPLEMENTED;
    }
    if (trainNo <= 0 || updatedTrain.trainNo != trainNo ||
        !isPersistableTrain(updatedTrain)) {
        return Status::INVALID_INPUT;
    }

    Train current;
    if (trains_->findByNumber(trainNo, current) != Status::OK) {
        return Status::TRAIN_NOT_FOUND;
    }

    const int confirmed = confirmedBookingsFor(*bookings_, trainNo);
    if (updatedTrain.totalSeats < confirmed) {
        return Status::INVALID_CAPACITY;
    }

    std::vector<Train> updatedTrains = trains_->list();
    for (Train& train : updatedTrains) {
        if (train.trainNo == trainNo) {
            train = updatedTrain;
            train.availableSeats = updatedTrain.totalSeats - confirmed;
            break;
        }
    }

    if (files_->saveTrains(updatedTrains) != Status::OK) {
        return Status::FILE_ERROR;
    }
    trains_->setTrains(std::move(updatedTrains));
    return Status::OK;
}

Status AdminModule::deleteTrain(int) {
    const Status access = requireLogin();
    if (access != Status::OK) {
        return access;
    }

    return Status::NOT_IMPLEMENTED;  // B13
}

Status AdminModule::viewTrains(std::vector<Train>& trains) const {
    const Status access = requireLogin();
    if (access != Status::OK) {
        return access;
    }

    if (trains_ == nullptr) {
        return Status::NOT_IMPLEMENTED;
    }
    trains = trains_->list();
    return Status::OK;
}

Status AdminModule::viewBookings(std::vector<Booking>& bookings) const {
    const Status access = requireLogin();
    if (access != Status::OK) {
        return access;
    }

    if (bookings_ == nullptr) {
        return Status::NOT_IMPLEMENTED;
    }
    bookings = *bookings_;
    return Status::OK;
}

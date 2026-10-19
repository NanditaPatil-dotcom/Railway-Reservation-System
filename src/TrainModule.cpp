#include "TrainModule.h"

#include <algorithm>
#include <utility>

TrainModule::TrainModule(std::vector<Train> trains) {
    setTrains(std::move(trains));
}

void TrainModule::setTrains(std::vector<Train> trains) {
    trains_ = std::move(trains);
}

const std::vector<Train>& TrainModule::list() const {
    return trains_;
}

Status TrainModule::findByNumber(int trainNo, Train& train) const {
    const auto found = std::find_if(
        trains_.begin(), trains_.end(),
        [trainNo](const Train& candidate) { return candidate.trainNo == trainNo; }
    );
    if (found == trains_.end()) {
        return Status::TRAIN_NOT_FOUND;
    }
    train = *found;
    return Status::OK;
}

Status TrainModule::add(const Train& train) {
    Train existing;
    if (findByNumber(train.trainNo, existing) == Status::OK) {
        return Status::DUPLICATE_TRAIN_NO;
    }
    trains_.push_back(train);
    return Status::OK;
}

Status TrainModule::modify(int trainNo, const Train& updatedTrain) {
    for (Train& train : trains_) {
        if (train.trainNo == trainNo) {
            train = updatedTrain;
            return Status::OK;
        }
    }
    return Status::TRAIN_NOT_FOUND;
}

Status TrainModule::remove(int trainNo) {
    const auto found = std::find_if(
        trains_.begin(), trains_.end(),
        [trainNo](const Train& train) { return train.trainNo == trainNo; }
    );
    if (found == trains_.end()) {
        return Status::TRAIN_NOT_FOUND;
    }
    trains_.erase(found);
    return Status::OK;
}

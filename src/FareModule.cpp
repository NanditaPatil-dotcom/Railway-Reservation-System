#include "FareModule.h"
#include "Status.h"

FareModule::FareModule(const TrainModule& trains)
    : trains_(trains) {
}

double FareModule::calculateFare(int trainNo) const {
    Train train;

    if (trains_.findByNumber(trainNo, train) != Status::OK) {
        return -1.0;
    }

    return train.fare;
}
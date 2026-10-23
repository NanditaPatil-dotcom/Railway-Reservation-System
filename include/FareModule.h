#pragma once

#include "TrainModule.h"

class FareModule {
public:
    explicit FareModule(const TrainModule& trains);

    double calculateFare(int trainNo) const;

private:
    const TrainModule& trains_;
};
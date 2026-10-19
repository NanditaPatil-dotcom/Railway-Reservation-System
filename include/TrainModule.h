#pragma once

#include "Models.h"
#include "Status.h"

#include <vector>

class TrainModule {
public:
    const std::vector<Train>& list() const;

    Status findByNumber(int trainNo, Train& train) const;

    Status add(const Train& train);

    Status modify(int trainNo, const Train& updatedTrain);

    Status remove(int trainNo);
};
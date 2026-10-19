#pragma once

#include "Models.h"
#include "TrainModule.h"

#include <string>
#include <vector>

class SearchModule {
public:
    explicit SearchModule(const TrainModule& trains);

    std::vector<Train> bySourceDest(
        const std::string& source,
        const std::string& destination
    ) const;

    std::vector<Train> byTrainNo(int trainNo) const;

private:
    const TrainModule& trains_;
};

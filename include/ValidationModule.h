#pragma once

#include "Models.h"
#include "Status.h"

#include <string>

class ValidationModule {
public:
    Status validatePassenger(const Passenger& passenger) const;

    Status validateTrainNo(const std::string& trainNo) const;

    Status validatePNR(const std::string& pnr) const;

    Status readMenuChoice(
        const std::string& input,
        int min,
        int max
    ) const;
};

#pragma once

#include "Models.h"
#include "Status.h"

#include <string>

class CancellationModule {
public:
    Status findForCancel(
        const std::string& pnr,
        Booking& booking
    );

    Status confirmCancel(
        const std::string& pnr,
        double& refundAmount
    );
};
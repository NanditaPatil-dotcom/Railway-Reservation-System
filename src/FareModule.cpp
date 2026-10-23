#include "FareModule.h"

// Temporary fallback for B06 integration.
// B07 should implement the actual fare calculation.
double FareModule::calculateFare(int trainNo) const {
    (void)trainNo;
    return 0.0;
}
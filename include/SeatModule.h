#pragma once

#include "Status.h"

class SeatModule {
public:
    int availableCount(int trainNo) const;

    Status allocateSeat(int trainNo, int& seatNo);

    Status releaseSeat(int trainNo, int seatNo);
};
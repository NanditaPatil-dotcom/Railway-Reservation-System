#include "TicketModule.h"

#include <string>

// Temporary fallback for B06 integration.
// B07 should implement the actual PNR generation.
std::string TicketModule::generatePNR() const {
    return "";
}
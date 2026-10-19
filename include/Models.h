#pragma once

#include <string>

struct Train {
    int trainNo{};
    std::string name;
    std::string source;
    std::string destination;
    std::string departure;
    std::string arrival;
    double fare{};
    int totalSeats{};
    int availableSeats{};
};

struct Passenger {
    std::string name;
    int age{};
    char gender{};
    std::string contact;
};

struct Booking {
    std::string pnr;
    int trainNo{};
    Passenger passenger;
    int seatNo{};
    double fare{};
    std::string bookingDate;
    std::string status;
    double refund{};
};

struct LoadReport {
    int trainsLoaded{};
    int bookingsLoaded{};
    int linesSkipped{};
};

#include "FileModule.h"

#include <cstdio>
#include <fstream>
#include <iomanip>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace {
std::vector<std::string> split(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream input(line);
    std::string field;
    while (std::getline(input, field, '|')) {
        fields.push_back(field);
    }
    if (!line.empty() && line.back() == '|') {
        fields.emplace_back();
    }
    return fields;
}

template <typename Number>
bool parseNumber(const std::string& text, Number& value) {
    try {
        std::size_t used = 0;
        if constexpr (std::is_integral<Number>::value) {
            const long long parsed = std::stoll(text, &used);
            if (used != text.size() || parsed < std::numeric_limits<Number>::min() ||
                parsed > std::numeric_limits<Number>::max()) return false;
            value = static_cast<Number>(parsed);
        } else {
            const double parsed = std::stod(text, &used);
            if (used != text.size()) return false;
            value = static_cast<Number>(parsed);
        }
        return true;
    } catch (...) {
        return false;
    }
}

template <typename Writer>
Status atomicWrite(const std::string& path, Writer writer) {
    const std::string temporaryPath = path + ".tmp";
    {
        std::ofstream output(temporaryPath, std::ios::trunc);
        if (!output || !writer(output) || !output.good()) {
            std::remove(temporaryPath.c_str());
            return Status::FILE_ERROR;
        }
    }
    if (std::rename(temporaryPath.c_str(), path.c_str()) != 0) {
        std::remove(temporaryPath.c_str());
        return Status::FILE_ERROR;
    }
    return Status::OK;
}

bool ensureFileExists(const std::string& path) {
    std::ifstream input(path);
    if (input.good()) return true;
    std::ofstream create(path);
    return create.good();
}
} // namespace

LoadReport FileModule::loadAll(
    std::vector<Train>& trains,
    std::vector<Booking>& bookings
) {
    LoadReport report;
    trains.clear();
    bookings.clear();

    if (!ensureFileExists("trains.txt") || !ensureFileExists("bookings.txt")) {
        ++report.linesSkipped;
        return report;
    }

    std::string line;
    std::ifstream trainFile("trains.txt");
    while (std::getline(trainFile, line)) {
        const auto fields = split(line);
        Train train;
        if (fields.size() != 8 || !parseNumber(fields[0], train.trainNo) ||
            !parseNumber(fields[6], train.fare) ||
            !parseNumber(fields[7], train.totalSeats) || train.trainNo <= 0 ||
            train.fare < 0 || train.totalSeats < 0 || fields[1].empty() ||
            fields[2].empty() || fields[3].empty()) {
            ++report.linesSkipped;
            continue;
        }
        train.name = fields[1];
        train.source = fields[2];
        train.destination = fields[3];
        train.departure = fields[4];
        train.arrival = fields[5];
        train.availableSeats = train.totalSeats;
        trains.push_back(std::move(train));
        ++report.trainsLoaded;
    }

    std::map<int, int> confirmedByTrain;
    std::ifstream bookingFile("bookings.txt");
    while (std::getline(bookingFile, line)) {
        const auto fields = split(line);
        Booking booking;
        if (fields.size() != 11 || fields[0].size() != 10 ||
            !parseNumber(fields[1], booking.trainNo) || fields[2].empty() ||
            !parseNumber(fields[3], booking.passenger.age) ||
            fields[4].size() != 1 || !parseNumber(fields[6], booking.seatNo) ||
            !parseNumber(fields[7], booking.fare) ||
            !parseNumber(fields[10], booking.refund) ||
            (fields[9] != "CONFIRMED" && fields[9] != "CANCELLED")) {
            ++report.linesSkipped;
            continue;
        }
        booking.pnr = fields[0];
        booking.passenger.name = fields[2];
        booking.passenger.gender = fields[4][0];
        booking.passenger.contact = fields[5];
        booking.bookingDate = fields[8];
        booking.status = fields[9];
        bookings.push_back(std::move(booking));
        if (bookings.back().status == "CONFIRMED") {
            ++confirmedByTrain[bookings.back().trainNo];
        }
        ++report.bookingsLoaded;
    }

    for (Train& train : trains) {
        const int confirmed = confirmedByTrain[train.trainNo];
        train.availableSeats = confirmed >= train.totalSeats
            ? 0 : train.totalSeats - confirmed;
    }
    return report;
}

Status FileModule::saveTrains(const std::vector<Train>& trains) {
    return atomicWrite("trains.txt", [&trains](std::ofstream& output) {
        output << std::setprecision(15);
        for (const Train& train : trains) {
            output << train.trainNo << '|' << train.name << '|' << train.source << '|'
                   << train.destination << '|' << train.departure << '|' << train.arrival
                   << '|' << train.fare << '|' << train.totalSeats << '\n';
        }
        return output.good();
    });
}

Status FileModule::saveBookings(const std::vector<Booking>& bookings) {
    return atomicWrite("bookings.txt", [&bookings](std::ofstream& output) {
        output << std::setprecision(15);
        for (const Booking& booking : bookings) {
            output << booking.pnr << '|' << booking.trainNo << '|'
                   << booking.passenger.name << '|' << booking.passenger.age << '|'
                   << booking.passenger.gender << '|' << booking.passenger.contact << '|'
                   << booking.seatNo << '|' << booking.fare << '|' << booking.bookingDate
                   << '|' << booking.status << '|' << booking.refund << '\n';
        }
        return output.good();
    });
}

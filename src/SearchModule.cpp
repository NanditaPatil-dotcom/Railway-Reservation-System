#include "SearchModule.h"

#include <algorithm>
#include <cctype>

namespace {
std::string normalize(const std::string& value) {
    auto first = std::find_if_not(value.begin(), value.end(), [](unsigned char c) {
        return std::isspace(c) != 0;
    });
    auto last = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char c) {
        return std::isspace(c) != 0;
    }).base();
    if (first >= last) {
        return {};
    }
    std::string result(first, last);
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return result;
}
} // namespace

SearchModule::SearchModule(const TrainModule& trains) : trains_(trains) {}

std::vector<Train> SearchModule::bySourceDest(
    const std::string& source,
    const std::string& destination
) const {
    const std::string wantedSource = normalize(source);
    const std::string wantedDestination = normalize(destination);
    std::vector<Train> matches;
    if (wantedSource.empty() || wantedDestination.empty()) {
        return matches;
    }
    for (const Train& train : trains_.list()) {
        if (normalize(train.source) == wantedSource &&
            normalize(train.destination) == wantedDestination) {
            matches.push_back(train);
        }
    }
    return matches;
}

std::vector<Train> SearchModule::byTrainNo(int trainNo) const {
    Train train;
    if (trains_.findByNumber(trainNo, train) != Status::OK) {
        return {};
    }
    return {train};
}

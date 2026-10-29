
#!/usr/bin/env bash
set -euo pipefail

CXX="${CXX:-g++}"
CXXFLAGS=(-std=c++17 -Wall -Wextra -pedantic -Iinclude)

# Build all executables in a temporary directory.
BUILD_DIR="$(mktemp -d)"

cleanup() {
    rm -rf "$BUILD_DIR"
}
trap cleanup EXIT

echo "=== Build application ==="
"$CXX" "${CXXFLAGS[@]}" src/*.cpp -o "$BUILD_DIR/railway"

echo "=== Validation tests ==="
"$CXX" "${CXXFLAGS[@]}" tests/test_validation.cpp \
    src/ValidationModule.cpp -o "$BUILD_DIR/test_validation"
"$BUILD_DIR/test_validation"

echo "=== Booking tests ==="
"$CXX" "${CXXFLAGS[@]}" tests/test_booking.cpp \
    src/BookingModule.cpp src/FileModule.cpp src/FareModule.cpp \
    src/TicketModule.cpp src/TrainModule.cpp src/ValidationModule.cpp \
    -o "$BUILD_DIR/test_booking"
"$BUILD_DIR/test_booking"

echo "=== Admin tests ==="
"$CXX" "${CXXFLAGS[@]}" tests/test_admin.cpp \
    src/AdminModule.cpp src/TrainModule.cpp src/FileModule.cpp \
    -o "$BUILD_DIR/test_admin"
"$BUILD_DIR/test_admin"

echo "=== Admin train-management tests ==="
"$CXX" "${CXXFLAGS[@]}" tests/test_admin_trains.cpp \
    src/AdminModule.cpp src/TrainModule.cpp src/FileModule.cpp \
    -o "$BUILD_DIR/test_admin_trains"
"$BUILD_DIR/test_admin_trains"

echo "=== NFR performance tests ==="
"$CXX" "${CXXFLAGS[@]}" tests/test_nfr_performance.cpp \
    src/BookingModule.cpp src/FareModule.cpp src/FileModule.cpp \
    src/SearchModule.cpp src/TicketModule.cpp src/TrainModule.cpp \
    src/ValidationModule.cpp -o "$BUILD_DIR/test_nfr_performance"
"$BUILD_DIR/test_nfr_performance"

echo "=== NFR seat-consistency tests ==="
"$CXX" "${CXXFLAGS[@]}" tests/test_nfr_seats.cpp \
    src/BookingModule.cpp src/FareModule.cpp src/FileModule.cpp \
    src/TicketModule.cpp src/TrainModule.cpp src/ValidationModule.cpp \
    -o "$BUILD_DIR/test_nfr_seats"
"$BUILD_DIR/test_nfr_seats"

echo "=== NFR usability tests ==="
bash tests/test_nfr_usability.sh "$BUILD_DIR/railway"

echo "=== All test suites passed ==="

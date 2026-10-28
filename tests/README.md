# Module-Level Tests

The Railway Reservation System is designed with separate C++ modules so that business logic can be tested independently of the console interface.

Existing and planned module tests include:

- AdminModule: login/session protection and B12 train add/modify acceptance criteria (`test_admin.cpp`, `test_admin_trains.cpp`).

- ValidationModule: valid and invalid passenger details, train numbers, PNRs, and menu choices.
- SeatModule: available-seat counts, lowest-free-seat allocation, and seat release.
- BookingModule: successful booking, invalid passenger input, missing train, and no-seat conditions.
- CancellationModule: invalid PNR, repeated cancellation, seat release, and refund calculation.
- FileModule: loading valid records, handling malformed records, and saving persistent data.

Additional tests will be implemented as the corresponding module functionality becomes available.

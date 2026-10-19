# Module-Level Testing

The Railway Reservation System is designed with separate C++ modules so that business logic can be tested independently of the console interface.

Planned module tests include:

- ValidationModule: valid and invalid passenger details, train numbers, PNRs, and menu choices.
- SeatModule: available-seat counts, lowest-free-seat allocation, and seat release.
- BookingModule: successful booking, invalid passenger input, missing train, and no-seat conditions.
- CancellationModule: invalid PNR, repeated cancellation, seat release, and refund calculation.
- FileModule: loading valid records, handling malformed records, and saving persistent data.

Tests will be implemented as the corresponding module functionality becomes available.

# Module-Level Tests

The Railway Reservation System is designed with separate C++ modules so that business logic can be tested independently of the console interface.

The automated module tests are:

- AdminModule: login/session protection and B12 train add/modify acceptance criteria (`test_admin.cpp`, `test_admin_trains.cpp`).

- ValidationModule: valid and invalid passenger details, train numbers, PNRs, and menu choices (`test_validation.cpp`).
- BookingModule: successful booking, invalid passenger input, missing train, no-seat conditions, lowest-free-seat allocation, cancellation, repeated cancellation, seat release, refund calculation, and reload of saved records (`test_booking.cpp`).
- FileModule: loading and saving records is exercised through the booking and administrator tests; there is no separate FileModule suite.

`SeatModule` and `CancellationModule` were designed in the SAD but not implemented; their behaviour is in `BookingModule` and is tested there.

- Non-functional (B14): search performance with 10,000 trains (`test_nfr_performance.cpp`), seat consistency after booking/cancellation (`test_nfr_seats.cpp`), and menu usability (`test_nfr_usability.sh`). Results are recorded in `NFR_TEST_REPORT.md`.

Not covered by an automated test: train deletion and the administrator record views (manual cases TC-31 to TC-33 in `SYSTEM_TEST_REPORT.md`).

## Reports

| Report | Contents |
|---|---|
| `TEST_SUMMARY_REPORT.md` | Final results against the 35 Test Plan cases |
| `DEFECT_REPORT.md` | Defects found, fixed and still open |
| `SYSTEM_TEST_REPORT.md` | 42 manual system test cases |
| `INTEGRATION_TEST_REPORT.md` | Integration and regression testing (B15) |
| `NFR_TEST_REPORT.md` | Performance, seat consistency and usability (B14) |
| `automated_test_output.txt` | Output of one full automated run |

# Requirements Traceability Matrix (Final)

**Project:** Railway Ticket Reservation System, Group 15
**Build:** `70e3a0b` on `main`
**Date:** 16 November 2026

This replaces the matrix in SRS section 8, where every row was still marked
N (not implemented). It maps each requirement to the module that actually
implements it, the Test Plan case, and the evidence that the case passed.

**Status:** N = not implemented, P = partially implemented, A = accepted
(implemented and its test case passed).

**Evidence abbreviations**

| Abbreviation | Source |
|---|---|
| ST TC-nn | Manual case in `tests/SYSTEM_TEST_REPORT.md` (2 November) |
| `test_*.cpp`, `test_nfr_usability.sh` | Automated suite in `tests/`, run by `tests/run_all_tests.sh` and CI |
| QA run | Verification run of 16 November in `tests/TEST_SUMMARY_REPORT.md`, section 4 |

Where the implementing module differs from the SAD, the SAD module is shown
in brackets. See DEV-03 in `DEVIATIONS.md`.

## Functional requirements

| Req ID | Requirement | Implemented in | Test case | Evidence | Status |
|---|---|---|---|---|---|
| RT-F001 | Display all available trains | TrainModule, UserInterface | TC-TRAIN-01 | ST TC-01; `test_nfr_usability.sh` | A |
| RT-F002 | Search by source and destination | SearchModule | TC-TRAIN-02 | ST TC-02, TC-03; `test_nfr_performance.cpp` | A |
| RT-F003 | Search by train number | SearchModule | TC-TRAIN-03 | ST TC-04, TC-05; `test_nfr_performance.cpp` | A |
| RT-F004 | Display available seats | TrainModule, FileModule [SeatModule] | TC-TRAIN-04 | ST TC-06; `test_nfr_seats.cpp` | A |
| RT-F005 | Select train and enter passenger details | BookingModule, UserInterface | TC-BOOK-01 | ST TC-07; `test_booking.cpp`; `test_nfr_usability.sh` | A |
| RT-F006 | Validate passenger details | ValidationModule | TC-BOOK-02 | ST TC-09 to TC-12; `test_validation.cpp`; QA run | A (see D-07) |
| RT-F007 | Check seat availability before booking | BookingModule [SeatModule] | TC-BOOK-03 | ST TC-14; `test_booking.cpp` | A |
| RT-F008 | Assign an available seat | BookingModule [SeatModule] | TC-BOOK-04 | ST TC-07, TC-08; `test_booking.cpp` | A |
| RT-F009 | Generate unique PNR | TicketModule | TC-BOOK-05 | ST TC-07; `test_booking.cpp`; QA run | A |
| RT-F010 | Calculate and display fare | FareModule | TC-BOOK-06 | ST TC-07; `test_booking.cpp` | A |
| RT-F011 | Display booking confirmation | UserInterface, web | TC-BOOK-07 | ST TC-07, TC-36; `test_nfr_usability.sh` | A |
| RT-F012 | Search booking using PNR | TicketModule | TC-CAN-01 | ST TC-15, TC-16; `test_booking.cpp` | A |
| RT-F013 | Display booking details before cancellation | UserInterface, web | TC-CAN-02 | ST TC-17, TC-20, TC-39 | A |
| RT-F014 | Cancel valid booking after confirmation | BookingModule [CancellationModule] | TC-CAN-03 | ST TC-17, TC-20; `test_booking.cpp` | A |
| RT-F015 | Make cancelled seat available | BookingModule [SeatModule] | TC-CAN-04 | ST TC-18; `test_booking.cpp`; `test_nfr_seats.cpp` | A |
| RT-F016 | Update booking status to CANCELLED | BookingModule, TicketModule | TC-CAN-05 | ST TC-17; `test_booking.cpp` | A |
| RT-F017 | Display refund amount (90% of fare) | BookingModule [RefundModule] | TC-CAN-06 | ST TC-17, TC-39; `test_booking.cpp` | A |
| RT-F018 | View booking details using PNR | TicketModule, UserInterface | TC-MGT-01 | ST TC-15; `test_booking.cpp` | A |
| RT-F019 | Prevent duplicate cancellation | BookingModule [CancellationModule] | TC-MGT-02 | ST TC-19; `test_booking.cpp`; QA run | A |
| RT-F020 | Maintain persistent booking records | FileModule | TC-MGT-03 | ST TC-21, TC-37; `test_booking.cpp`; QA run | A |
| RT-F021 | Add train; reject duplicate numbers | AdminModule, TrainModule | TC-ADMIN-01 | ST TC-27, TC-28; `test_admin_trains.cpp` | A |
| RT-F022 | Modify train within capacity rules | AdminModule, TrainModule | TC-ADMIN-02 | ST TC-29, TC-30; `test_admin_trains.cpp` | A |
| RT-F023 | View all train and booking records | AdminModule | TC-ADMIN-03 | ST TC-33, TC-41 (manual only) | A |
| RT-F024 | Administrator authentication | AdminModule | TC-ADMIN-04 | ST TC-23 to TC-26, TC-34; `test_admin.cpp` | A |
| RT-F025 | Delete train without confirmed bookings | AdminModule, TrainModule | TC-ADMIN-05 | ST TC-31, TC-32 (manual only) | A |

## Non-functional requirements

| Req ID | Requirement | Implemented in | Test case | Evidence | Status |
|---|---|---|---|---|---|
| RT-NF001 | Search within 2 seconds for 10,000 trains | SearchModule, TrainModule | TC-NFR-01 | `test_nfr_performance.cpp`: slowest search about 7 ms | A |
| RT-NF002 | Consistent seat availability | BookingModule, FileModule [SeatModule] | TC-NFR-02 | `test_nfr_seats.cpp` | A |
| RT-NF003 | Simple menu-driven usability | UserInterface | TC-NFR-03 | `test_nfr_usability.sh`: 30 checks | A |
| RT-NF004 | Records persist after restart | FileModule | TC-NFR-04 | ST TC-21; `test_booking.cpp`; QA run | A |
| RT-NF005 | Logical component separation | Layered structure | TC-NFR-05 | QA run (architecture inspection) | A |

## Security requirements

| Req ID | Requirement | Implemented in | Test case | Evidence | Status |
|---|---|---|---|---|---|
| PRJ-SR001 | Validate all user input | ValidationModule | TC-SEC-01 | ST TC-09 to TC-12, TC-22, TC-38, TC-42; `test_validation.cpp`; QA run | A (see D-07) |
| PRJ-SR002 | Reject invalid train numbers and PNRs | ValidationModule, TicketModule | TC-SEC-02 | ST TC-05, TC-13, TC-16; QA run | A |
| PRJ-SR003 | Prevent duplicate PNRs | TicketModule | TC-SEC-03 | `test_booking.cpp`; QA run | A |
| PRJ-SR004 | Prevent booking with no seats | BookingModule [SeatModule] | TC-SEC-04 | ST TC-14; `test_booking.cpp`; QA run | A |
| PRJ-SR005 | Require administrator authentication | AdminModule | TC-SEC-05 | ST TC-26, TC-34; `test_admin.cpp`; QA run | A |

## Coverage

| Group | Requirements | Implemented | With passing evidence |
|---|---:|---:|---:|
| Functional | 25 | 25 | 25 |
| Non-functional | 5 | 5 | 5 |
| Security | 5 | 5 | 5 |
| **Total** | **35** | **35** | **35** |

Two requirements, RT-F023 and RT-F025, have manual evidence only (DEV-09).
One open low-severity defect, D-07, relates to RT-F006 and PRJ-SR001.

## Backlog to requirement mapping

| Backlog item | Requirements | Owner | Pull request |
|---|---|---|---|
| B01 Layered C++17 project | RT-NF005 | Farhan | #18 |
| B02 Load and save records | RT-F020, RT-NF004 (the issue text lists RT-F001 to RT-F003) | Nandita | #20 |
| B03 Display and search trains | RT-F001 to RT-F003 | Nandita | #20 |
| B04 Train details and seat availability | RT-F004 | Pranathi | #21 |
| B05 Validate passenger details and input | RT-F006, PRJ-SR001, PRJ-SR002 | Pranathi | #22 |
| B06 Ticket booking | RT-F005, RT-F007, RT-F008, PRJ-SR004 | Farhan | #24 |
| B07 PNR, fare and confirmation | RT-F009 to RT-F011, PRJ-SR003 | Monisha | #25 |
| B08 Retrieve bookings | RT-F012, RT-F018 | Monisha | #27 |
| B09 Cancel tickets | RT-F013 to RT-F016, RT-F019 | Monisha | #28 |
| B10 Cancellation refund | RT-F017 | Monisha | #29 |
| B11 Administrator login | RT-F024, PRJ-SR005 | Pranathi | #30 |
| B12 Add and modify trains | RT-F021, RT-F022 | Nandita | #31 |
| B13 View records, delete trains | RT-F023, RT-F025 | Nandita | #32 |
| B14 Performance, seats, usability | RT-NF001 to RT-NF003 | Pranathi | #33 |
| B15 Integration and regression testing | All | Farhan | #34 |
| B16 Website home page | Beyond SRS scope (DEV-01) | Pranathi | Direct commits to `main` |
| B17 Train search and results (web) | Beyond SRS scope (DEV-01) | Nandita | #39, #40 |
| B18 Passenger booking interface (web) | Beyond SRS scope (DEV-01) | Farhan | #41 |
| B19 Cancellation and admin interface (web) | Beyond SRS scope (DEV-01) | Monisha | #42, #43 |
| B20 Documentation and demos | Deliverable | Pranathi, Farhan | This change |

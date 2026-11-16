# Test Summary Report

**Project:** Railway Ticket Reservation System, Group 15
**Backlog item:** B20 (issue #16)
**Build tested:** `70e3a0b` on `main`
**Report date:** 16 November 2026
**Prepared by:** Mohammed Farhan Ahmed (QA Lead)

This is the final Test Summary Report required by section 8 of the Software
Test Plan. It reports results against the plan's own 35 test case IDs and
brings together the earlier reports:

- `INTEGRATION_TEST_REPORT.md` (B15, 29 October)
- `NFR_TEST_REPORT.md` (B14, 29 October)
- `SYSTEM_TEST_REPORT.md` (42 manual cases, 2 November)
- `DEFECT_REPORT.md` (16 November)

## 1. Result

| Measure | Value |
|---|---:|
| Test cases planned (Test Plan section 13) | 35 |
| Test cases executed | 35 (100%) |
| Passed | 34 |
| Passed with an open defect | 1 (TC-BOOK-02, defect D-07) |
| Failed | 0 |
| Requirements covered | 35 of 35 |
| Automated suites passing | 7 of 7 |
| Manual system cases passing | 42 of 42 |
| Open defects | 3 (0 critical, 0 major, 1 minor, 2 low) |

**Exit criteria (Test Plan 5.2)**

| Criterion | Met | Note |
|---|---|---|
| 100% of planned test cases executed | Yes | 35 of 35 |
| No critical functional, security or persistence defect open | Yes | Highest open severity is minor |
| All applicable acceptance criteria satisfied | Yes, with one exception | D-07: a one-character name is accepted |

The system is ready for sign-off by the Product Owner, subject to a decision
on D-07.

## 2. Test environments

| Run | Date | Environment |
|---|---|---|
| Integration and regression (B15) | 29 October | Local, g++ with `-Wall -Wextra -pedantic` |
| System test | 2 November | Windows with WSL (Ubuntu 22.04), g++ 11.4.0 |
| Continuous integration | Every push | GitHub Actions, `ubuntu-latest`, g++ |
| Final verification (this report) | 16 November | macOS 27.0.1 (arm64), Apple clang 21.0.0 |

## 3. Automated test results

Run with `./tests/run_all_tests.sh` on 16 November. All suites passed.

| Suite | Source | Result |
|---|---|---|
| Application build, warnings enabled | `src/*.cpp` | Pass, no warnings |
| ValidationModule | `test_validation.cpp` | Pass |
| BookingModule (booking, retrieval, cancellation, refund, reload) | `test_booking.cpp` | Pass |
| AdminModule (login, session) | `test_admin.cpp` | Pass |
| Admin train add and modify | `test_admin_trains.cpp` | Pass |
| TC-NFR-01 Search performance | `test_nfr_performance.cpp` | Pass |
| TC-NFR-02 Seat consistency | `test_nfr_seats.cpp` | Pass |
| TC-NFR-03 Menu usability (30 checks) | `test_nfr_usability.sh` | Pass |

The latest GitHub Actions run on `main` (2 November, commit `70e3a0b`) also
passed.

**TC-NFR-01 measurements, 10,000 train records, limit 2,000 ms**

| Operation | 29 Oct | 2 Nov | 16 Nov |
|---|---:|---:|---:|
| Load 10,000 trains | 25.9 ms | 34.9 ms | 50.7 ms |
| Search by source and destination | 4.1 ms | 6.3 ms | 7.1 ms |
| Search by train number, last record | 0.05 ms | not recorded | 0.21 ms |
| 100 consecutive route searches | 378.8 ms | 614.1 ms | 398.0 ms |
| Book one ticket | 0.66 ms | 1.07 ms | 0.64 ms |

Times vary by machine. Every run is far inside the limit.

## 4. Final verification run, 16 November

The system test report did not record the Test Plan's security, persistence
and architecture cases under their own IDs. They were executed again on
16 November against a copy of the data files, through the HTTP API, which
calls the same C++ modules as the console.

### TC-SEC-01: Invalid input is rejected without crashing

| Input | Response | Result |
|---|---|---|
| Body `garbage` | "Request body must be a valid JSON object." | Pass |
| Age 0 | "Train number, age or gender is invalid." | Pass |
| Age 121 | "Passenger details are invalid. ..." | Pass |
| Gender X | "Passenger details are invalid. ..." | Pass |
| Contact `12345` | "Passenger details are invalid. ..." | Pass |
| Name containing the pipe character | "Passenger details are invalid. ..." | Pass |
| Name `A` (one character) | Booking created | **Defect D-07** |

The server kept running after every request.

### TC-SEC-02: Invalid train numbers and PNRs

| Input | Response | Result |
|---|---|---|
| Retrieve PNR `123` | "Please enter a valid 10-digit PNR." | Pass |
| Retrieve PNR `0000000000` | "No booking was found for this PNR." | Pass |
| Book on train 99999 | "Train number was not found." | Pass |
| Cancel PNR `abc` | "Please enter a valid 10-digit PNR." | Pass |
| Cancel PNR `0000000000` | "No booking was found for this PNR." | Pass |

No record was created or changed by these requests.

### TC-SEC-03: PNR uniqueness

Four bookings were created in a row. They received PNRs 9372051853 to
9372051856. `bookings.txt` then held 22 records with 22 distinct PNRs. Pass.

### TC-SEC-04: No booking when the train is full

Booking on train 17301 (5 of 5 seats confirmed) returned "No seats are
available on this train." No record was created. Pass.

### TC-SEC-05: Administrator functions require login

Without logging in, each of these returned "Please log in as an
administrator first.": list trains, list bookings, add a train, delete a
train. Pass.

### TC-NFR-04: Records persist after restart

1. Booked PNR 9372051854 and cancelled it (refund Rs. 189).
2. Stopped the server and started it again.
3. Retrieved PNR 9372051853: found, status CONFIRMED.
4. Retrieved PNR 9372051854: found, status CANCELLED, refund 189.
5. Cancelled PNR 9372051854 again: "This booking has already been
   cancelled."
6. Train 16217 showed 145 of 150 seats available, matching its five
   confirmed bookings.

Pass. `test_booking.cpp` checks the same behaviour at module level.

### TC-NFR-05: Logical component separation (inspection)

| Check | Finding | Result |
|---|---|---|
| Only `FileModule` reads or writes the data files | `trains.txt` and `bookings.txt` are opened only in `src/FileModule.cpp`. The server uses file streams only to serve the static files in `web/`. | Pass |
| Presentation is separate from business rules | `UserInterface` and the server call the modules; `ValidationModule`, `BookingModule` and the others contain no console input or output. | Pass |
| Modules can be tested in isolation | Each automated suite links only the modules it needs, for example `test_validation.cpp` with `ValidationModule.cpp` alone. | Pass |
| A second interface could be added without changing the modules | The website was added over the same modules. | Pass |

Two findings from the inspection are recorded elsewhere: three designed
modules are not implemented (DEV-03) and their headers remain (D-08).

## 5. Results by Test Plan case

Evidence abbreviations: ST = `SYSTEM_TEST_REPORT.md`; AT = automated suite;
QA = section 4 above.

| Test case | Requirement | Evidence | Result |
|---|---|---|---|
| TC-TRAIN-01 | RT-F001 | ST TC-01; AT usability | Pass |
| TC-TRAIN-02 | RT-F002 | ST TC-02, TC-03; AT performance | Pass |
| TC-TRAIN-03 | RT-F003 | ST TC-04, TC-05; AT performance | Pass |
| TC-TRAIN-04 | RT-F004 | ST TC-06; AT seats | Pass |
| TC-BOOK-01 | RT-F005 | ST TC-07; AT booking, usability | Pass |
| TC-BOOK-02 | RT-F006 | ST TC-09 to TC-12; AT validation; QA | Pass with open defect D-07 |
| TC-BOOK-03 | RT-F007 | ST TC-14; AT booking | Pass |
| TC-BOOK-04 | RT-F008 | ST TC-07, TC-08; AT booking | Pass |
| TC-BOOK-05 | RT-F009 | ST TC-07; AT booking; QA | Pass |
| TC-BOOK-06 | RT-F010 | ST TC-07; AT booking | Pass |
| TC-BOOK-07 | RT-F011 | ST TC-07, TC-36; AT usability | Pass |
| TC-CAN-01 | RT-F012 | ST TC-15, TC-16; AT booking | Pass |
| TC-CAN-02 | RT-F013 | ST TC-17, TC-20, TC-39 | Pass |
| TC-CAN-03 | RT-F014 | ST TC-17, TC-20; AT booking | Pass |
| TC-CAN-04 | RT-F015 | ST TC-18; AT booking, seats | Pass |
| TC-CAN-05 | RT-F016 | ST TC-17; AT booking | Pass |
| TC-CAN-06 | RT-F017 | ST TC-17, TC-39; AT booking | Pass |
| TC-MGT-01 | RT-F018 | ST TC-15; AT booking | Pass |
| TC-MGT-02 | RT-F019 | ST TC-19; AT booking; QA | Pass |
| TC-MGT-03 | RT-F020 | ST TC-21, TC-37; AT booking; QA | Pass |
| TC-ADMIN-01 | RT-F021 | ST TC-27, TC-28; AT admin trains | Pass |
| TC-ADMIN-02 | RT-F022 | ST TC-29, TC-30; AT admin trains | Pass |
| TC-ADMIN-03 | RT-F023 | ST TC-33, TC-41 | Pass (manual only) |
| TC-ADMIN-04 | RT-F024 | ST TC-23 to TC-26, TC-34; AT admin | Pass |
| TC-ADMIN-05 | RT-F025 | ST TC-31, TC-32 | Pass (manual only) |
| TC-NFR-01 | RT-NF001 | AT performance | Pass |
| TC-NFR-02 | RT-NF002 | AT seats | Pass |
| TC-NFR-03 | RT-NF003 | AT usability | Pass |
| TC-NFR-04 | RT-NF004 | ST TC-21; AT booking; QA | Pass |
| TC-NFR-05 | RT-NF005 | QA inspection | Pass |
| TC-SEC-01 | PRJ-SR001 | ST TC-09 to TC-12, TC-22, TC-38, TC-42; QA | Pass (D-07 counted under TC-BOOK-02) |
| TC-SEC-02 | PRJ-SR002 | ST TC-05, TC-13, TC-16; QA | Pass |
| TC-SEC-03 | PRJ-SR003 | AT booking; QA | Pass |
| TC-SEC-04 | PRJ-SR004 | ST TC-14; AT booking; QA | Pass |
| TC-SEC-05 | PRJ-SR005 | ST TC-26, TC-34; AT admin; QA | Pass |

## 6. Known limitations

- Train deletion and the administrator record views have no automated test
  (DEV-09).
- The security checks cover the requirements in the SRS. No penetration
  testing was done; the server is for local coursework use only.
- Performance was measured on developer machines, not on a fixed reference
  machine.
- The 16 November run and the Sprint 2 demo used a Chromium browser; other browsers were not checked in that run.

## 7. Demonstration evidence

| Sprint | File | Content |
|---|---|---|
| 1 | `demo/sprint 1 demo.mp4` | Console application |
| 2 | `demo/sprint 2 demo.mp4` | Website: search, booking, cancellation and refund, administrator login and train management. About two minutes, captions with no narration. |

## 8. Sign-off

| Role | Name | Decision | Date |
|---|---|---|---|
| QA Lead | Mohammed Farhan Ahmed | Recommended for acceptance | 16 November 2026 |
| Test Engineer | Monisha Agarwal | | |
| Developer | Nandita N Patil | | |
| Product Owner | Lakshmi Pranathi Tummalapenta | | |

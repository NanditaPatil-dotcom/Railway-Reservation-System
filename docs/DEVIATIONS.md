# Deviations from the SRS, SAD and Test Plan

**Project:** Railway Ticket Reservation System, Group 15
**Backlog item:** B20 (issue #16)
**Build reviewed:** `70e3a0b` on `main`
**Date:** 16 November 2026
**Prepared by:** Mohammed Farhan Ahmed (QA Lead)

This document lists every place where the delivered system differs from the
three baseline documents in the team `docs` repository:

- Software Requirements Specification (SRS) v1.0
- Software Architecture and Design Specification (SAD) v1.0
- Software Test Plan (STP) v1.0

Each deviation states what the document says, what was built, the explanation
and the impact. Explanations marked **(team to confirm)** are inferred from the
code, commit history and README because the reason was not recorded at the
time.

## Summary

| ID | Area | Deviation | Impact |
|---|---|---|---|
| DEV-01 | SRS, SAD | A website and local HTTP server were added to a console-only design | Scope extension; console requirements still met |
| DEV-02 | SAD | Administrator credentials are fixed in code; `admin.txt` is not used | Credentials cannot be changed without rebuilding |
| DEV-03 | SAD | `CancellationModule`, `RefundModule` and `SeatModule` are not implemented | Their logic lives in `BookingModule`; traceability updated |
| DEV-04 | SAD | Console uses one flat menu, not Main / Passenger / Admin menus | None on function; fewer steps for the user |
| DEV-05 | SAD | Lookup maps for train number and PNR were not built | None; performance requirement still met |
| DEV-06 | SAD | Passenger validation rules differ from the SAD table | One open low-severity defect (D-07) |
| DEV-07 | STP | System test cases use IDs TC-01 to TC-42, not the plan's IDs | Resolved by the mapping in `TRACEABILITY_MATRIX.md` |
| DEV-08 | STP | Defects were not logged as GitHub issues; no daily status reports | Defects reconstructed in `tests/DEFECT_REPORT.md` |
| DEV-09 | STP | No automated test for train deletion and admin record views | Covered by manual system tests only |
| DEV-10 | B20 | Sprint dates and video deadlines were missed | Schedule only |

## 1. Deviations from the SRS

### DEV-01: Website and local HTTP server

- **Document:** SRS 1.2, 2.1 and 2.5 define a console-based C++ application.
  SRS 3.4 states that no external network communication is required. The SAD
  (3.5) rejected a client-server design and listed a graphical interface as a
  future enhancement (4.6).
- **Delivered:** Backlog items B16 to B19 added a website in `web/` and a
  small HTTP server in `server/booking_server.cpp`. The server exposes the
  same C++ modules through a JSON API.
- **Explanation (team to confirm):** The website was added in Sprint 2 as a
  second presentation layer once the console requirements were complete. It
  reuses the existing business logic and data access modules without changing
  them, which the layered design allows.
- **Impact:**
  - The console application is unchanged and still satisfies every SRS
    requirement on its own.
  - The server listens on the loopback address `127.0.0.1` only, so nothing
    leaves the machine. This keeps to the intent of SRS 3.4.
  - The server uses POSIX sockets, which is outside the "standard library
    only" choice in SAD 3.6. It builds on Linux, macOS and WSL but not on
    native Windows. The console application still builds on Windows and Linux.

No other deviation from the SRS functional, non-functional or security
requirements was found. All 35 requirements are implemented; see
`TRACEABILITY_MATRIX.md`.

## 2. Deviations from the SAD

### DEV-02: Administrator credentials are not stored in `admin.txt`

- **Document:** SAD 3.4, 3.6 and assumption A-1 say credentials are stored in
  `admin.txt` and loaded by `FileModule`.
- **Delivered:** The username and password are constants in
  `src/AdminModule.cpp`. No `admin.txt` exists. The comparison is done in
  constant time.
- **Explanation (team to confirm):** A fixed demo credential was simpler for
  a coursework prototype and avoided committing a password file to the
  repository. The README states that these are demo credentials.
- **Impact:** RT-F024 and PRJ-SR005 are still met. Changing the password
  needs a rebuild. Password hashing remains a future enhancement, as in the
  SAD.

### DEV-03: Three designed modules are not implemented

- **Document:** SAD 3.4 and 4.3 define `SeatModule`, `CancellationModule`
  and `RefundModule` as separate components.
- **Delivered:** Only their header files exist in `include/`. Their source
  files were removed as obsolete on 2 November (commit `ec2c989`).
  - Seat counting and lowest-free-seat allocation are in `BookingModule`,
    with available seats recomputed in `FileModule` on load.
  - Cancellation, repeat-cancellation blocking and the 90% refund are in
    `BookingModule::cancelBooking`.
- **Explanation (team to confirm):** Booking and cancellation both change the
  same seat and booking data, so they were kept in one module to keep each
  operation and its save atomic.
- **Impact:** All behaviour the SAD assigns to these modules is implemented
  and tested. The module column of the SRS traceability matrix is out of date
  and is corrected in `TRACEABILITY_MATRIX.md`. The three unused headers can
  be deleted in a later cleanup.

### DEV-04: Console menu structure

- **Document:** SAD 4.5 shows a Main menu leading to a Passenger menu and an
  Admin menu.
- **Delivered:** One main menu lists all passenger functions, with
  "8. Administrator login" opening the admin menu after authentication.
- **Explanation (team to confirm):** A single menu removes one navigation
  step for the common passenger flows.
- **Impact:** None on function. The usability test (TC-NFR-03) checks this
  menu and passes. Admin operations are still unreachable without login.

### DEV-05: No lookup maps

- **Document:** SAD 3.7 and 4.1 specify a map from train number to position
  and from PNR to position.
- **Delivered:** Trains and bookings are held in vectors and scanned
  linearly.
- **Explanation:** The maps were not needed to meet the performance target.
- **Impact:** None. With 10,000 trains, a search by train number takes under
  1 ms and the slowest route search about 7 ms, against a 2,000 ms limit.

### DEV-06: Passenger validation rules

| Field | SAD 4.5 rule | Implemented rule |
|---|---|---|
| Name | 2 to 50 characters; letters and spaces only | 1 to 50 characters; letters, spaces, `.`, `-` and `'`; must start with a letter |
| Contact number | Exactly 10 digits | Exactly 10 digits, first digit 6 to 9 |

- **Explanation:** Code comments show the name rule was widened on purpose
  for names such as "Jr." and hyphenated names, and the contact rule was
  tightened to the Indian mobile number format.
- **Impact:** The wider character set and stricter contact rule are accepted
  changes. The minimum length of 1 is not: a one-character name is accepted,
  which the SAD rule forbids. This is logged as open defect D-07 in
  `tests/DEFECT_REPORT.md`.

### Smaller differences

- **Web login lockout:** The SAD says three failed logins return the user to
  the main menu. The console does this. The website instead locks the login
  form until the server is restarted.
- **Status codes:** The code adds `INVALID_SEAT` and `NOT_IMPLEMENTED` to the
  list in SAD 4.4. Neither is returned by any implemented operation.

### Design decisions that were followed

These SAD decisions were checked in the code and are implemented as written.

| Decision | Where |
|---|---|
| ADR-1 Layered architecture, one class per module | `include/`, `src/` |
| ADR-2 Pipe-delimited text files | `src/FileModule.cpp` |
| ADR-3 Load into memory at start-up, save after every change | `src/FileModule.cpp`, `src/BookingModule.cpp` |
| ADR-4 Save through a temporary file, then rename | `atomicWrite` in `src/FileModule.cpp` |
| ADR-5 PNR is highest stored PNR plus one, from 1000000001 to 9999999999 | `src/TicketModule.cpp` |
| ADR-6 Available seats recomputed from confirmed bookings on load | `src/FileModule.cpp` |
| ADR-7 Lowest unoccupied seat number is assigned | `src/BookingModule.cpp` |
| Refund is 90% of the stored booking fare | `src/BookingModule.cpp` |
| In-memory change is rolled back when a save fails | `src/BookingModule.cpp` |
| Only `FileModule` reads or writes the data files | Inspection, see TC-NFR-05 |

## 3. Deviations from the Test Plan

### DEV-07: Test case identifiers

- **Document:** STP section 13 defines 35 test cases named by area, for
  example TC-TRAIN-01 and TC-SEC-05.
- **Delivered:** `tests/SYSTEM_TEST_REPORT.md` numbers its manual cases TC-01
  to TC-42. No report named TC-SEC-01 to TC-SEC-05, TC-NFR-04 or TC-NFR-05.
- **Explanation:** The system test cases were written per feature and
  screen, which gave more cases than the plan.
- **Impact:** Coverage could not be read off per requirement. This is
  resolved: `TRACEABILITY_MATRIX.md` maps every plan test case to its
  evidence, and the security, persistence and architecture cases were
  re-executed on 16 November and recorded in `tests/TEST_SUMMARY_REPORT.md`.

### DEV-08: Defect reporting and status reports

- **Document:** STP sections 6, 8 and 14 call for defects to be reported
  through GitHub issues, with defect reports, defect aging and daily
  execution status.
- **Delivered:** No issue was ever labelled as a bug. Defects were fixed
  directly in commits and pull requests. No daily status reports were kept.
- **Explanation:** With a four-person team and two one-week sprints, fixes
  were made in the same pull request or the next commit.
- **Impact:** Defect aging cannot be calculated. The defects were
  reconstructed from the commit history in `tests/DEFECT_REPORT.md`.

### DEV-09: Automated coverage gap for B13

- **Document:** STP 5.1 plans unit tests at module level for all modules.
- **Delivered:** `AdminModule::deleteTrain`, `viewTrains` and `viewBookings`
  have no automated test. They are covered by manual system cases TC-31 to
  TC-33 and TC-41.
- **Impact:** TC-ADMIN-03 and TC-ADMIN-05 rely on manual evidence only.
  A regression in these functions would not be caught by CI.

### Other differences

- **Test environment:** The plan names Windows or Linux with g++. Tests were
  also run on macOS with Apple clang, and CI runs on Ubuntu. All three pass.
- **Roles:** The plan names a separate Test Engineer for execution. System
  test execution on 2 November was recorded by the Product Owner.

## 4. Deviations from the B20 schedule

| Commitment | Actual | Note |
|---|---|---|
| Sprint 1 video uploaded by 23 October | Committed 24 October | One day late |
| Sprint 2 video uploaded by 30 October | Recorded 16 November | `demo/sprint 2 demo.mp4`, about two minutes, captions with no narration |
| Sprint 2 ends 30 October; development frozen | Last feature merge 2 November | Web items B18, B19 and admin web management merged 1 to 2 November |
| Final documentation complete by 30 October | Completed 16 November | This document and the linked reports |

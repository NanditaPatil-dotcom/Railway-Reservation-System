# Defect Report

**Project:** Railway Ticket Reservation System, Group 15
**Build:** `70e3a0b` on `main`
**Date:** 16 November 2026
**Prepared by:** Mohammed Farhan Ahmed (QA Lead)

The Test Plan called for defects to be logged as GitHub issues. That was not
done during the sprints: defects were fixed directly in commits and pull
requests (see DEV-08 in `docs/DEVIATIONS.md`). This report reconstructs them
from the commit history and adds the defects found during the final
verification run on 16 November.

**Severity:** Critical = data loss, security failure or crash. Major = a
requirement is not met. Minor = a requirement is met with a flaw.
Low = cosmetic, documentation or repository hygiene.

## Summary

| Status | Critical | Major | Minor | Low | Total |
|---|---:|---:|---:|---:|---:|
| Fixed and retested | 0 | 2 | 2 | 2 | 6 |
| Open | 0 | 0 | 1 | 2 | 3 |
| **Total** | **0** | **2** | **3** | **4** | **9** |

No critical defect was found. No critical or major defect is open, so the
Test Plan exit criterion on open defects is met.

## Fixed defects

| ID | Sprint | Severity | Defect | Fix | Retest |
|---|---|---|---|---|---|
| D-01 | 1 | Major | Train search rejected a search with only a source or only a destination; both fields were required. | `b9be4c2`, PR #26, 24 October. Either field may now be blank, but not both. | ST TC-03; `test_nfr_performance.cpp` searches by source only and by destination only. Pass. |
| D-02 | 1 | Minor | Booking tests existed but were not run by GitHub Actions, so a booking regression would not fail the build. | `b9be4c2`, PR #26, 24 October. Booking test step added to CI. | CI run on `main` executes the BookingModule tests. Pass. |
| D-03 | 1 | Low | An unused PNR helper (about 30 lines) was left in `BookingModule` after PNR generation moved to `TicketModule`. | `b9be4c2`, PR #26, 24 October. Dead code removed. | Build with `-Wall -Wextra -pedantic` has no warnings. Pass. |
| D-04 | 2 | Minor | The AdminModule test failed to link in CI after B12, because the build command did not include `TrainModule` and `FileModule`. The README command and a test fixture had the same problem. | `c5e0dc3` and `66f7034`, PR #31, 28 October. | CI steps "Run AdminModule tests" and "Run admin train management tests". Pass. |
| D-05 | 2 | Major | The website loaded without its stylesheet: `style.css` was in the repository root, not `web/`, and was not linked from the page. | `7e37b24`, PR #39, 30 October. | ST TC-35; Sprint 2 demo recording. Pass. |
| D-06 | 2 | Low | Compiled binaries (`railway`, `booking_server`, six test executables) and five patch files were committed to the repository. | `3945b52`, `3131ff8`, `ec2c989`, `70e3a0b`, 30 October to 2 November. `.gitignore` updated. | `git ls-files` shows no binaries. Pass. |

## Open defects

### D-07: A one-character passenger name is accepted

- **Severity:** Minor
- **Found:** 16 November, final verification run (TC-SEC-01 / TC-BOOK-02)
- **Requirement:** RT-F006, PRJ-SR001; SAD 4.5 rule "Name: 2 to 50
  characters"
- **Steps:** Book on train 16217 with name `A`, age 30, gender F, contact
  9123456780.
- **Expected:** Booking rejected as invalid input.
- **Actual:** Booking created and saved with passenger name `A`.
- **Cause:** `isValidName` in `src/ValidationModule.cpp` checks for an empty
  name and a maximum of 50 characters, but has no minimum of 2.
  `tests/test_validation.cpp` has no one-character case.
- **Suggested fix:** Reject names shorter than 2 characters and add the test
  case.

### D-08: Unused module headers remain

- **Severity:** Low
- **Found:** 16 November, architecture inspection (TC-NFR-05)
- **Detail:** `include/SeatModule.h`, `include/RefundModule.h` and
  `include/CancellationModule.h` declare classes that have no implementation
  and are not included anywhere. Their source files were removed in
  `ec2c989`.
- **Suggested fix:** Delete the three headers, or implement the modules as
  designed in the SAD.

### D-09: Admin tables are clipped on the website at 1280 px width

- **Severity:** Low (cosmetic)
- **Found:** 16 November, while recording the Sprint 2 demo
- **Detail:** In the administrator dashboard, the trains and bookings tables
  are wider than their container. The status and refund columns are only
  visible after scrolling the table sideways.
- **Suggested fix:** Reduce column padding or let the dashboard use the full
  page width.

## Documentation defects fixed in B20

These were corrected as part of this change and are not counted above.

- `README.md` said the three unimplemented modules "exist as interface
  placeholders" without saying their source files had been removed.
- `tests/README.md` listed `SeatModule` and `CancellationModule` tests as
  existing or planned.
- `tests/INTEGRATION_TEST_REPORT.md` still showed B15 acceptance as pending
  after pull request #34 was merged.

## Metrics

| Metric | Value |
|---|---|
| Defects found | 9 |
| Defects fixed and retested | 6 |
| Defects open | 3 (0 critical, 0 major) |
| Production code size | About 5,350 lines (C++ 3,190; web 2,160) |
| Defect density | About 1.7 defects per 1,000 lines |
| Defect aging | Not available; defects were not logged with a found date |

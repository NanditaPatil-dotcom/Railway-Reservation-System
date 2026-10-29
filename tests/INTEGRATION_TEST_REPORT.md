# B15 — Integration, Regression and Acceptance Test Report

**Issue:** #15
**Sprint:** 2
**Execution date:** 29 October 2026
**Branch:** `test/B15-integration-regression-acceptance`

## 1. Objective

Verify the integrated Railway Reservation System against the applicable
requirements and software test plan. Execute the available module-level,
integration, regression, performance, seat-consistency and usability tests.

## 2. Test Execution Summary

| Test suite | Result |
|---|---|
| C++17 application build | Passed |
| ValidationModule tests | Passed |
| BookingModule and related booking tests | Passed |
| AdminModule tests | Passed |
| Administrator train-management tests | Passed |
| TC-NFR-01: Search performance | Passed |
| TC-NFR-02: Seat consistency | Passed |
| TC-NFR-03: Menu usability | Passed |

**Observed outcome:** All commands in the recorded local test run completed
successfully. No failures were reported by these test suites.

## 3. Performance Test Evidence

The performance test used 10,000 train records and a 2,000 ms limit.

| Operation | Observed time |
|---|---:|
| Load 10,000 trains | 25.940 ms |
| Search by source and destination | 4.058 ms |
| Search by source only | 2.263 ms |
| Search by destination only | 2.125 ms |
| Search by train number, last record | 0.046 ms |
| Search by train number, no match | 0.035 ms |
| 100 consecutive route searches | 378.775 ms |
| Book one ticket with 10,000 trains loaded | 0.662 ms |

All reported performance checks passed their specified limits.

## 4. Regression and Integration Evidence

- Validation tests passed.
- Booking tests, including the reported B07, B08, B09 and B10 checks, passed.
- Administrator login and train-management tests passed.
- Seat-consistency checks passed.
- The scripted usability test passed all 30 checks, including booking,
  confirmation details, PNR retrieval, invalid inputs and clean exit.

## 5. Defects and Limitations

No failures were reported in the executed test suites.

This report records the observed test run. It does not claim that every
possible defect or security vulnerability has been ruled out. Dedicated
security testing and any additional acceptance cases must be recorded
separately if required by the approved test plan.

## 6. Reproduction

Run the commands listed in `tests/NFR_TEST_REPORT.md` to reproduce the
non-functional performance, seat-consistency and usability checks.

The module-level commands are defined in `.github/workflows/` and can be
used to reproduce the validation, booking and administrator test runs.

## 7. Final Status

**Local test execution:** Passed for all suites listed above.

**Final B15 acceptance:** Pending review of the complete test plan,
verification of the final changes in CI, and confirmation that all applicable
acceptance criteria have been satisfied.

## 8. Reproducible Test Execution

The complete local regression suite can be executed using the unified test runner:

```bash
chmod +x tests/run_all_tests.sh
./tests/run_all_tests.sh
```

The runner compiles and executes the application and the available validation, booking, administrator, performance, seat-consistency and usability tests.

The test output should be reviewed for failures before the integration changes are submitted for review. The GitHub Actions workflow must also pass before the pull request is considered ready to merge.

## 9. Test Execution Environment and Limitations

The test suite was executed locally using a C++17 compiler with warning checks enabled. The unified test runner completed successfully, including validation, booking, administrator, performance, seat-consistency and usability tests.

Performance measurements may vary across machines. The recorded results demonstrate the observed behavior for this test run and do not guarantee the absence of all defects. Final verification depends on successful CI execution and review of the applicable acceptance criteria.
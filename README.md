# Railway Ticket Reservation System

**Software Engineering Mini-Project · Group 15**

A railway ticket reservation application written in **C++17**. It runs as a menu-driven console program and also as a local website backed by the same C++ modules. Data is stored in plain text files, so there is nothing to install beyond a C++ compiler.

## Features

**Passengers**

- View all trains, or search by source, destination or train number
- See train details and live seat availability
- Book a ticket with validated passenger details; a unique 10-digit PNR, seat number and fare are issued
- Look up a booking by PNR
- Cancel a confirmed booking and receive a refund of 90% of the fare

**Administrators** (login required)

- Add, modify and delete trains
- View all trains and all bookings
- A train cannot be deleted, or have its capacity cut below its confirmed bookings

## Team

| Team Member | Role | SRN |
|---|---|---|
| Monisha Agarwal | Developer | `PES2UG24CS285` |
| Lakshmi Pranathi Tummalapenta | Scrum Master | `PES2UG24CS249` |
| Mohammed Farhan Ahmed | Developer | `PES2UG24CS280` |
| Nandita N Patil | Developer | `PES2UG24CS304` |

| Sprint | Dates | Focus |
|---|---|---|
| Sprint 1 | 19–23 October 2026 | Initial implementation and working product |
| Sprint 2 | 26–30 October 2026 | Remaining features, integration, testing and documentation |

## Architecture

The system follows the three-layer design in the Software Architecture and Design Specification.

| Layer | Modules |
|---|---|
| Presentation | `UserInterface` (console), `web/` (browser) via `server/booking_server.cpp` |
| Business logic | `ValidationModule`, `TrainModule`, `SearchModule`, `FareModule`, `TicketModule`, `BookingModule`, `AdminModule` |
| Data access | `FileModule`, the only module that reads or writes `trains.txt` and `bookings.txt` |

Cancellation, refund and seat allocation logic lives in `BookingModule`. The SAD also designed `CancellationModule`, `RefundModule` and `SeatModule`; these were not implemented and only their unused header files remain. This and the other differences from the SRS, SAD and Test Plan are explained in `docs/DEVIATIONS.md`.

## Project Structure

```
include/   Header files and shared data types
src/       Module implementations and the console entry point
server/    Small HTTP server that exposes the C++ modules to the website
web/       Website (HTML, CSS, JavaScript)
tests/     Test sources, test runner and test reports
docs/      Deviations from the baseline documents and the final traceability matrix
demo/      Sprint 1 and Sprint 2 demo videos
trains.txt, bookings.txt   Persistent data files
```

## Getting Started

You need `g++` with C++17 support.

### Console application

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude src/*.cpp -o railway
./railway
```

### Website

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude server/booking_server.cpp src/AdminModule.cpp src/BookingModule.cpp src/FareModule.cpp src/FileModule.cpp src/TicketModule.cpp src/TrainModule.cpp src/ValidationModule.cpp -o booking_server
./booking_server
```

Open **http://127.0.0.1:8080** and keep the terminal running while you use the site. Stop the server with `Ctrl+C`. Do not open `web/index.html` directly from disk; the page needs the server to reach the API.

The server binds to the loopback address only and is meant for local coursework use. Do not expose it to an untrusted network.

Both versions read and write the same `trains.txt` and `bookings.txt`, so a booking made in one appears in the other.

## Administrator Access

| Username | Password |
|---|---|
| `admin` | `Admin@123` |

Use **8. Administrator login** in the console, or the Administrator section of the website. Three failed attempts return you to the main menu in the console and lock the web login until the server is restarted. Administrative operations are refused unless an administrator is logged in.

These are fixed demo credentials for coursework and must be changed for any real deployment.

## Web API

| Method | Endpoint | Purpose |
|---|---|---|
| GET | `/api/trains` | Trains, fares and current available seats |
| POST | `/api/bookings` | Validate passenger details and create a booking |
| GET | `/api/bookings/{PNR}` | Retrieve a booking |
| POST | `/api/bookings/cancel` | Cancel a booking; returns refund and cancellation charge |
| POST | `/api/admin/login` | Administrator login |
| GET | `/api/admin/session` | Login state and remaining attempts |
| POST | `/api/admin/logout` | End the administrator session |
| GET | `/api/admin/trains` | List all trains (admin only) |
| GET | `/api/admin/bookings` | List all bookings (admin only) |
| POST | `/api/admin/trains/add` | Add a train (admin only) |
| POST | `/api/admin/trains/modify` | Modify a train (admin only) |
| POST | `/api/admin/trains/delete` | Delete a train (admin only) |

## Testing

Run the full suite (build, unit, integration, performance, seat-consistency and usability tests) from the project root:

```bash
chmod +x tests/run_all_tests.sh
./tests/run_all_tests.sh
```

The same checks run on every push through GitHub Actions (`.github/workflows/ci.yml`).

## Documentation

The SRS, SAD and Test Plan are in the team [`docs`](https://github.com/Railway-Reservation-Group15/docs) repository. The final project documentation is in this repository.

| Document | Contents |
|---|---|
| `docs/DEVIATIONS.md` | Differences from the SRS, SAD and Test Plan, with explanations |
| `docs/TRACEABILITY_MATRIX.md` | Each requirement mapped to its module, test case and evidence |
| `tests/TEST_SUMMARY_REPORT.md` | Final results against the 35 Test Plan cases |
| `tests/DEFECT_REPORT.md` | Defects found, fixed and still open |
| `tests/SYSTEM_TEST_REPORT.md` | 42 manual system test cases |
| `tests/INTEGRATION_TEST_REPORT.md` | Integration and regression testing (B15) |
| `tests/NFR_TEST_REPORT.md` | Performance, seat consistency and usability (B14) |
| `demo/sprint 1 demo.mp4` | Sprint 1 demo: console application |
| `demo/sprint 2 demo.mp4` | Sprint 2 demo: website |

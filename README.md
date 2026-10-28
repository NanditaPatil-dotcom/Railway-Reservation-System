# Railway Ticket Reservation System

**Software Engineering Mini-Project · Group 15**

A console-based railway ticket reservation application developed in **C++17**, designed to support train search, ticket booking, reservation management, ticket cancellation, and administrative operations.

---

## Overview

The Railway Ticket Reservation System aims to simplify railway reservation operations through a menu-driven console application. It enables passengers to search for trains, check seat availability, book tickets, retrieve booking details using a PNR, and cancel existing reservations.

The system also provides administrative functionality for managing train records and viewing train and booking information.

The application is designed as a standalone system using local text files for persistent data storage.

## Objectives

- Display available trains and support searching by source, destination, or train number.
- Show train details and seat availability.
- Validate passenger information before booking.
- Generate unique Passenger Name Record (PNR) identifiers and calculate fares.
- Support booking retrieval, ticket cancellation, and refund calculation.
- Maintain consistent seat availability and persistent booking records.
- Provide authenticated administrative operations for train management.
- Follow a modular, layered architecture to improve maintainability and testability.


## Team Members

| Team Member | Role | SRN |
|---|---|---|
| Monisha Agarwal | Developer | `PES2UG24CS285` |
| Lakshmi Pranathi Tummalapenta | Scrum Master | `PES2UG24CS249` |
| Mohammed Farhan Ahmed | Developer | `PES2UG24CS280` |
| Nandita N Patil | Developer | `PES2UG24CS304` |


### Sprint Schedule

| Sprint | Dates | Focus |
|---|---|---|
| Sprint 1 | 19–23 October 2026 | Initial implementation and working product |
| Sprint 2 | 26–30 October 2026 | Remaining features, integration, testing, and documentation |

## Architecture

The Railway Reservation System follows a three-layer architecture defined in the Software Architecture and Design Specification.

- **Presentation Layer:** `UserInterface` handles menus, prompts, confirmations, and messages.
- **Business Logic Layer:** `ValidationModule`, `TrainModule`, `SearchModule`, `SeatModule`, `FareModule`, `TicketModule`, `BookingModule`, `CancellationModule`, `RefundModule`, and `AdminModule`.
- **Data Access Layer:** `FileModule` is the only module responsible for persistent file access.

## Project Structure

- `include/` — Header files, public interfaces, and shared data types.
- `src/` — Module implementations and the application entry point.
- `tests/` — Module-level testing documentation and future tests.
- `.github/workflows/` — Automated build checks.

## Build

Compile with a C++17-compatible compiler:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude src/*.cpp -o railway
```

Run the setup application:

```bash
./railway
```

## Administrator Access

Choose **8. Administrator login** from the main menu and enter the administrator credentials:

| Username | Password |
|---|---|
| `admin` | `Admin@123` |

Three failed attempts return you to the main menu. Administrative operations are refused unless an administrator is logged in, even when called directly from code. Leaving the administrator menu logs the administrator out.

After login, choose **1. Add train** to enter a new train record or **2. Modify train** to update an existing record. Train numbers are unique and remain fixed when editing; train capacity cannot be reduced below the number of confirmed bookings. Train changes are saved to `trains.txt`, while existing booking fares remain stored in `bookings.txt`.

Run the administrator tests with:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude tests/test_admin.cpp src/AdminModule.cpp -o test_admin && ./test_admin
```

Run the B12 add/modify acceptance tests with:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude tests/test_admin_trains.cpp src/AdminModule.cpp src/TrainModule.cpp src/FileModule.cpp -o test_admin_trains && ./test_admin_trains
```

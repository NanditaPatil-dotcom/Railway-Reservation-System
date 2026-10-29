#!/usr/bin/env bash
# B14 / TC-NFR-03 (RT-NF003): a user can complete a basic booking using the
# menu alone, without external instructions. The console application is
# driven exactly as a user would type, and the on-screen output is checked.
#
# Usage: tests/test_nfr_usability.sh [path-to-railway-binary]
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$(cd "$(dirname "${1:-$ROOT/railway}")" && pwd)/$(basename "${1:-$ROOT/railway}")"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

failures=0
pass() { echo "PASS: $1"; }
fail() { echo "FAIL: $1"; failures=$((failures + 1)); }

expect() {  # expect <output> <pattern> <description>
    if grep -qE -- "$2" <<<"$1"; then pass "$3"; else fail "$3"; fi
}

reset_data() {
    cp "$ROOT/trains.txt" "$WORK/trains.txt"
    : > "$WORK/bookings.txt"
}

run_app() { ( cd "$WORK" && "$BIN" 2>&1 ); }

if [ ! -x "$BIN" ]; then
    echo "Binary not found or not executable: $BIN"
    exit 2
fi

echo "TC-NFR-03 usability checks"

# 1. The main menu lists every function with a number, and says how to exit.
reset_data
out="$(printf '0\n' | run_app)"
for item in "View all trains" "Search by source and destination" \
            "Search by train number" "View train details and seat availability" \
            "Book a ticket" "View booking by PNR" "Cancel booking" \
            "Administrator login" "Exit"; do
    expect "$out" "$item" "main menu shows: $item"
done
expect "$out" "Choose an option:" "main menu prompts the user for a choice"

# 2. A first-time user can complete a basic booking from the menu alone.
reset_data
out="$(printf '5\n12627\nAsha Rao\n24\nF\n9876543210\n0\n' | run_app)"
expect "$out" "Train number:"       "booking asks for the train number"
expect "$out" "Passenger name:"     "booking asks for the passenger name"
expect "$out" "Passenger age:"      "booking asks for the age"
expect "$out" "Gender \(M/F/O\):"  "booking asks for gender with allowed values"
expect "$out" "Mobile number:"      "booking asks for the mobile number"
expect "$out" "BOOKING CONFIRMED"   "booking ends with a clear confirmation"
expect "$out" "PNR +: [0-9]{10}"    "confirmation shows a 10-digit PNR"
expect "$out" "Seat Number +: 1"    "confirmation shows the seat number"
expect "$out" "Fare +: Rs\. 1850\.50" "confirmation shows the fare"
expect "$out" "Status +: CONFIRMED" "confirmation shows the booking status"

# Number of user inputs needed for a basic booking (menu + 5 fields).
echo "INFO: a basic booking needs 6 inputs (menu choice + 5 fields)"

# 3. The user can find the booking again with the PNR printed on screen.
pnr="$(grep -oE 'PNR +: [0-9]{10}' <<<"$out" | grep -oE '[0-9]{10}' | head -1)"
if [ -n "$pnr" ]; then
    out2="$(printf '6\n%s\n0\n' "$pnr" | ( cd "$WORK" && "$BIN" 2>&1 ))"
    expect "$out2" "BOOKING DETAILS" "booking can be retrieved by the PNR shown"
    expect "$out2" "Passenger +: Asha Rao" "retrieved booking shows the passenger"
else
    fail "could not read the PNR from the confirmation"
fi

# 4. The user can discover trains before booking (search flow, no manual).
reset_data
out="$(printf '2\nBengaluru\nChennai\n0\n' | run_app)"
expect "$out" "Source \(blank = any\):" "search prompt explains blank means any"
expect "$out" "Shatabdi Express"      "route search lists matching trains"

# 5. Mistakes are explained in plain language and the user can continue.
reset_data
out="$(printf 'abc\n9\n0\n' | run_app)"
expect "$out" "Please enter a valid whole number\." "non-numeric menu input is explained"
expect "$out" "Choose 0, 1, 2, 3, 4, 5, 6, 7, or 8\." "out-of-range choice lists valid options"

reset_data
out="$(printf '5\n99999\nAsha Rao\n24\nF\n9876543210\n0\n' | run_app)"
expect "$out" "Train not found\. Check the train number\." "unknown train gives a helpful message"

reset_data
out="$(printf '5\n12627\nAsha Rao\n24\nX\n9876543210\n0\n' | run_app)"
expect "$out" "Invalid train number or passenger details\." "invalid passenger details are reported"

reset_data
out="$(printf '6\n123\n0\n' | run_app)"
expect "$out" "Invalid PNR\. Please enter a valid 10-digit PNR\." "bad PNR explains the expected format"

# 6. The application never crashes when input ends unexpectedly.
reset_data
printf '5\n12627\nAsha' | run_app >/dev/null
if [ $? -eq 0 ]; then pass "app exits cleanly when input ends mid-booking"
else fail "app exits cleanly when input ends mid-booking"; fi

echo
if [ "$failures" -ne 0 ]; then
    echo "$failures usability check(s) failed."
    exit 1
fi
echo "All TC-NFR-03 usability checks passed."

// Module-level tests for ValidationModule (B05).
// Build and run from the repository root (one command):
//   g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude tests/test_validation.cpp src/ValidationModule.cpp -o test_validation && ./test_validation

#include "ValidationModule.h"

#include <iostream>
#include <string>

namespace {
int failures = 0;

void check(bool condition, const std::string& label) {
    if (!condition) {
        ++failures;
        std::cout << "FAIL: " << label << '\n';
    }
}

Passenger valid() {
    Passenger p;
    p.name = "Asha Rao";
    p.age = 25;
    p.gender = 'F';
    p.contact = "9876543210";
    return p;
}
}  // namespace

int main() {
    const ValidationModule v;

    // ---- validatePassenger ----
    check(v.validatePassenger(valid()) == Status::OK, "valid passenger");

    Passenger p = valid();
    p.name = "";
    check(v.validatePassenger(p) == Status::INVALID_INPUT, "empty name");
    p.name = "   ";
    check(v.validatePassenger(p) == Status::INVALID_INPUT, "blank name");
    p.name = " Asha";
    check(v.validatePassenger(p) == Status::INVALID_INPUT, "leading space in name");
    p.name = "Asha ";
    check(v.validatePassenger(p) == Status::INVALID_INPUT, "trailing space in name");
    p.name = "Asha123";
    check(v.validatePassenger(p) == Status::INVALID_INPUT, "digits in name");
    p.name = "Asha|Rao";
    check(v.validatePassenger(p) == Status::INVALID_INPUT, "pipe in name breaks file format");
    p.name = std::string(51, 'a');
    check(v.validatePassenger(p) == Status::INVALID_INPUT, "name over 50 chars");
    p.name = std::string(50, 'a');
    check(v.validatePassenger(p) == Status::OK, "name of exactly 50 chars");
    p.name = "Mary-Ann O'Neil Jr.";
    check(v.validatePassenger(p) == Status::OK, "hyphen, apostrophe, dot in name");

    p = valid();
    p.age = 0;
    check(v.validatePassenger(p) == Status::INVALID_INPUT, "age 0");
    p.age = -5;
    check(v.validatePassenger(p) == Status::INVALID_INPUT, "negative age");
    p.age = 121;
    check(v.validatePassenger(p) == Status::INVALID_INPUT, "age 121");
    p.age = 1;
    check(v.validatePassenger(p) == Status::OK, "age 1");
    p.age = 120;
    check(v.validatePassenger(p) == Status::OK, "age 120");

    p = valid();
    for (char g : {'M', 'F', 'O'}) {
        p.gender = g;
        check(v.validatePassenger(p) == Status::OK, std::string("gender ") + g);
    }
    for (char g : {'m', 'X', ' ', '1', '\0'}) {
        p.gender = g;
        check(v.validatePassenger(p) == Status::INVALID_INPUT, "bad gender");
    }

    p = valid();
    p.contact = "";
    check(v.validatePassenger(p) == Status::INVALID_INPUT, "empty contact");
    p.contact = "987654321";
    check(v.validatePassenger(p) == Status::INVALID_INPUT, "9-digit contact");
    p.contact = "98765432100";
    check(v.validatePassenger(p) == Status::INVALID_INPUT, "11-digit contact");
    p.contact = "5876543210";
    check(v.validatePassenger(p) == Status::INVALID_INPUT, "contact starting with 5");
    p.contact = "98765abcde";
    check(v.validatePassenger(p) == Status::INVALID_INPUT, "letters in contact");
    p.contact = "+919876543";
    check(v.validatePassenger(p) == Status::INVALID_INPUT, "plus sign in contact");
    p.contact = "6000000000";
    check(v.validatePassenger(p) == Status::OK, "contact starting with 6");

    // ---- validateTrainNo ----
    check(v.validateTrainNo("12028") == Status::OK, "train no 12028");
    check(v.validateTrainNo("1") == Status::OK, "train no 1");
    check(v.validateTrainNo("999999999") == Status::OK, "9-digit train no");
    check(v.validateTrainNo("") == Status::INVALID_INPUT, "empty train no");
    check(v.validateTrainNo("0") == Status::INVALID_INPUT, "train no 0");
    check(v.validateTrainNo("000") == Status::INVALID_INPUT, "train no 000");
    check(v.validateTrainNo("-12") == Status::INVALID_INPUT, "negative train no");
    check(v.validateTrainNo("+12") == Status::INVALID_INPUT, "signed train no");
    check(v.validateTrainNo("12a") == Status::INVALID_INPUT, "letters in train no");
    check(v.validateTrainNo(" 12") == Status::INVALID_INPUT, "space in train no");
    check(v.validateTrainNo("1.5") == Status::INVALID_INPUT, "decimal train no");
    check(v.validateTrainNo("1000000000") == Status::INVALID_INPUT, "overflowing train no");
    check(v.validateTrainNo("99999999999999999999") == Status::INVALID_INPUT, "huge train no");

    // ---- validatePNR ----
    check(v.validatePNR("1234567890") == Status::OK, "numeric PNR");
    check(v.validatePNR("AB12CD34EF") == Status::OK, "alphanumeric PNR");
    check(v.validatePNR("") == Status::INVALID_PNR, "empty PNR");
    check(v.validatePNR("123456789") == Status::INVALID_PNR, "9-char PNR");
    check(v.validatePNR("12345678901") == Status::INVALID_PNR, "11-char PNR");
    check(v.validatePNR("12345 7890") == Status::INVALID_PNR, "space in PNR");
    check(v.validatePNR("12345|7890") == Status::INVALID_PNR, "pipe in PNR");
    check(v.validatePNR("123456789!") == Status::INVALID_PNR, "symbol in PNR");

    // ---- readMenuChoice ----
    check(v.readMenuChoice("0", 0, 4) == Status::OK, "menu lower bound");
    check(v.readMenuChoice("4", 0, 4) == Status::OK, "menu upper bound");
    check(v.readMenuChoice("2", 0, 4) == Status::OK, "menu middle");
    check(v.readMenuChoice("5", 0, 4) == Status::INVALID_INPUT, "menu above max");
    check(v.readMenuChoice("-1", 0, 4) == Status::INVALID_INPUT, "menu negative");
    check(v.readMenuChoice("", 0, 4) == Status::INVALID_INPUT, "menu empty");
    check(v.readMenuChoice("abc", 0, 4) == Status::INVALID_INPUT, "menu letters");
    check(v.readMenuChoice("2x", 0, 4) == Status::INVALID_INPUT, "menu trailing junk");
    check(v.readMenuChoice("1.5", 0, 4) == Status::INVALID_INPUT, "menu decimal");
    check(v.readMenuChoice("99999999999999", 0, 4) == Status::INVALID_INPUT, "menu overflow");
    check(v.readMenuChoice("1", 4, 0) == Status::INVALID_INPUT, "menu min > max");

    if (failures == 0) {
        std::cout << "All ValidationModule tests passed.\n";
        return 0;
    }
    std::cout << failures << " test(s) failed.\n";
    return 1;
}

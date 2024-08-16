#include <string>
#include <sstream>
#include <locale>
#include <iomanip>
#include <string.h>
#include <math.h>
#include <stdint.h>

#include "money.h"

using namespace std;

Money::Money() {
    this->representedValue = 0;
}

Money::Money(money_t amount) {
    _setValue(amount);
}

Money::Money(string & amount) {
    _setValue(amount.c_str());
}

Money::Money(const char * amount) {
    _setValue(amount);
}

int Money::findDecimalPointPos(const char * amount) {
    int decimalPointPos = DECIMAL_POINT_NOT_FOUND_VALUE;

    for (int i = 0;i < strlen(amount);i++) {
        if (amount[i] == '.') {
            decimalPointPos = i;
            break;
        }
    }

    return decimalPointPos;
}

money_t Money::makeRepresentedValue(money_t whole, money_t decimal) {
    int signMultiplier;

    if (whole < 0) {
        signMultiplier = -1;
    }
    else {
        signMultiplier = 1;
    }

    return (whole * 100) + (decimal * signMultiplier);
}

Money::Money(double amount) {
    _setValue(amount);
}

Money::Money(const Money & src) {
    _setValue(src._getValue());
}

money_t Money::_getValue() const {
    return representedValue;
}

void Money::_setValue(const char * amount) {
    int decimalPointPos = findDecimalPointPos(amount);

    money_t whole;
    money_t decimal = 0L;

    if (decimalPointPos == DECIMAL_POINT_NOT_FOUND_VALUE) {
        whole = (money_t)strtol(amount, NULL, 10);
    }
    else {
        char buffer[16];
        for (int i = 0;i < decimalPointPos;i++) {
            buffer[i] = amount[i];
        }
        buffer[decimalPointPos] = 0;

        whole = (money_t)strtol(buffer, NULL, 10);
        decimal = (money_t)strtol(&amount[decimalPointPos + 1], NULL, 10);
    }

    _setValue(makeRepresentedValue(whole, decimal));
}

void Money::_setValue(double amount) {
    _setValue((money_t)round(amount * (double)100.0));
}

void Money::_setValue(money_t amount) {
    this->representedValue = amount;
}

double Money::getDoubleValue() {
    double value = (double)this->representedValue / (double)100.0;
    return value;
}

string Money::getRawStringValue() const {
    char buffer[16];
    money_t whole = this->representedValue / 100;
    money_t decimal = this->representedValue - (whole * 100);

    if (decimal < 0) {
        decimal = decimal * -1;
    }

    snprintf(buffer, 16, "%d.%02d", whole, decimal);

    string value = buffer;
    return value;
}

string Money::getFormattedStringValue() const {
    stringstream s;

    s << showbase << put_money(getRawStringValue());

    return s.str();
}

Money & Money::operator=(const Money & rhs) {
    if (this == &rhs) {
        return *this;
    }

    this->_setValue(rhs._getValue());

    return *this;
}

const Money Money::operator+(const Money & rhs) {
    Money result = *this;
    result += rhs;
    return result;
}

Money & Money::operator+=(const Money & rhs) {
    this->representedValue += rhs.representedValue;
    return *this;
}

const Money Money::operator-(const Money & rhs) {
    Money result = *this;
    result -= rhs;
    return result;
}

Money & Money::operator-=(const Money & rhs) {
    this->representedValue -= rhs.representedValue;
    return *this;
}

const Money Money::operator*(const Money & rhs) {
    Money result = *this;
    result *= rhs;
    return result;
}

Money & Money::operator*=(const Money & rhs) {
    this->representedValue *= rhs.representedValue;
    this->representedValue /= 100;
    return *this;
}

const Money Money::operator/(const Money & rhs) {
    Money result = *this;
    result /= rhs;
    return result;
}

Money & Money::operator/=(const Money & rhs) {
    this->representedValue /= rhs.representedValue;
    return *this;
}

bool Money::operator==(const Money & rhs) {
    return (this->representedValue == rhs._getValue());
}

bool Money::operator!=(const Money & rhs) {
    return !(*this == rhs);
}

bool Money::operator<(const Money & src) {
    return (this->representedValue < src._getValue());
}

bool Money::operator<=(const Money & src) {
    return (this->representedValue <= src._getValue());
}

bool Money::operator>(const Money & src) {
    return (this->representedValue > src._getValue());
}

bool Money::operator>=(const Money & src){
    return (this->representedValue >= src._getValue());
}

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
    this->representedValue = amount;
}

Money::Money(string & amount) {
    Money(amount.c_str());
}

Money::Money(const char * amount) {
    int decimalPointPos = findDecimalPointPos(amount);

    money_t whole;
    money_t decimal = 0L;

    if (decimalPointPos == DECIMAL_POINT_NOT_FOUND_VALUE) {
        whole = (money_t)strtol(amount, NULL, 10);
    }
    else {
        char buffer[16];
        strncpy(buffer, amount, decimalPointPos);
        whole = (money_t)strtol(buffer, NULL, 10);
        decimal = (money_t)strtol(&amount[decimalPointPos + 1], NULL, 10);
    }

    Money((whole * 100) + decimal);
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

Money::Money(double amount) {
    this->representedValue = (money_t)round(amount * (double)100.0);
}

Money::Money(const Money & src) {
    Money(src._getValue());
}

money_t Money::_getValue() const {
    return representedValue;
}

void Money::_setValue(money_t m) {
    this->representedValue = m;
}

double Money::getDoubleValue() {
    return ((double)this->representedValue / (double)100.0);
}

string Money::getRawStringValue() const {
    char buffer[16];
    money_t whole = this->representedValue / 100;
    money_t decimal = this->representedValue - (whole * 100);

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

    this->representedValue = rhs._getValue();

    return *this;
}

Money Money::operator+(const Money & rhs) {
    Money money = Money(this->_getValue() + rhs._getValue());
    return money;
}

Money Money::operator+=(const Money & rhs) {
    *this = *this + rhs;
    return *this;
}

Money Money::operator-(const Money & rhs) {
    Money money = Money(this->_getValue() - rhs._getValue());
    return money;
}

Money Money::operator-=(const Money & rhs) {
    *this = *this - rhs;
    return *this;
}

Money Money::operator*(const Money & rhs) {
    Money money = Money(this->_getValue() * rhs._getValue());
    return money;
}

Money Money::operator*=(const Money & rhs) {
    *this = *this * rhs;
    return *this;
}

Money Money::operator/(const Money & rhs) {
    Money money = Money(this->_getValue() / rhs._getValue());
    return money;
}

Money Money::operator/=(const Money & rhs) {
    *this = *this / rhs;
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

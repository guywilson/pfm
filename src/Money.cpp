#include <string>
#include <iostream>
#include <sstream>
#include <locale>
#include <iomanip>
#include <string.h>
#include <math.h>
#include <stdint.h>

#include "logger.h"
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

    for (int i = 0;i < (int)strlen(amount);i++) {
        if (amount[i] == '.') {
            decimalPointPos = i;
            break;
        }
    }

    return decimalPointPos;
}

void Money::copyToDecimalPoint(char * targetBuffer, const char * amount, int targetBufferLen) {
    int decimalPointPos = findDecimalPointPos(amount);

    if (decimalPointPos > (targetBufferLen - 1)) {
        decimalPointPos = (targetBufferLen - 1);
    }

    int i;
    for (i = 0;i < decimalPointPos;i++) {
        targetBuffer[i] = amount[i];
    }

    targetBuffer[i] = 0;
}

void Money::copyAfterDecimalPoint(char * targetBuffer, const char * amount, int targetBufferLen) {
    int decimalPointPos = findDecimalPointPos(amount);

    int j = 0;
    for (int i = decimalPointPos + 1;i < (int)strlen(amount) && j < (targetBufferLen - 1);i++) {
        targetBuffer[j++] = amount[i];
    }

    targetBuffer[j] = 0;
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

money_t Money::getWholeValueFromString(const char * amount) {
    int decimalPointPos = findDecimalPointPos(amount);

    log.info("Money::getWholeValue(%s), decimal index %d", amount, decimalPointPos);
    
    money_t whole;

    if (decimalPointPos == DECIMAL_POINT_NOT_FOUND_VALUE) {
        whole = (money_t)strtol(amount, NULL, 10);
    }
    else {
        char buffer[AMOUNT_BUFFER_LENGTH];

        copyToDecimalPoint(buffer, amount, AMOUNT_BUFFER_LENGTH);
        log.info("Got whole number buffer '%s'", buffer);
        whole = (money_t)strtol(buffer, NULL, 10);
    }

    log.info("Money::getWholeValue() = %u", whole);

    return whole;
}

money_t Money::getDecimalValueFromString(const char * amount) {
    int decimalPointPos = findDecimalPointPos(amount);

    log.debug("Money::getDecimalValue(%s), decimal index %d", amount, decimalPointPos);
    
    money_t decimal;

    if (decimalPointPos == DECIMAL_POINT_NOT_FOUND_VALUE) {
        decimal = 0L;
    }
    else {
        char buffer[AMOUNT_BUFFER_LENGTH];

        copyAfterDecimalPoint(buffer, amount, AMOUNT_BUFFER_LENGTH);

        /*
        ** If the amount after the decimal point is only 1 char,
        ** e.g. 96.2, this is actually 96.20, not 96.02. This can
        ** happen if we are importing transactions for example from a
        ** spreadsheet...
        */
        if (strlen(buffer) == 1) {
            strncat(buffer, "0", AMOUNT_BUFFER_LENGTH - 1);
        }

        decimal = (money_t)strtol(buffer, NULL, 10);
    }

    log.debug("Money::getDecimalValue() = %u", decimal);

    return decimal;
}

void Money::_setValue(const char * amount) {
    money_t whole = getWholeValueFromString(amount);
    money_t decimal = getDecimalValueFromString(amount);

    money_t result = makeRepresentedValue(whole, decimal);

    _setValue(result);
}

void Money::_setValue(const string & amount) {
    _setValue(amount.c_str());
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
    char buffer[AMOUNT_BUFFER_LENGTH];
    money_t whole = this->representedValue / 100;
    money_t decimal = this->representedValue - (whole * 100);

    if (decimal < 0) {
        decimal = decimal * -1;
    }

    snprintf(buffer, AMOUNT_BUFFER_LENGTH, "%d.%02d", whole, decimal);

    string value = buffer;
    return value;
}

string Money::getFormattedStringValue() const {    
    char buffer[AMOUNT_BUFFER_LENGTH];
    snprintf(buffer, AMOUNT_BUFFER_LENGTH, "%d", this->representedValue);

    string raw = buffer;

    stringstream s;

    s.imbue(locale(""));
    s << showbase << put_money(raw);

    return s.str();
}

Money & Money::operator=(const Money & rhs) {
    if (this == &rhs) {
        return *this;
    }

    this->_setValue(rhs._getValue());

    return *this;
}

Money & Money::operator=(const string & rhs) {
    this->_setValue(rhs);
    return *this;
}

Money & Money::operator=(const char * rhs) {
    this->_setValue(rhs);
    return *this;
}

Money & Money::operator=(const double rhs) {
    this->_setValue(rhs);
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

const Money Money::operator*(const int rhs) {
    Money result = *this;
    result *= rhs;
    return result;
}

Money & Money::operator*=(const int rhs) {
    this->representedValue *= rhs;
    return *this;
}

const Money Money::operator/(const int rhs) {
    Money result = *this;
    result /= rhs;
    return result;
}

Money & Money::operator/=(const int rhs) {
    double result = (double)this->representedValue;
    result /= (double)rhs;
    this->representedValue = (money_t)round(result);
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

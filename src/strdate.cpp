#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <locale>
#include <ctype.h>

#include "logger.h"
#include "pfm_error.h"
#include "strdate.h"

using namespace std;

#define EPOCH_YEAR                  1970
#define EPOCH_MONTH                    1
#define EPOCH_DAY                      1

#define CURRENT_DATE_BUFFER_LENGTH    11

static string _currentDate;
static bool _isDateOverride = false;

void setOverrideDate(const string & date) {
    _currentDate = date;
    _isDateOverride = true;
}

void clearOverrideDate() {
    _isDateOverride = false;
}

static void fillTimeStruct(TimeDetails * time) {
    struct timeval tv;
    gettimeofday(&tv, nullptr);

    struct tm ltime;
    time_t t = tv.tv_sec;

    localtime_r(&t, &ltime);

    time->year = (uint16_t)(ltime.tm_year + 1900);
    time->month = (uint8_t)(ltime.tm_mon + 1);
    time->day = (uint8_t)ltime.tm_mday;

    time->hour = (uint8_t)ltime.tm_hour;
    time->minute = (uint8_t)ltime.tm_min;
    time->second = (uint8_t)ltime.tm_sec;

    time->microsecond = (uint32_t)tv.tv_usec;
}

void checkForInvalidChars(const string & date) {
    for (size_t i = 0;i < date.length();i++) {
        char c = date[i];

        if (!isdigit(c) && c != '-' && c != '/') {
            throw pfm_validation_error(
                        pfm_error::buildMsg(
                            "\nInvalid date string '%s': Date must be in the format 'dd-mm-yyyy' or 'yyyy-mm-dd'",
                            date.c_str()),
                        __FILE__,
                        __LINE__);
        }
    }
}

StrDate::StrDate() {
    this->_date = StrDate::today();
}

StrDate::StrDate(const StrDate & sd) {
    this->set(sd.shortDate());
}

StrDate::StrDate(const string & sd) {
    this->set(sd);
}

StrDate::StrDate(const char * pszDate) {
    this->set(pszDate);
}

StrDate::StrDate(int year, int month, int day) {
    char dateStr[TIME_STAMP_BUFFER_LEN];

    snprintf(
        dateStr, 
        TIME_STAMP_BUFFER_LEN, 
        "%d-%02d-%02d", 
        year, 
        month, 
        day);

    this->set(dateStr);
}

bool StrDate::isYear(string & part) {
    StrDate today;

    if (part.length() == 4) {
        for (int i = 0;i < (int)part.length();i++) {
            if (!isdigit(part[i])) {
                throw pfm_validation_error("Invalid date format");
            }
        }

        long yearCandidate = strtol(part.c_str(), NULL, 10);

        if (yearCandidate < 1970) {
            throw pfm_validation_error("Invalid date format");
        }

        return true;
    }

    return false;
}

bool StrDate::isMonth(string & part) {
    long monthCandidate = strtol(part.c_str(), NULL, 10);

    if (monthCandidate > 12) {
        return false;
    }

    return true;
}

/*
** Accepted formats:
**
** yyyy-mm-dd
** dd-mm-yyyy
** As above but with / rather than -
*/
StrDate::YMD StrDate::splitDate(const string & date) {
    if (date.length() != DATE_STRING_LENGTH) {
        throw pfm_validation_error(
                pfm_error::buildMsg(
                    "Invalid date '%s': Invalid date length, date must be in the format 'yyyy-mm-dd' or 'dd-mm-yyyy'",
                    date.c_str()),
                __FILE__,
                __LINE__);
    }

    char dateBuffer[DATE_STAMP_BUFFER_LEN];
    char * pszDate = dateBuffer;

    strncpy(dateBuffer, date.c_str(), DATE_STRING_LENGTH);
    dateBuffer[DATE_STRING_LENGTH] = 0;

    string part1 = strtok_r(dateBuffer, "-/", &pszDate);
    string part2 = strtok_r(NULL, "-/", &pszDate);
    string part3 = strtok_r(NULL, "-/", &pszDate);

    StrDate::YMD dateComponents;

    if (isYear(part1)) {
        dateComponents.year = (unsigned int)atoi(part1.c_str());
    }
    else {
        dateComponents.day = (unsigned int)atoi(part1.c_str());
    }

    if (isMonth(part2)) {
        dateComponents.month = (unsigned int)atoi(part2.c_str());
    }
    else {
        throw pfm_validation_error(
                pfm_error::buildMsg(
                    "Invalid date '%s': Invalid date length, date must be in the format 'yyyy-mm-dd' or 'dd-mm-yyyy'",
                    date.c_str()),
                __FILE__,
                __LINE__);
    }

    if (dateComponents.year != 0) {
        dateComponents.day = (unsigned int)atoi(part3.c_str());
    }
    else if (isYear(part3)) {
        dateComponents.year = (unsigned int)atoi(part3.c_str());
    }

    return dateComponents;
}

string StrDate::today() {
    if (!_isDateOverride) {
        auto twoDigits = [](int v) {
            string s = to_string(v);

            if (s.size() < 2) {
                s.insert(s.begin(), '0');
            }

            return s;
        };

        auto fourDigits = [](int v) {
            string s = to_string(v);

            while (s.size() < 4) {
                s.insert(s.begin(), '0');
            }

            return s;
        };

        TimeDetails td;
        fillTimeStruct(&td);

        _currentDate = 
                fourDigits(td.year) + "-" +
                twoDigits(td.month) + "-" +
                twoDigits(td.day);
    }

    return _currentDate;
}

string StrDate::getTimestamp() {
    return getTimestamp(false);
}

string StrDate::getTimestampToMicrosecond() {
    return getTimestamp(true);
}

string StrDate::getTimestamp(bool includeus) {
    TimeDetails td;
    fillTimeStruct(&td);

    auto twoDigits = [](int v) {
        string s = to_string(v);

        if (s.size() < 2) {
            s.insert(s.begin(), '0');
        }

        return s;
    };

    auto fourDigits = [](int v) {
        string s = to_string(v);

        while (s.size() < 4) {
            s.insert(s.begin(), '0');
        }

        return s;
    };

    string ts =
        fourDigits(td.year) + "-" +
        twoDigits(td.month) + "-" +
        twoDigits(td.day) + " " +
        twoDigits(td.hour) + ":" +
        twoDigits(td.minute) + ":" +
        twoDigits(td.second);

    if (includeus) {
        ts += ".";
        ts += to_string(static_cast<int>(td.microsecond));
    }

    return ts;
}

int StrDate::getDaysInMonth(int year, int month) {
    StrDate d;
    return d.daysInMonth(year, month);
}

bool StrDate::isDateValid(const string & date) {
    try {
        StrDate d(date);
    }
    catch (pfm_validation_error & e) {
        return false;
    }

    return true;
}

void StrDate::validateDateString(const string & date) {
    if (date == "N/A") {
        return;
    }

    checkForInvalidChars(date);

    StrDate::YMD dateComponents = splitDate(date);

    /*
    ** Valid date in the format 'yyyy-mm-dd' or 'dd-mm-yyyy'
    ** e.g. 2024-07-04 or 04-07-2024
    */
    if (dateComponents.year < EPOCH_YEAR) {
        throw pfm_validation_error(
                pfm_error::buildMsg(
                    "Invalid date '%s': Date must be greater than '1970-01-01'",
                    date.c_str()),
                __FILE__,
                __LINE__);
    }
    if (dateComponents.month < 1 || dateComponents.month > 12) {
        throw pfm_validation_error(
                pfm_error::buildMsg(
                    "Invalid date '%s': Invalid month, must be between 1 and 12",
                    date.c_str()),
                __FILE__,
                __LINE__);
    }
    if (dateComponents.day < 0 || dateComponents.day > 31) {
        throw pfm_validation_error(
                pfm_error::buildMsg(
                    "Invalid date '%s': Invalid day, must be between 1 and 31",
                    date.c_str()),
                __FILE__,
                __LINE__);
    }
    if ((dateComponents.month == 4 || dateComponents.month == 6 || dateComponents.month == 9 || dateComponents.month == 11) && dateComponents.day > 30) {
        throw pfm_validation_error(
                pfm_error::buildMsg(
                    "Invalid date '%s': Supplied month has 30 days",
                    date.c_str()),
                __FILE__,
                __LINE__);
    }
    if (dateComponents.month == 2) {
        if (isLeapYear(dateComponents.year)) {
            if (dateComponents.day > 29) {
                throw pfm_validation_error(
                        pfm_error::buildMsg(
                            "Invalid date '%s': February has max 29 days in a leap year",
                            date.c_str()),
                        __FILE__,
                        __LINE__);
            }
        }
        else {
            if (dateComponents.day > 28) {
                throw pfm_validation_error(
                        pfm_error::buildMsg(
                            "Invalid date '%s': February has max 28 days in a non-leap year",
                            date.c_str()),
                        __FILE__,
                        __LINE__);
            }
        }
    }
}

string StrDate::shortDate() const {
    return _date;
}

string StrDate::getDisplayDate() const {
    if (isEpoch()) {
        return "N/A";
    }
    else {
        return shortDate();
    }
}

void StrDate::set(const string & date) {
    set(date.c_str());
}

void StrDate::set(const char * date) {
    if (strncmp(date, _nullDate.c_str(), 3) == 0) {
        clear();
    }
    else if (strlen(date) > 0) {
        validateDateString(date);

        StrDate::YMD dateComponents = splitDate(date);
        this->set(dateComponents);
    }
    else {
        clear();
    }
}

void StrDate::set(int year, int month, int day) {
    char dateStr[TIME_STAMP_BUFFER_LEN];

    snprintf(
        dateStr, 
        TIME_STAMP_BUFFER_LEN, 
        "%d-%02d-%02d", 
        year, 
        month, 
        day);

    this->_date = dateStr;
}

void StrDate::set(StrDate::YMD & date) {
    set(date.year, date.month, date.day);
}

void StrDate::clear() {
    this->_date = _nullDate;
}

bool StrDate::isNull() {
    return (_date.compare(_nullDate) == 0 ? true : false);
}

time_t StrDate::epoch() {
    const time_t    secsPerDay = 3600 * 24;
    time_t          value = 0;
    
    for (int i = EPOCH_YEAR;i < year();i++) {
        value += (secsPerDay * (StrDate::isLeapYear(i) ? 366 : 365));
    }

    for (int i = EPOCH_MONTH;i < month();i++) {
        value += (secsPerDay * daysInMonth(year(), i));
    }

    /*
    ** Months start at day 1, not day 0 so subtract
    ** 1 from the day...
    */
    value += ((day() - 1) * secsPerDay);
    
    return value;
}

bool StrDate::isLeapYear(int year) {
    if (year % 4 == 0) {
        if (year % 100 == 0) {
            if (year % 400 == 0) {
                return true;
            }
            else {
                return false;
            }
        }
        else {
            return true;
        }
    }
    else {
        return false;
    }
}

bool StrDate::isLeapYear() {
    return(StrDate::isLeapYear(year()));
}

int StrDate::daysInMonth(int year, int month) {
    if (month == 4 || month == 6 || month == 9 || month == 11) {
        return 30;
    }
    else if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) {
        return 31;
    }
    else if (month == 2) {
        if (StrDate::isLeapYear(year)) {
            return 29;
        }
        else {
            return 28;
        }
    }
    else {
        return -1;
    }
}

int StrDate::daysInMonth() {
    return daysInMonth(year(), month());
}

StrDate StrDate::firstDayInMonth() {
    StrDate newDate(year(), month(), 1);
    return newDate;
}

StrDate StrDate::lastDayInMonth() {
    StrDate newDate(year(), month(), daysInMonth());
    return newDate;
}

int StrDate::year() const {
    return atoi(_date.substr(0, 4).c_str());
}

int StrDate::month() const {
    return atoi(_date.substr(5, 2).c_str());
}

int StrDate::day() const {
    return atoi(_date.substr(8, 2).c_str());
}

StrDate::sd_weekday StrDate::dayOfTheWeek() const {
    time_t rawTime;

    time(&rawTime);
    struct tm * timeInfo = localtime(&rawTime);

    timeInfo->tm_year = year() - 1900;
    timeInfo->tm_mon = month() - 1;
    timeInfo->tm_mday = day();

    mktime(timeInfo);

    return (StrDate::sd_weekday)(timeInfo->tm_wday);
}

bool StrDate::isSunday() const {
    StrDate::sd_weekday weekDay = dayOfTheWeek();

    if (weekDay == sd_sunday) {
        return true;
    }

    return false;
}

bool StrDate::isSaturday() const {
    StrDate::sd_weekday weekDay = dayOfTheWeek();

    if (weekDay == sd_saturday) {
        return true;
    }

    return false;
}

bool StrDate::isWeekend() const {
    if (isSaturday() || isSunday()) {
        return true;
    }

    return false;
}

bool StrDate::isEpoch() const {
    return (year() == EPOCH_YEAR && month() == EPOCH_MONTH && day() == EPOCH_DAY);
}

StrDate StrDate::addYears(int years) {
    int y = year();
    int m = month();
    int d = day();
    
    int newYear = y + years;
    
    if (m == 2 && d == 29 && !isLeapYear()) {
        d = 28;
    }
    
    StrDate newDate(newYear, m, d);

    return newDate;
}

StrDate StrDate::addMonths(int months) {
    int y = year();
    int m = month();
    int d = day();
    
    int monthCounter = months;

    if (monthCounter < 0) {
        monthCounter *= -1;
    }

    while (monthCounter > 0) {
        months >= 0 ? m++ : m--;
        monthCounter--;

        if (m > 12) {
            m = 1;
            y++;
        }
        else if (m < 1) {
            m = 12;
            y--;
        }
    }

    if (d > StrDate::daysInMonth(y, m)) {
        d = daysInMonth(y, m);
    }
    
    StrDate newDate(y, m, d);

    return newDate;
}

StrDate StrDate::addWeeks(int weeks) {
    return addDays(weeks * 7);
}

StrDate StrDate::addDays(int days) {
    int y = year();
    int m = month();
    int d = day();
    
    int dayCounter = days;

    if (dayCounter < 0) {
        dayCounter *= -1;
    }

    while (dayCounter > 0) {
        days >= 0 ? d++ : d--;
        dayCounter--;

        if (d > daysInMonth(y, m)) {
            m++;
            d = 1;
        }
        else if (d < 1) {
            m--;
            d = daysInMonth(y, m);
        }
        if (m > 12) {
            y++;
            m = 1;
            d = 1;
        }
        else if (m < 1) {
            y--;
            m = 12;
            d = daysInMonth(y, m);
        }
    }
    
    StrDate newDate(y, m, d);

    return newDate;
}

StrDate & StrDate::operator=(const StrDate & rhs) {
    if (this == &rhs) {
        return *this;
    }

    this->set(rhs.shortDate());

    return *this;
}

StrDate & StrDate::operator=(const string & rhs) {
    this->set(rhs);
    return *this;
}

StrDate & StrDate::operator=(const char * rhs) {
    this->set(rhs);
    return *this;
}

bool StrDate::operator==(StrDate & rhs) {
    return (this->epoch() == rhs.epoch());
}

bool StrDate::operator==(string & rhs) {
    StrDate d1(rhs);
    return (this->epoch() == d1.epoch());
}

bool StrDate::operator<(StrDate & rhs) {
    return (this->epoch() < rhs.epoch());
}

bool StrDate::operator<(string & rhs) {
    StrDate d1(rhs);
    return (this->epoch() < d1.epoch());
}

bool StrDate::operator<=(StrDate & rhs) {
    return (this->epoch() <= rhs.epoch());
}

bool StrDate::operator<=(string & rhs) {
    StrDate d1(rhs);
    return (this->epoch() <= d1.epoch());
}

bool StrDate::operator>(StrDate & rhs) {
    return (this->epoch() > rhs.epoch());
}

bool StrDate::operator>(string & rhs) {
    StrDate d1(rhs);
    return (this->epoch() > d1.epoch());
}

bool StrDate::operator>=(StrDate & rhs) {
    return (this->epoch() >= rhs.epoch());
}

bool StrDate::operator>=(string & rhs) {
    StrDate d1(rhs);
    return (this->epoch() >= d1.epoch());
}

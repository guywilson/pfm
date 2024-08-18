#include <sstream>
#include <iomanip>
#include <string>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "pfm_error.h"
#include "strdate.h"

using namespace std;

StrDate::StrDate() {
    this->_date = StrDate::today();
}

StrDate::StrDate(StrDate & sd) {
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

string StrDate::today() {
    struct timeval		tv;
    struct tm *         localTime;
    time_t				t;
    char *              today;

    gettimeofday(&tv, NULL);
    t = tv.tv_sec;
    localTime = localtime(&t);

    today = (char *)malloc(DATE_STAMP_BUFFER_LEN);

    if (today != NULL) {
        snprintf(
            today, 
            DATE_STAMP_BUFFER_LEN, 
            "%d-%02d-%02d", 
            localTime->tm_year + 1900, 
            localTime->tm_mon + 1, 
            localTime->tm_mday);
    }

    return string(today);
}

string StrDate::getTimestamp() {
    struct timeval		tv;
    struct tm *         localTime;
    time_t				t;
    char *              now;

    gettimeofday(&tv, NULL);
    t = tv.tv_sec;
    localTime = localtime(&t);

    now = (char *)malloc(TIME_STAMP_BUFFER_LEN);

    if (now != NULL) {
        snprintf(
            now, 
            TIME_STAMP_BUFFER_LEN, 
            "%d-%02d-%02d %02d:%02d:%02d", 
            localTime->tm_year + 1900, 
            localTime->tm_mon + 1, 
            localTime->tm_mday,
            localTime->tm_hour,
            localTime->tm_min,
            localTime->tm_sec);
    }

    return string(now);
}

bool StrDate::isDateValid(const string & date) {
    try {
        StrDate d(date);
        d.validateDateString(d.shortDate());
    }
    catch (pfm_validation_error & e) {
        return false;
    }

    return true;
}

void StrDate::validateDateString(const string & date) {
    int                 day;
    int                 month;
    int                 year;

    if (date.length() < 10) {
        throw pfm_validation_error(
                pfm_error::buildMsg(
                    "Invalid date '%s': Invalid date length, date must be in the format 'yyyy-mm-dd'",
                    date.c_str()),
                __FILE__,
                __LINE__);
    }

    /*
    ** Valid date in the format 'yyyy-mm-dd'
    ** e.g. 2024-07-04
    */
    year = atoi(date.substr(0, 4).c_str());
    month = atoi(date.substr(5, 2).c_str());
    day = atoi(date.substr(8, 2).c_str());

    if (year < 1900) {
        throw pfm_validation_error(
                pfm_error::buildMsg(
                    "Invalid date '%s': Date must be greater than '1900-01-01'",
                    date.c_str()),
                __FILE__,
                __LINE__);
    }
    if (month < 1 || month > 12) {
        throw pfm_validation_error(
                pfm_error::buildMsg(
                    "Invalid date '%s': Invalid month, must be between 1 and 12",
                    date.c_str()),
                __FILE__,
                __LINE__);
    }
    if (day < 0 || day > 31) {
        throw pfm_validation_error(
                pfm_error::buildMsg(
                    "Invalid date '%s': Invalid day, must be between 1 and 31",
                    date.c_str()),
                __FILE__,
                __LINE__);
    }
    if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) {
        throw pfm_validation_error(
                pfm_error::buildMsg(
                    "Invalid date '%s': Supplied month has 30 days",
                    date.c_str()),
                __FILE__,
                __LINE__);
    }
    if (month == 2) {
        if (isLeapYear(year)) {
            if (day > 29) {
                throw pfm_validation_error(
                        pfm_error::buildMsg(
                            "Invalid date '%s': February has max 29 days in a leap year",
                            date.c_str()),
                        __FILE__,
                        __LINE__);
            }
        }
        else {
            if (day > 28) {
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

void StrDate::set(const string & date) {
    set(date.c_str());
}

void StrDate::set(const char * date) {
    validateDateString(date);

    this->_date = date;
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

    this->set(dateStr);
}

time_t StrDate::epoch() {
    const time_t    secsPerDay = 3600 * 24;
    time_t          value = 0;
    
    for (int i = 1970;i < year();i++) {
        value += (secsPerDay * (StrDate::isLeapYear(i) ? 366 : 365));
    }

    for (int i = 1;i < month();i++) {
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

int StrDate::year() {
    return atoi(_date.substr(0, 4).c_str());
}

int StrDate::month() {
    return atoi(_date.substr(5, 2).c_str());
}

int StrDate::day() {
    return atoi(_date.substr(8, 2).c_str());
}

void StrDate::addYears(int years) {
    int y = year();
    int m = month();
    int d = day();
    
    int newYear = y + years;
    
    if (m == 2 && d == 29 && !isLeapYear()) {
        d = 28;
    }
    
    set(newYear, m, d);
}

void StrDate::addMonths(int months) {
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

    if (d > daysInMonth()) {
        d = daysInMonth();
    }
    
    set(y, m, d);
}

void StrDate::addWeeks(int weeks) {
    addDays(weeks * 7);
}

void StrDate::addDays(int days) {
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

        // cout << "[" << y << ", " << m << ", " << d << "] (" << dayCounter << ")" << endl;
    }
    
    set(y, m, d);
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

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

bool StrDate::validateDate(const string & date) {
    int                 day;
    int                 month;
    int                 year;

    if (date.length() == 0) {
        /*
        ** Allow blank date for nullable fields...
        */
        return true;
    }
    else if (date.length() < 10) {
        return false;
    }

    /*
    ** Valid date in the format 'yyyy-mm-dd'
    ** e.g. 2024-07-04
    */
    year = atoi(date.substr(0, 4).c_str());
    month = atoi(date.substr(5, 2).c_str());
    day = atoi(date.substr(8, 2).c_str());

    if (year < 1900) {
        return false;
    }
    if (month < 1 || month > 12) {
        return false;
    }
    if (day < 0 || day > 31) {
        return false;
    }
    if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) {
        return false;
    }
    if (month == 2) {
        if (StrDate::isLeapYear(year)) {
            if (day > 29) {
                return false;
            }
        }
        else {
            if (day > 28) {
                return false;
            }
        }
    }

    return true;
}

string StrDate::shortDate() const {
    return _date;
}

void StrDate::set(const string & date) {
    if (StrDate::validateDate(date)) {
        this->_date.assign(date);
    }
    else {
        throw pfm_validation_error(pfm_error::buildMsg("Invalid date string: %s", date.c_str()));
    }
}

void StrDate::set(const char * date) {
    if (StrDate::validateDate(date)) {
        this->_date.assign(date);
    }
    else {
        throw pfm_validation_error(pfm_error::buildMsg("Invalid date string: %s", date));
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

    this->set(dateStr);
}

time_t StrDate::epoch() {
    return StrDate::epoch(_date);
}

time_t StrDate::epoch(string & date) {
    const time_t    secsPerDay = 3600 * 24;
    time_t          value = 0;
    int             month = StrDate::month(date);
    int             year = StrDate::year(date);
    int             i;
    
    for (i = 1970;i < year;i++) {
        value += (secsPerDay * (StrDate::isLeapYear(i) ? 366 : 365));
    }

    for (i = 1;i < month;i++) {
        value += (secsPerDay * StrDate::daysInMonth(year, i));
    }

    /*
    ** Months start at day 1, not day 0 so subtract
    ** 1 from the day...
    */
    value += ((day(date) - 1) * secsPerDay);
    
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

bool StrDate::isLeapYear(string & date) {
    return(StrDate::isLeapYear(StrDate::year(date)));
}

int StrDate::daysInMonth(int year, int month) {
    if (month == 4 || month == 6 || month == 9 || month == 11) {
        return 30;
    }
    else if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) {
        return 31;
    }
    else if (month == 2) {
        if (isLeapYear(year)) {
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

int StrDate::daysInMonth(string & date) {
    return StrDate::daysInMonth(StrDate::year(date), StrDate::month(date));
}

int StrDate::year() {
    return StrDate::year(_date);
}

int StrDate::month() {
    return StrDate::month(_date);
}

int StrDate::day() {
    return StrDate::day(_date);
}

int StrDate::year(string & date) {
    return atoi(date.substr(0, 4).c_str());
}

int StrDate::month(string & date) {
    return atoi(date.substr(5, 2).c_str());
}

int StrDate::day(string & date) {
    return atoi(date.substr(8, 2).c_str());
}

void StrDate::addYears(int years) {
    int y = year();
    int m = month();
    int d = day();
    
    int newYear = y + years;
    
    if (m == 2 && d == 29 && !isLeapYear(_date)) {
        d = 28;
    }
    
    set(newYear, m, d);
}

void StrDate::addMonths(int months) {
    int y = year();
    int m = month();
    int d = day();
    
    int totalMonths = m + months;
    int newYear = y + (totalMonths - 1) / 12;
    
    if (d > StrDate::daysInMonth(_date)) {
        d = StrDate::daysInMonth(_date);
    }
    
    set(newYear, totalMonths, d);
}

void StrDate::addWeeks(int weeks) {
    addDays(weeks * 7);
}

void StrDate::addDays(int days) {
    int y = year();
    int m = month();
    int d = day();
    
    int dayCounter = days;

    while (dayCounter > 0) {
        d++;
        dayCounter--;

        if (d > StrDate::daysInMonth(y, m)) {
            d = 1;
            m++;
        }
        if (m > 12) {
            y++;
            m = 1;
        }
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

bool StrDate::operator==(StrDate & rh) {
    return (epoch() == rh.epoch());
}

bool StrDate::operator==(string & rh) {
    return (epoch() == StrDate::epoch(rh));
}

bool StrDate::operator<(StrDate & rh) {
    return (epoch() < rh.epoch());
}

bool StrDate::operator<(string & rh) {
    return (epoch() < StrDate::epoch(rh));
}

bool StrDate::operator<=(StrDate & rh) {
    return (epoch() <= rh.epoch());
}

bool StrDate::operator<=(string & rh) {
    return (epoch() <= StrDate::epoch(rh));
}

bool StrDate::operator>(StrDate & rh) {
    return (epoch() > rh.epoch());
}

bool StrDate::operator>(string & rh) {
    return (epoch() > StrDate::epoch(rh));
}

bool StrDate::operator>=(StrDate & rh) {
    return (epoch() >= rh.epoch());
}

bool StrDate::operator>=(string & rh) {
    return (epoch() >= StrDate::epoch(rh));
}

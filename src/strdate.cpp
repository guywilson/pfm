#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <unordered_map>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <locale>
#include <ctype.h>
#include <cstdint>

#include "logger.h"
#include "pfm_error.h"
#include "cfgmgr.h"
#include "strdate.h"


#define EPOCH_YEAR                  1970
#define EPOCH_MONTH                    1
#define EPOCH_DAY                      1

#define CURRENT_DATE_BUFFER_LENGTH    11

static std::string _currentDate;
static bool _isDateOverride = false;
static std::unordered_map<std::string, std::string> _publicHolidays;

void setOverrideDate(const std::string & date) {
    _currentDate = date;
    _isDateOverride = true;
}

void clearOverrideDate() {
    _isDateOverride = false;
}

void addPublicHoliday(std::pair<StrDate, std::string> & holiday) {

    _publicHolidays.insert({holiday.first.shortDate(), holiday.second});
}

void clearPublicHolidays() {
    _publicHolidays.clear();
}

bool isPublicHoliday(const StrDate & date) {
    return _publicHolidays.contains(date.shortDate());
}

static void fillTimeStruct(TimeComponents * time) {
    auto twoDigits = [](int v) {
        std::string s = std::to_string(v);

        if (s.size() < 2) {
            s.insert(s.begin(), '0');
        }

        return s;
    };

    auto fourDigits = [](int v) {
        std::string s = std::to_string(v);

        while (s.size() < 4) {
            s.insert(s.begin(), '0');
        }

        return s;
    };

    struct timeval tv;
    gettimeofday(&tv, nullptr);

    struct tm ltime;
    time_t t = tv.tv_sec;

    localtime_r(&t, &ltime);

    time->year = fourDigits((int)(ltime.tm_year + 1900));
    time->month = twoDigits((int)(ltime.tm_mon + 1));
    time->day = twoDigits(ltime.tm_mday);

    time->hour = twoDigits((int)ltime.tm_hour);
    time->minute = twoDigits((int)ltime.tm_min);
    time->second = twoDigits((int)ltime.tm_sec);

    time->microsecond = std::to_string(static_cast<int>(tv.tv_usec));
}

static bool isNumeric(std::string & cfgDate) {
    bool isNumeric = true;
    for (int i = 0;i < (int)cfgDate.length();i++) {
        if (!isdigit(cfgDate.at(i))) {
            isNumeric = false;
            break;
        }
    }

    return isNumeric;
}

static void checkForInvalidChars(const std::string & date) {
    for (size_t i = 0;i < date.length();i++) {
        unsigned char c = static_cast<unsigned char>(date[i]);

        if (!isdigit(c) && !isalpha(c) && c != '-' && c != '/') {
            throw pfm_validation_error(
                        pfm_error::buildMsg(
                            "\nInvalid date string '%s': Date must be in the format 'dd-mm-yyyy', 'yyyy-mm-dd', 'dd-Mmm-yyyy' or 'yyyy-Mmm-dd'",
                            date.c_str()),
                        __FILE__,
                        __LINE__);
        }
    }
}

static std::string dayString(int day) {
    std::string comp;

    if (day == 1 || day == 21 || day == 31) {
        comp = std::to_string(day) + "st";
    }
    else if (day == 2 || day == 22) {
        comp = std::to_string(day) + "nd";
    }
    else if (day == 3 || day == 23) {
        comp = std::to_string(day) + "rd";
    }
    else {
        comp = std::to_string(day) + "th";
    }

    return comp;
}

static const std::string monthString(int month) {
    std::string comp;

    switch (month) {
        case 1:
            return "January";

        case 2:
            return "February";

        case 3:
            return "March";

        case 4:
            return "April";

        case 5:
            return "May";

        case 6:
            return "June";

        case 7:
            return "July";

        case 8:
            return "August";

        case 9:
            return "September";

        case 10:
            return "October";

        case 11:
            return "November";

        case 12:
            return "December";
        
        default:
            return "Unknown";
    }
}

StrDate::StrDate() {
    this->_date = StrDate::today();
}

StrDate::StrDate(const StrDate & sd) {
    this->set(sd.shortDate());
}

StrDate::StrDate(const std::string & sd) {
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

bool StrDate::isYear(std::string & part) {
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

bool StrDate::isMonth(std::string & part) {
    if (part.length() == 2 &&
        isdigit(static_cast<unsigned char>(part[0])) &&
        isdigit(static_cast<unsigned char>(part[1]))) {
        int monthCandidate = std::stoi(part);
        return monthCandidate >= 1 && monthCandidate <= 12;
    }

    static const std::string monthMmm[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    return part.length() == 3 &&
           std::find(std::begin(monthMmm), std::end(monthMmm), part) != std::end(monthMmm);
}

static int monthNumber(const std::string & part) {
    if (part.length() == 2 &&
        isdigit(static_cast<unsigned char>(part[0])) &&
        isdigit(static_cast<unsigned char>(part[1]))) {
        return std::stoi(part);
    }

    static const std::string monthMmm[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    auto month = std::find(std::begin(monthMmm), std::end(monthMmm), part);
    return month == std::end(monthMmm) ? 0 : std::distance(std::begin(monthMmm), month) + 1;
}

static bool isTwoDigitNumber(const std::string & part) {
    return part.length() == 2 &&
           isdigit(static_cast<unsigned char>(part[0])) &&
           isdigit(static_cast<unsigned char>(part[1]));
}

/*
** Accepted formats:
**
** yyyy-mm-dd or yyyy-Mmm-dd
** dd-mm-yyyy or dd-Mmm-yyyy
** As above but with / rather than -
*/
StrDate::YMD StrDate::splitDate(const std::string & date) {
    auto invalidFormat = [&date]() {
        throw pfm_validation_error(
                pfm_error::buildMsg(
                    "Invalid date '%s': Date must be in the format 'yyyy-mm-dd', 'dd-mm-yyyy', 'yyyy-Mmm-dd' or 'dd-Mmm-yyyy'",
                    date.c_str()),
                __FILE__,
                __LINE__);
    };

    size_t firstSeparator = date.find_first_of("-/");
    size_t secondSeparator = firstSeparator == std::string::npos
            ? std::string::npos
            : date.find_first_of("-/", firstSeparator + 1);

    if (firstSeparator == std::string::npos ||
        secondSeparator == std::string::npos ||
        date.find_first_of("-/", secondSeparator + 1) != std::string::npos ||
        date[firstSeparator] != date[secondSeparator]) {
        invalidFormat();
    }

    std::string part1 = date.substr(0, firstSeparator);
    std::string part2 = date.substr(firstSeparator + 1, secondSeparator - firstSeparator - 1);
    std::string part3 = date.substr(secondSeparator + 1);

    bool isYearFirst = part1.length() == 4;
    std::string & yearPart = isYearFirst ? part1 : part3;
    std::string & dayPart = isYearFirst ? part3 : part1;

    if (!isTwoDigitNumber(dayPart) || !isMonth(part2) || !isYear(yearPart)) {
        invalidFormat();
    }

    StrDate::YMD dateComponents;
    dateComponents.year = (unsigned int)std::stoi(yearPart);
    dateComponents.month = (unsigned int)monthNumber(part2);
    dateComponents.day = (unsigned int)std::stoi(dayPart);

    return dateComponents;
}

std::string StrDate::today() {
    if (!_isDateOverride) {
        TimeComponents tc;
        fillTimeStruct(&tc);

        _currentDate = tc.year + "-" + tc.month + "-" + tc.day;
    }

    return _currentDate;
}

std::string StrDate::getTimestamp() {
    return getTimestamp(false);
}

std::string StrDate::getTimestampToMicrosecond() {
    return getTimestamp(true);
}

std::string StrDate::getTimestamp(bool includeus) {
    TimeComponents tc;
    fillTimeStruct(&tc);

    std::string ts =
        tc.year + "-" +
        tc.month + "-" +
        tc.day + " " +
        tc.hour + ":" +
        tc.minute + ":" +
        tc.second;

    if (includeus) {
        ts += ".";
        ts += tc.microsecond;
    }

    return ts;
}

int StrDate::getDaysInMonth(int year, int month) {
    StrDate d;
    return d.daysInMonth(year, month);
}

bool StrDate::isDateValid(const std::string & date) {
    try {
        StrDate d(date);
    }
    catch (pfm_validation_error & e) {
        return false;
    }

    return true;
}

void StrDate::validateDateString(const std::string & date) {
    if (date == "N/A") {
        return;
    }

    checkForInvalidChars(date);

    StrDate::YMD dateComponents = splitDate(date);

    if (dateComponents.year < EPOCH_YEAR) {
        throw pfm_validation_error(
                pfm_error::buildMsg(
                    "Invalid date '%s': Date must be on or after '1970-01-01'",
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
    if (dateComponents.day < 1 ||
        dateComponents.day > (unsigned int)daysInMonth(dateComponents.year, dateComponents.month)) {
        throw pfm_validation_error(
                pfm_error::buildMsg(
                    "Invalid date '%s': Invalid day for the supplied month and year",
                    date.c_str()),
                __FILE__,
                __LINE__);
    }
}

std::string StrDate::shortDate() const {
    return _date;
}

std::string StrDate::longDate() const {
    std::string ldate = dayString(day()) + " " + monthString(month()) + " " + std::to_string(year());

    return ldate;
}

std::string StrDate::getDisplayDate() const {
    if (isEpoch()) {
        return "N/A";
    }
    else {
        return shortDate();
    }
}

void StrDate::set(const StrDate & date) {
    set(date.year(), date.month(), date.day());
}

void StrDate::set(const std::string & date) {
    set(date.c_str());
}

void StrDate::set(const char * date) {
    if (strcmp(date, _nullDate.c_str()) == 0) {
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
    
    if (!isNull()) {
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
    }
    
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

int StrDate::getPeriodEndDay() {
    StrDate today;
    return getPeriodEndDay(today);
}

int StrDate::getPeriodEndDay(StrDate & referenceDate) {
    Logger & log = Logger::getInstance();
    log.entry("StrDate::getPeriodEndDay()");

    cfgmgr & cfg = cfgmgr::getInstance();

    std::string cycleEnd = cfg.getValue("cycle.end");

    log.info("Value of config item 'cycle.end' is '%s'", cycleEnd.c_str());

    int periodEnd;

    if (isNumeric(cycleEnd)) {
        periodEnd = atoi(cycleEnd.c_str());

        StrDate specificDate(referenceDate.year(), referenceDate.month(), periodEnd);

        while (specificDate.isWeekend() || isPublicHoliday(specificDate)) {
            specificDate = specificDate.addDays(-1);
        }

        periodEnd = specificDate.day();
    }
    else if (cycleEnd.compare("last-working-day") == 0) {
        StrDate lastWorkingDay = referenceDate;
        lastWorkingDay = lastWorkingDay.lastDayInMonth();

        while (lastWorkingDay.isWeekend() || isPublicHoliday(lastWorkingDay)) {
            lastWorkingDay = lastWorkingDay.addDays(-1);
        }

        periodEnd = lastWorkingDay.day();
    }
    else if (cycleEnd.compare("last-friday") == 0) {
        StrDate lastFriday = referenceDate;
        lastFriday = lastFriday.lastDayInMonth();

        while (lastFriday.dayOfTheWeek() != StrDate::sd_friday) {
            lastFriday = lastFriday.addDays(-1);
        }

        /*
        ** If the last friday is a public holiday, then
        ** roll back to the previous week...
        */
        if (isPublicHoliday(lastFriday)) {
            lastFriday = lastFriday.addDays(-7);
        }

        periodEnd = lastFriday.day();
    }
    else {
        StrDate lastDay = referenceDate.lastDayInMonth();
        periodEnd = lastDay.day();
    }

    log.debug("Got period end day as %d for date '%s'", periodEnd, referenceDate.shortDate().c_str());

    log.exit("StrDate::getPeriodEndDay()");

    return periodEnd;
}

StrDate StrDate::getPeriodStartDate() {
    StrDate today;
    return getPeriodStartDate(today);
}

StrDate StrDate::getPeriodEndDate() {
    StrDate today;
    return getPeriodEndDate(today);
}

StrDate StrDate::getPeriodStartDate(StrDate & referenceDate) {
    Logger & log = Logger::getInstance();
    log.entry("StrDate::getPeriodStartDate()");

    // Determine which period end applies to referenceDate
    StrDate periodEnd = getPeriodEndDate(referenceDate);

    // Previous period end is the configured end date in the month before periodEnd's month
    StrDate endMonth(periodEnd.year(), periodEnd.month(), 1);
    StrDate prevMonth = endMonth.addMonths(-1);

    const int prevEndDay = getPeriodEndDay(prevMonth);
    StrDate prevPeriodEnd(prevMonth.year(), prevMonth.month(), prevEndDay);

    // Start is the day after previous period end
    StrDate periodStart = prevPeriodEnd.addDays(1);

    log.debug("Period start for '%s' is '%s' (prev end '%s', current end '%s')",
              referenceDate.shortDate().c_str(),
              periodStart.shortDate().c_str(),
              prevPeriodEnd.shortDate().c_str(),
              periodEnd.shortDate().c_str());

    log.exit("StrDate::getPeriodStartDate()");
    return periodStart;
}

StrDate StrDate::getPeriodEndDate(StrDate & referenceDate) {
    Logger & log = Logger::getInstance();
    log.entry("StrDate::getPeriodEndDate()");

    // Compute end date for referenceDate's month
    StrDate thisMonth(referenceDate.year(), referenceDate.month(), 1);
    const int endDayThisMonth = getPeriodEndDay(thisMonth);
    StrDate endThisMonth(thisMonth.year(), thisMonth.month(), endDayThisMonth);

    // If referenceDate is on/before this month's configured end, that's the period end
    if (referenceDate <= endThisMonth) {
        log.debug("Period end for '%s' is '%s' (same month)",
                  referenceDate.shortDate().c_str(),
                  endThisMonth.shortDate().c_str());
        log.exit("StrDate::getPeriodEndDate()");
        return endThisMonth;
    }

    // Otherwise, period ends at next month's configured end
    StrDate nextMonth = thisMonth.addMonths(1);
    const int endDayNextMonth = getPeriodEndDay(nextMonth);
    StrDate endNextMonth(nextMonth.year(), nextMonth.month(), endDayNextMonth);

    log.debug("Period end for '%s' is '%s' (next month)",
              referenceDate.shortDate().c_str(),
              endNextMonth.shortDate().c_str());

    log.exit("StrDate::getPeriodEndDate()");
    return endNextMonth;
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
    int y = year() + years;
    int m = month();
    int d = day();

    if (m == 2 && d == 29 && !StrDate::isLeapYear(y)) {
        d = 28;
    }

    StrDate newDate(y, m, d);
    set(newDate);
    return *this;
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
    set(newDate);
    return *this;
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
    set(newDate);
    return *this;
}

StrDate & StrDate::operator=(const StrDate & rhs) {
    if (this == &rhs) {
        return *this;
    }

    this->set(rhs.shortDate());

    return *this;
}

StrDate & StrDate::operator=(const std::string & rhs) {
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

bool StrDate::operator==(std::string & rhs) {
    StrDate d1(rhs);
    return (this->epoch() == d1.epoch());
}

bool StrDate::operator!=(StrDate & rhs) {
    return !(*this == rhs);
}

bool StrDate::operator!=(std::string & rhs) {
    return !(*this == rhs);
}

bool StrDate::operator<(StrDate & rhs) {
    return (this->epoch() < rhs.epoch());
}

bool StrDate::operator<(std::string & rhs) {
    StrDate d1(rhs);
    return (this->epoch() < d1.epoch());
}

bool StrDate::operator<=(StrDate & rhs) {
    return (this->epoch() <= rhs.epoch());
}

bool StrDate::operator<=(std::string & rhs) {
    StrDate d1(rhs);
    return (this->epoch() <= d1.epoch());
}

bool StrDate::operator>(StrDate & rhs) {
    return (this->epoch() > rhs.epoch());
}

bool StrDate::operator>(std::string & rhs) {
    StrDate d1(rhs);
    return (this->epoch() > d1.epoch());
}

bool StrDate::operator>=(StrDate & rhs) {
    return (this->epoch() >= rhs.epoch());
}

bool StrDate::operator>=(std::string & rhs) {
    StrDate d1(rhs);
    return (this->epoch() >= d1.epoch());
}

// prefix increment
StrDate & StrDate::operator++() {
    StrDate d = this->addDays(1);
    this->set(d);

    return *this;
}

// postfix increment
StrDate StrDate::operator++(int) {
    StrDate old = *this;
    operator++();
    return old;
}

// prefix decrement
StrDate & StrDate::operator--() {
    StrDate d = this->addDays(-1);
    this->set(d);

    return *this;
}

// postfix decrement
StrDate StrDate::operator--(int) {
    StrDate old = *this;
    operator--();
    return old;
}

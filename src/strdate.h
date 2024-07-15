#include <sstream>
#include <iomanip>
#include <string>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "pfm_error.h"

using namespace std;

#ifndef __INCL_STRDATE
#define __INCL_STRDATE

#define TIME_STAMP_BUFFER_LEN               12

class StrDate {
    private:
        string              _date;

    public:
        StrDate() {
            this->_date = StrDate::today();
        }

        StrDate(StrDate & sd) {
            this->set(sd.shortDate());
        }

        StrDate(const string & sd) {
            this->set(sd);
        }

        StrDate(const char * pszDate) {
            this->set(pszDate);
        }

        StrDate(int year, int month, int day) {
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

        static string today() {
            struct timeval		tv;
            struct tm *         localTime;
            time_t				t;
            char *              today;

            gettimeofday(&tv, NULL);
            t = tv.tv_sec;
            localTime = localtime(&t);

            today = (char *)malloc(TIME_STAMP_BUFFER_LEN);

            if (today != NULL) {
                snprintf(
                    today, 
                    TIME_STAMP_BUFFER_LEN, 
                    "%d-%02d-%02d", 
                    localTime->tm_year + 1900, 
                    localTime->tm_mon + 1, 
                    localTime->tm_mday);
            }

            return string(today);
        }

        static bool validateDate(const string & date) {
            int                 day;
            int                 month;
            int                 year;

            if (date.length() < 10) {
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

        string shortDate() {
            return _date;
        }

        void set(const string & date) {
            if (StrDate::validateDate(date)) {
                this->_date.assign(date);
            }
            else {
                throw pfm_validation_error(pfm_error::buildMsg("Invalid date string: %s", date.c_str()));
            }
        }

        void set(const char * date) {
            if (StrDate::validateDate(date)) {
                this->_date.assign(date);
            }
            else {
                throw pfm_validation_error(pfm_error::buildMsg("Invalid date string: %s", date));
            }
        }

        void set(int year, int month, int day) {
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

        time_t epoch() {
            return StrDate::epoch(_date);
        }

        /*
        ** Calculates the number of seconds since Jan 1, 1970
        */
        static time_t epoch(string & date) {
            const time_t secsPerDay = 3600 * 24;
            time_t value = 0;
            
            value += ((StrDate::year(date) - 1970) * secsPerDay * 365);
            value += (secsPerDay * StrDate::daysInMonth(date));
            value += (day(date) * secsPerDay);
            
            return value;
        }

        static bool isLeapYear(int year) {
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

        static bool isLeapYear(string & date) {
            return(StrDate::isLeapYear(StrDate::year(date)));
        }

        static int daysInMonth(int year, int month) {
            if (month == 4 || month == 6 || month == 9 || month == 11) {
                return 30;
            }
            else if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) {
                return 31;
            }
            else {
                if (isLeapYear(year)) {
                    return 29;
                }
                else {
                    return 28;
                }
            }
        }

        static int daysInMonth(string & date) {
            return StrDate::daysInMonth(StrDate::year(date), StrDate::month(date));
        }
    
        int year() {
            return StrDate::year(_date);
        }

        int month() {
            return StrDate::month(_date);
        }

        int day() {
            return StrDate::day(_date);
        }

        static int year(string & date) {
            return atoi(date.substr(0, 4).c_str());
        }

        static int month(string & date) {
            return atoi(date.substr(5, 2).c_str());
        }

        static int day(string & date) {
            return atoi(date.substr(8, 2).c_str());
        }

//    function addYears(date, years):
//        year = getYear(date)
//        month = getMonth(date)
//        day = getDay(date)
//        
//        newYear = year + years
//        
//        # Adjust day if necessary for leap years, end of month, etc.
//        if month == 2 and day == 29 and not isLeapYear(newYear):
//            day = 28
//
//        return createDate(newYear, month, day)
        void addYears(int years) {
            int y = year();
            int m = month();
            int d = day();
            
            int newYear = y + years;
            
            if (m == 2 && d == 29 && !isLeapYear(_date)) {
                d = 28;
            }
            
            set(newYear, m, d);
        }

//    function addMonths(date, months):
//        year = getYear(date)
//        month = getMonth(date)
//        day = getDay(date)
//        
//        totalMonths = month + months
//        newYear = year + (totalMonths - 1) // 12
//        newMonth = (totalMonths - 1) % 12 + 1
//        
//        # Adjust day if necessary for end of month, etc.
//        maxDay = daysInMonth(newYear, newMonth)
//        if day > maxDay:
//            day = maxDay
//
//        return createDate(newYear, newMonth, day)
        void addMonths(int months) {
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

        void addWeeks(int weeks) {
            addDays(weeks * 7);
        }

        void addDays(int days) {
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

        bool operator==(StrDate & rh) {
            return (epoch() == rh.epoch());
        }

        bool operator==(string & rh) {
            return (epoch() == StrDate::epoch(rh));
        }

        bool operator<(StrDate & rh) {
            return (epoch() < rh.epoch());
        }

        bool operator<(string & rh) {
            return (epoch() < StrDate::epoch(rh));
        }

        bool operator>(StrDate & rh) {
            return (epoch() > rh.epoch());
        }
        
        bool operator>(string & rh) {
            return (epoch() > StrDate::epoch(rh));
        }
};

#endif

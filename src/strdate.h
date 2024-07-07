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
            struct timeval		tv;
            struct tm *         localTime;
            time_t				t;
            int                 day;
            int                 month;
            int                 year;

            gettimeofday(&tv, NULL);
            t = tv.tv_sec;
            localTime = localtime(&t);

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

            if (year < 1900 || year > (localTime->tm_year + 1900)) {
                return false;
            }
            if (month < 1 || month > 12) {
                return false;
            }
            if (month > (localTime->tm_mon + 1)) {
                return false;
            }
            if (day < 0 || day > 31) {
                return false;
            }
            if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) {
                return false;
            }
            if ((month == 2) && day > 29) {
                return false;
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

        static time_t epoch(string & date) {
            tm dt;

            istringstream ss(date);

            // Parse the date string using std::get_time
            ss >> get_time(&dt, "%Y-%m-%d");

            // Check if parsing was successful
            if (ss.fail()) {
                throw pfm_error(
                        pfm_error::buildMsg(
                            "Failed to parse date '%s'", 
                            date.c_str()), 
                        __FILE__, 
                        __LINE__);
            }

            return mktime(&dt);
        }

        int year() {
            return atoi(_date.substr(0, 4).c_str());
        }

        int month() {
            return atoi(_date.substr(5, 2).c_str());
        }

        int day() {
            return atoi(_date.substr(8, 2).c_str());
        }

        void addYears(int years) {
            tm dt;

            istringstream ss(_date);

            // Parse the date string using std::get_time
            ss >> get_time(&dt, "%Y-%m-%d");

            dt.tm_year += years;

            mktime(&dt);

            set(dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday);
        }

        void addMonths(int months) {
            tm dt;

            istringstream ss(_date);

            // Parse the date string using std::get_time
            ss >> get_time(&dt, "%Y-%m-%d");

            dt.tm_mon += months;

            mktime(&dt);

            set(dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday);
        }

        void addWeeks(int weeks) {
            addDays(weeks * 7);
        }

        void addDays(int days) {
            tm dt;

            istringstream ss(_date);

            // Parse the date string using std::get_time
            ss >> get_time(&dt, "%Y-%m-%d");

            dt.tm_mday += days;

            mktime(&dt);

            set(dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday);
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

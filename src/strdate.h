#include <string>
#include <time.h>
#include <sys/time.h>

#include "pfm_error.h"

using namespace std;

#ifndef __INCL_STRDATE
#define __INCL_STRDATE

#define TIME_STAMP_BUFFER_LEN               20
#define DATE_STAMP_BUFFER_LEN               12

class StrDate {
    private:
        string _date;

        /*
        ** Calculates the number of seconds since Jan 1, 1970
        */
        time_t epoch();
        bool isLeapYear();

        int daysInMonth(int month);
        int daysInMonth();

        void validateDateString(const string & date);

        static bool isLeapYear(int year);

    public:
        StrDate();
        StrDate(StrDate & sd);
        StrDate(const string & sd);
        StrDate(const char * pszDate);
        StrDate(int year, int month, int day);

        void set(const string & date);
        void set(const char * date);
        void set(int year, int month, int day);

        static string today();
        static string getTimestamp();
        static bool isDateValid(const string & date);
        
        string shortDate() const;
    
        int year();
        int month();
        int day();

        void addYears(int years);
        void addMonths(int months);
        void addWeeks(int weeks);
        void addDays(int days);

        StrDate & operator=(const StrDate & rhs);
        StrDate & operator=(const string & rhs);

        bool operator==(StrDate & rh);
        bool operator==(string & rh);

        bool operator<(StrDate & rh);
        bool operator<(string & rh);

        bool operator<=(StrDate & rh);
        bool operator<=(string & rh);

        bool operator>(StrDate & rh);
        bool operator>(string & rh);

        bool operator>=(StrDate & rh);
        bool operator>=(string & rh);
};

#endif

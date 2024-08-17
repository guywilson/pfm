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

    public:
        StrDate();
        StrDate(StrDate & sd);
        StrDate(const string & sd);
        StrDate(const char * pszDate);
        StrDate(int year, int month, int day);

        static string today();
        static string getTimestamp();
        static bool validateDate(const string & date);
        string shortDate() const;

        void set(const string & date);
        void set(const char * date);
        void set(int year, int month, int day);

        /*
        ** Calculates the number of seconds since Jan 1, 1970
        */
        time_t epoch();
        static time_t epoch(string & date);

        static bool isLeapYear(int year);
        static bool isLeapYear(string & date);

        static int daysInMonth(int year, int month);
        static int daysInMonth(string & date);
    
        int year();
        int month();
        int day();

        static int year(string & date);
        static int month(string & date);
        static int day(string & date);

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

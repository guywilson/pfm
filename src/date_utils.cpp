#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>

#include "pfm_error.h"

using namespace std;

#define TIME_STAMP_BUFFER_LEN               12


static const char * months[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};


char * getToday(void) {
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

    return today;
}

char * formatPrintDate(char * pszDate) {
    char *          pszFormattedDate;
    int                 day;
    int                 month;
    int                 year;

    pszFormattedDate = (char *)malloc(TIME_STAMP_BUFFER_LEN);

    if (pszFormattedDate != NULL) {
        pszDate[4] = 0;
        pszDate[7] = 0;

        year = atoi(pszDate);
        month = atoi(&pszDate[5]);
        day = atoi(&pszDate[8]);

        pszDate[4] = '-';
        pszDate[7] = '-';

        snprintf(
            pszFormattedDate, 
            TIME_STAMP_BUFFER_LEN, 
            "%d-%s-%02d", 
            year, 
            months[month], 
            day);
    }

    return pszFormattedDate;
}

bool validateDate(char * pszDate) {
	struct timeval		tv;
    struct tm *         localTime;
	time_t				t;
    int                 day;
    int                 month;
    int                 year;

	gettimeofday(&tv, NULL);
	t = tv.tv_sec;
	localTime = localtime(&t);

    if (strlen(pszDate) < 10) {
        return false;
    }

    pszDate[4] = 0;
    pszDate[7] = 0;

    year = atoi(pszDate);
    month = atoi(&pszDate[5]);
    day = atoi(&pszDate[8]);

    pszDate[4] = '-';
    pszDate[7] = '-';

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

void getDateFromString(tm * dt, string & s) {
    istringstream ss(s);

    // Parse the date string using std::get_time
    ss >> get_time(dt, "%Y-%m-%d");

    // Check if parsing was successful
    if (ss.fail()) {
        throw pfm_error(
                pfm_error::buildMsg(
                    "Failed to parse date '%s'", 
                    s.c_str()), 
                __FILE__, 
                __LINE__);
    }
}

bool isDateGreaterThan(string & lhDate, string rhDate) {
    tm          lhd;
    tm          rhd;

    getDateFromString(&lhd, lhDate);
    getDateFromString(&rhd, rhDate);

    time_t lht = mktime(&lhd);
    time_t rht = mktime(&rhd);

    return (lht > rht);
}

bool isDateLessThan(string & lhDate, string rhDate) {
    tm          lhd;
    tm          rhd;

    getDateFromString(&lhd, lhDate);
    getDateFromString(&rhd, rhDate);

    time_t lht = mktime(&lhd);
    time_t rht = mktime(&rhd);

    return (lht < rht);
}

uint32_t getYears(string date) {

}

string addYears(string & startDate, uint32_t years) {

}

string addMonths(string & startDate, uint32_t months) {

}

string addWeeks(string & startDate, uint32_t weeks) {

}

string addDays(string & startDate, uint32_t days) {

}

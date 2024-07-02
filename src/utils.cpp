#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

#include <readline/readline.h>
#include <readline/history.h>

using namespace std;

#define TIME_STAMP_BUFFER_LEN               12

static const char * months[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

string fixStrWidth(string & src, int requiredLen) {
    string          target;

    if (src.length() > requiredLen) {
        target = src.substr(0, requiredLen - 2);
        target.append("..");
    }
    else if (src.length() < requiredLen) {
        target = src;
        
        for (int i = 0;i < requiredLen - src.length();i++) {
            target.append(" ");
        }
    }
    else {
        target = src;
    }

    return target;
}

string formatCurrency(double src) {
    static char szAmount[16];

    snprintf(szAmount, 15, "£%.2f", (float)src);

    return string(szAmount);
}

char * readString(const char * pszPrompt, const char * pszDefault, const size_t maxLength) {
    char *      pszAnswer;

    pszAnswer = readline(pszPrompt);

    if (strlen(pszAnswer) == 0 && pszDefault != NULL) {
        pszAnswer = strndup(pszDefault, maxLength);
    }

    return pszAnswer;
}

char readChar(const char * pszPrompt) {
    char        answer;

    printf("%s", pszPrompt);
    fflush(stdout);

    answer = getchar();
    fflush(stdin);

    return answer;
}

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

bool isDateValid(char * pszDate) {
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

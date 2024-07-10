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

#include <readline/readline.h>
#include <readline/history.h>

#include "pfm_error.h"

using namespace std;

#define CURRENCY_BUFFER_LEN                 16

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
    static char szAmount[CURRENCY_BUFFER_LEN];

    snprintf(szAmount, CURRENCY_BUFFER_LEN, "£%.2f", (float)src);

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

bool validatePaymentFrequency(char * pszFrequency) {
    uint32_t            frequencyLen;

    frequencyLen = strlen(pszFrequency);

    if (frequencyLen < 2) {
        return false;
    }
    if (!isdigit(pszFrequency[0])) {
        return false;
    }
    if (pszFrequency[frequencyLen - 1] != 'w' && 
        pszFrequency[frequencyLen - 1] != 'm' && 
        pszFrequency[frequencyLen - 1] != 'y' &&
        pszFrequency[frequencyLen - 1] != 'd')
    {
        return false;
    }

    return true;
}

int getFrequencyValue(string & frequency) {
    return atoi(frequency.substr(0, frequency.length() - 1).c_str());
}

char getFrequencyUnit(string & frequency) {
    return frequency.substr(frequency.length() - 1, 1).c_str()[0];
}

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <gmp.h>
#include <mpfr.h>
#include <readline/readline.h>
#include <readline/history.h>

using namespace std;

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

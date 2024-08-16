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

#include "db_criteria.h"
#include "pfm_error.h"

using namespace std;

#define MAX_PROMPT_LENGTH                   64

int readCriteria(
        const char * pszField, 
        const db_column_type & type, 
        bool isFirst, 
        const db_operator acceptedOperators[], 
        int numCriteria, 
        DBCriteria * pCriteria)
{
    char *      pszAnswer;
    char        szPrompt[MAX_PROMPT_LENGTH];
    char        szAcceptedOperators[16];
    db_operator op;
    int         i = 0;
    int         j = 0;
    bool        isValidOperator = false;

    strncpy(szPrompt, (isFirst ? "WHERE " : "AND "), MAX_PROMPT_LENGTH);
    strncat(szPrompt, pszField, MAX_PROMPT_LENGTH);
    strncat(szPrompt, ": ", MAX_PROMPT_LENGTH);

    memset(szAcceptedOperators, 0, 16);

    pszAnswer = readline(szPrompt);

    if (strlen(pszAnswer) == 0) {
        return 0;
    }

    op = DBCriteria::getOperator(pszAnswer);

    for (i = 0;i < numCriteria;i++) {
        db_operator ao = acceptedOperators[i];

        switch (ao) {
            case db_operator::less_than:
                strcpy(&szAcceptedOperators[j++], "<");
                break;

            case db_operator::less_than_or_equal_to:
                strcpy(&szAcceptedOperators[j], "<=");
                j += 2;
                break;

            case db_operator::greater_than:
                strcpy(&szAcceptedOperators[j++], ">");
                break;

            case db_operator::greater_than_or_equal_to:
                strcpy(&szAcceptedOperators[j], ">=");
                j += 2;
                break;

            case db_operator::equals:
                strcpy(&szAcceptedOperators[j++], "=");
                break;

            case db_operator::not_equals:
                strcpy(&szAcceptedOperators[j], "!=");
                j += 2;
                break;

            case db_operator::like:
                strcpy(&szAcceptedOperators[j++], "~");
                break;

            case db_operator::unknown:
                break;
        }

        if (op == ao) {
            isValidOperator = true;
        }
        else if (op == db_operator::unknown) {
            isValidOperator = false;
            break;
        }
    }
    
    if (!isValidOperator) {
        throw pfm_validation_error(
                pfm_error::buildMsg(
                    "Invalid operator specified, accepts '%s'", 
                    szAcceptedOperators));
    }

    pCriteria->operation = op;
    pCriteria->value = DBCriteria::getValue(pszAnswer);
    pCriteria->columnName = pszField;
    pCriteria->columnType = type;

    free(pszAnswer);

    return 1;
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

bool validateCreditDebit(const char * pszCD) {
    if (strlen(pszCD) != 2) {
        return false;
    }
    else if (strncmp(pszCD, "CR", 2) != 0 && strncmp(pszCD, "DB", 2) != 0) {
        return false;
    }
    else {
        return true;
    }
}

bool strtobool(const char * yes_no) {
    if (yes_no[0] == 'y' || yes_no[0] == 'Y') {
        return true;
    }
    else {
        return false;
    }
}

bool decodeCreditDebit(const char * credit_debit) {
    if (strncmp(credit_debit, "CR", 2) == 0) {
        return true;
    }
    else if (strncmp(credit_debit, "DB", 2) == 0) {
        return false;
    }
    else {
        return false;
    }
}

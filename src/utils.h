#include <string>

#include "db_criteria.h"

using namespace std;

#ifndef __INCL_UTILS
#define __INCL_UTILS

int         readCriteria(
                const char * pszField, 
                const db_column_type & type, 
                bool isFirst, 
                const db_operator acceptedOperators[], 
                int numCriteria, 
                DBCriteria * pCriteria);
bool        validateDate(char * pszDate);
char *      formatPrintDate(char * pszDate);
bool        validatePaymentFrequency(char * pszFrequency);
void        getDateFromString(tm * dt, string & s);
bool        isDateGreaterThan(string & lhDate, string rhDate);
bool        isDateLessThan(string & lhDate, string rhDate);
bool        strtobool(const char * yes_no);
bool        decodeCreditDebit(const char * credit_debit);
bool        validateCreditDebit(const char * pszCD);

#endif

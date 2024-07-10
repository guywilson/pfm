#include <string>

using namespace std;

#ifndef __INCL_UTILS
#define __INCL_UTILS

string      fixStrWidth(string & src, int requiredLen);
string      formatCurrency(double src);
char *      readString(const char * pszPrompt, const char * pszDefault, const size_t maxLength);
char        readChar(const char * pszPrompt);
char *      getToday(void);
bool        validateDate(char * pszDate);
char *      formatPrintDate(char * pszDate);
bool        validatePaymentFrequency(char * pszFrequency);
void        getDateFromString(tm * dt, string & s);
bool        isDateGreaterThan(string & lhDate, string rhDate);
bool        isDateLessThan(string & lhDate, string rhDate);
int         getFrequencyValue(string & frequency);
char        getFrequencyUnit(string & frequency);

#endif

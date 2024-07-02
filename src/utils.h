#include <string>

using namespace std;

#ifndef __INCL_UTILS
#define __INCL_UTILS

string  fixStrWidth(string & src, int requiredLen);
string  formatCurrency(double src);
char *  readString(const char * pszPrompt, const char * pszDefault, const size_t maxLength);
char    readChar(const char * pszPrompt);
char *  getToday(void);
bool    isDateValid(char * pszDate);
char *  formatPrintDate(char * pszDate);

#endif

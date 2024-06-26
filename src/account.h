#include <string>
#include <vector>

#include <sqlite3.h>

using namespace std;

#ifndef __INCL_ACCOUNT
#define __INCL_ACCOUNT

class Account {
    public:
        sqlite3_int64           id;

        string                  name;
        string                  code;
        double                  openingBalance;
        double                  currentBalance;
};

class AccountResult {
    public:
        AccountResult() {
            numRows = 0;
        }
        
        int                     numRows;

        vector<Account>         results;
};

#endif

#include <string>
#include <stdint.h>

#include <sqlite3.h>

#include "account.h"

using namespace std;

#ifndef __INCL_DB
#define __INCL_DB

class AccountDB {
    public:
        static AccountDB & getInstance() {
            static AccountDB instance;
            return instance;
        }

    private:
        AccountDB() {}

        sqlite3 *       dbHandle;

        void            createSchema();

    public:
        ~AccountDB();

        bool            open(string dbName);

        sqlite3_int64   createAccount(string name, string code, double openingBalance);
        int             getAccounts(AccountResult * result);
        int             getAccount(string code, Account * result);

};

#endif

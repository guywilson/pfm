#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db_new.h"
#include "strdate.h"

using namespace std;

#ifndef __INCL_ACCOUNT
#define __INCL_ACCOUNT

class DBAccountResult;

class DBAccount : public DBBase {
    private:
        PFM_DB_NEW & db = PFM_DB_NEW::getInstance();

        const char * sqlSelectByID = "SELECT id, name, code, opening_balance, current_balance, created, updated FROM account WHERE id = %lld;";
        const char * sqlSelectByCode = "SELECT id, name, code, opening_balance, current_balance, created, updated FROM account WHERE code = '%s';";
        const char * sqlSelectAll = "SELECT id, name, code, opening_balance, current_balance FROM account;";
        const char * sqlInsert = "INSERT INTO account (name, code, opening_balance, current_balance, created, updated) VALUES ('%s', '%s', %.2f, %.2f, '%s', '%s');";
        const char * sqlUpdate = "UPDATE account SET code = '%s', name = '%s', opening_balance = %.2f, current_balance = %.2f, updated = '%s' WHERE id = %lld;";
        const char * sqlDelete = "DELETE FROM account WHERE id = %lld;";

        sqlite3_int64 insert(void);
        void update(void);

    public:
        string                  name;
        string                  code;
        double                  openingBalance;
        double                  currentBalance;

        DBAccount();

        void clear(void);
        void set(const DBAccount & src);
        void print(void);

        void save(void);
        DBAccount retrieveByID(sqlite3_int64 id);
        DBAccount retrieveByCode(string & code);
        DBAccountResult retrieveAll(void);
        void remove(void);
};

class DBAccountResult : public DBResult {
    private:
        vector<DBAccount>       results;

    public:
        DBAccountResult() : DBResult() {}

        void clear(void);
        DBAccount getAccountAt(int i);
        void processRow(DBRow & row) override;
};

#endif

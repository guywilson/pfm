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

class DBAccount : public DBEntity {
    private:
        const char * sqlSelectByID = 
                        "SELECT id, " \
                        "name," \
                        "code," \
                        "opening_balance," \
                        "current_balance," \
                        "created," \
                        "updated " \
                        "FROM account " \
                        "WHERE id = %lld;";

        const char * sqlSelectByCode = 
                        "SELECT id," \
                        "name," \
                        "code," \
                        "opening_balance," \
                        "current_balance," \
                        "created," \
                        "updated " \
                        "FROM account " \
                        "WHERE code = '%s';";

        const char * sqlSelectAll = 
                        "SELECT id," \
                        "name," \
                        "code," \
                        "opening_balance," \
                        "current_balance " \
                        "FROM account;";

        const char * sqlInsert = 
                        "INSERT INTO account (" \
                        "name," \
                        "code," \
                        "opening_balance," \
                        "current_balance," \
                        "created," \
                        "updated) " \
                        "VALUES ('%s', '%s', %.2f, %.2f, '%s', '%s');";

        const char * sqlUpdate = 
                        "UPDATE account SET " \
                        "code = '%s'," \
                        "name = '%s'," \
                        "opening_balance = %.2f," \
                        "current_balance = %.2f," \
                        "updated = '%s' " \
                        "WHERE id = %lld;";

        const char * sqlDelete = 
                        "DELETE FROM account WHERE id = %lld;";

    public:
        string                  name;
        string                  code;
        double                  openingBalance;
        double                  currentBalance;

        DBAccount();

        const char * getInsertStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::now();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlInsert,
                name.c_str(),
                code.c_str(),
                openingBalance,
                currentBalance,
                now.c_str(),
                now.c_str());

            return szStatement;
        }

        const char * getUpdateStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::now();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlUpdate,
                code.c_str(),
                name.c_str(),
                openingBalance,
                currentBalance,
                now.c_str(),
                id);

            return szStatement;
        }

        const char * getDeleteStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::now();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlDelete,
                id);

            return szStatement;
        }

        void clear(void);
        void set(const DBAccount & src);
        void print(void);

        void            retrieveByID(sqlite3_int64 id);
        void            retrieveByCode(string & code);
        DBAccountResult retrieveAll(void);
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

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db.h"
#include "strdate.h"
#include "money.h"
#include "cfgmgr.h"

using namespace std;

#ifndef __INCL_ACCOUNT
#define __INCL_ACCOUNT

class DBAccount : public DBEntity {
    private:
        const char * sqlSelectByCode = 
                        "SELECT id," \
                        "name," \
                        "code," \
                        "opening_date," \
                        "opening_balance," \
                        "created," \
                        "updated " \
                        "FROM account " \
                        "WHERE code = '%s';";

        const char * sqlInsert = 
                        "INSERT INTO account (" \
                        "name," \
                        "code," \
                        "opening_date," \
                        "opening_balance," \
                        "created," \
                        "updated) " \
                        "VALUES ('%s', '%s', '%s', '%s', '%s', '%s');";

        const char * sqlUpdate = 
                        "UPDATE account SET " \
                        "name = '%s'," \
                        "code = '%s'," \
                        "opening_date = '%s'," \
                        "opening_balance = '%s'," \
                        "updated = '%s' " \
                        "WHERE id = %lld;";

        void createRecurringTransactions();
        void createCarriedOverLogs();

    public:
        string                  name;
        string                  code;
        StrDate                 openingDate;
        Money                   openingBalance;

        DBAccount() : DBEntity() {
            clear();
        }

        void clear() {
            DBEntity::clear();

            this->name = "";
            this->code = "";
            this->openingDate.clear();
            this->openingBalance = 0.0;
        }

        void set(const DBAccount & src) {
            DBEntity::set(src);

            this->name =            src.name;
            this->code =            src.code;
            this->openingDate =     src.openingDate;
            this->openingBalance =  src.openingBalance;
        }

        void set(JRecord & record) {
            this->code = record.get("code");
            this->name = record.get("name");
            this->openingDate = record.get("openingDate");
            this->openingBalance = record.get("openingBalance");
        }

        JRecord getRecord() override  {
            JRecord r;

            r.add("name", name);
            r.add("code", code);
            r.add("openingDate", openingDate.shortDate());
            r.add("openingBalance", openingBalance.rawStringValue());

            return r;
        }

        void onRowComplete(int sequence) override {
            doBalancePrerequisites();
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == "name") {
                name = column.getValue();
            }
            else if (column.getName() == "code") {
                code = column.getValue();
            }
            else if (column.getName() == "opening_date") {
                openingDate = column.getValue();
            }
            else if (column.getName() == "opening_balance") {
                openingBalance = column.doubleValue();
            }
        }

        void print() {
            DBEntity::print();

            cout << "Name: '" << name << "'" << endl;
            cout << "Code: '" << code << "'" << endl;
            cout << "Opening date: '" <<  openingDate.shortDate() << "'" << endl;

            cout << fixed << setprecision(2);
            cout << "Opening balance: " << openingBalance.localeFormattedStringValue() << endl;
        }

        const char * getTableName() override {
            return "account";
        }

        const char * getClassName() override {
            return "DBAccount";
        }

        const char * getInsertStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlInsert,
                name.c_str(),
                code.c_str(),
                openingDate.shortDate().c_str(),
                openingBalance.rawStringValue().c_str(),
                now.c_str(),
                now.c_str());

            return szStatement;
        }

        const char * getUpdateStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlUpdate,
                name.c_str(),
                code.c_str(),
                openingDate.shortDate().c_str(),
                openingBalance.rawStringValue().c_str(),
                now.c_str(),
                id);

            return szStatement;
        }

        void doBalancePrerequisites();
        
        void beforeUpdate() override;
        Money calculateCurrentBalance();
        Money calculateBalanceAfterBills();
        Money calculateReconciledBalance();
        void retrieveByCode(string & code);

        bool isPrimary();
};

#endif

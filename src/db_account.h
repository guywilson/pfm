#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db.h"
#include "strdate.h"
#include "money.h"

using namespace std;

#ifndef __INCL_ACCOUNT
#define __INCL_ACCOUNT

class DBAccount : public DBEntity {
    private:
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

        const char * sqlInsert = 
                        "INSERT INTO account (" \
                        "name," \
                        "code," \
                        "opening_balance," \
                        "current_balance," \
                        "created," \
                        "updated) " \
                        "VALUES ('%s', '%s', %s, %s, '%s', '%s');";

        const char * sqlUpdate = 
                        "UPDATE account SET " \
                        "name = '%s'," \
                        "code = '%s'," \
                        "opening_balance = %s," \
                        "current_balance = %s," \
                        "updated = '%s' " \
                        "WHERE id = %lld;";

        const char * sqlDelete = 
                        "DELETE FROM account WHERE id = %lld;";

        void createRecurringTransactions();
        void createCarriedOverLogs();

        string encryptName() {
            return DBColumn::encrypt(name);
        }

        void decryptName(const string & name) {
            this->name = DBColumn::decrypt(name);
        }

        string encryptCode() {
            return DBColumn::encrypt(code);
        }

        void decryptCode(const string & code) {
            this->code = DBColumn::decrypt(code);
        }

        string encryptOpeningBalance() {
            return DBColumn::encrypt(openingBalance.getRawStringValue());
        }

        void decryptOpeningBalance(const string & openingBalance) {
            this->openingBalance = strtod(DBColumn::decrypt(openingBalance).c_str(), NULL);
        }

        string encryptCurrentBalance() {
            return DBColumn::encrypt(currentBalance.getRawStringValue());
        }

        void decryptCurrentBalance(const string & currentBalance) {
            this->currentBalance = strtod(DBColumn::decrypt(currentBalance).c_str(), NULL);
        }

    public:
        string                  name;
        string                  code;
        Money                   openingBalance;
        Money                   currentBalance;

        DBAccount() : DBEntity() {
            clear();
        }

        void clear() {
            DBEntity::clear();

            this->name = "";
            this->code = "";
            this->openingBalance = 0.0;
            this->currentBalance = 0.0;
        }

        void set(const DBAccount & src) {
            DBEntity::set(src);

            this->name =            src.name;
            this->code =            src.code;
            this->openingBalance =  src.openingBalance;
            this->currentBalance =  src.currentBalance;
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == "name") {
                decryptName(column.getValue());
            }
            else if (column.getName() == "code") {
                decryptCode(column.getValue());
            }
            else if (column.getName() == "opening_balance") {
                decryptOpeningBalance(column.getValue());
            }
            else if (column.getName() == "current_balance") {
                decryptCurrentBalance(column.getValue());
            }
        }

        void print() {
            DBEntity::print();

            cout << "Name: '" << name << "'" << endl;
            cout << "Code: '" << code << "'" << endl;

            cout << fixed << setprecision(2);
            cout << "Opening balance: " << openingBalance.getFormattedStringValue() << endl;
            cout << "Current balance: " << currentBalance.getFormattedStringValue() << endl;
        }

        const char * getTableName() override {
            return "account";
        }

        const char * getInsertStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlInsert,
                encryptName().c_str(),
                encryptCode().c_str(),
                encryptOpeningBalance().c_str(),
                encryptCurrentBalance().c_str(),
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
                encryptName().c_str(),
                encryptCode().c_str(),
                encryptOpeningBalance().c_str(),
                encryptCurrentBalance().c_str(),
                now.c_str(),
                id);

            return szStatement;
        }

        const char * getDeleteStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlDelete,
                id);

            return szStatement;
        }

        void onUseAccountTrigger() {
            createRecurringTransactions();
            createCarriedOverLogs();
        }

        Money calculateBalanceAfterBills();

        void retrieveByCode(string & code);
};

#endif

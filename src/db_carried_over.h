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

#ifndef __INCL_CARRIED_OVER
#define __INCL_CARRIED_OVER

class DBCarriedOver : public DBEntity {
    protected:
        struct Columns {
            static constexpr const char * accountId = "account_id";
            static constexpr const char * date = "date";
            static constexpr const char * description = "description";
            static constexpr const char * balance = "balance";
        };

    public:
        pfm_id_t accountId;
        StrDate date;
        string description;
        Money balance;

        DBCarriedOver() : DBEntity() {
            clear();
        }

        DBCarriedOver(const DBCarriedOver & src) : DBEntity(src) {
            set(src);
        }

        void clear() {
            DBEntity::clear();

            this->accountId.clear();
            this->date.clear();
            this->description = "";
            this->balance = 0.0;
        }

        void set(const DBCarriedOver & src) {
            DBEntity::set(src);

            this->accountId = src.accountId;
            this->date = src.date;
            this->description = src.description;
            this->balance =  src.balance;
        }

        void print() {
            DBEntity::print();

            cout << "AccountId: " << accountId.getValue() << endl;
            cout << "Date: '" << date.shortDate() << "'" << endl;
            cout << "Description: '" << description << "'" << endl;

            cout << fixed << setprecision(2);
            cout << "Balance: " << balance.localeFormattedStringValue() << endl;
        }

        const char * getTableName() override {
            return "carried_over_log";
        }

        const char * getClassName() override {
            return "DBCarriedOver";
        }

        const string getInsertStatement() override {
            vector<pair<string, string>> columnValuePairs = {
                {Columns::accountId, accountId.getValue()},
                {Columns::date, date.shortDate()},
                {Columns::description, delimitSingleQuotes(description)},
                {Columns::balance, balance.rawStringValue()}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const string getUpdateStatement() override {
            vector<pair<string, string>> columnValuePairs = {
                {Columns::accountId, accountId.getValue()},
                {Columns::date, date.shortDate()},
                {Columns::description, delimitSingleQuotes(description)},
                {Columns::balance, balance.rawStringValue()}
            };

            return buildUpdateStatement(getTableName(), columnValuePairs);
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == Columns::accountId) {
                accountId = column.getIDValue();
            }
            else if (column.getName() == Columns::date) {
                date = column.getValue();
            }
            else if (column.getName() == Columns::description) {
                description = column.getValue();
            }
            else if (column.getName() == Columns::balance) {
                balance = column.doubleValue();
            }
        }

        int retrieveLatestByAccountId(pfm_id_t & accountId);
        DBResult<DBCarriedOver> retrieveByAccountId(pfm_id_t & accountId);
        DBResult<DBCarriedOver> retrieveByAccountIdAfterDate(pfm_id_t & accountId, StrDate & after);

        static DBCarriedOver createForPeriod(pfm_id_t & accountId, Money & startingBalance, StrDate & startDate, StrDate & endDate);
};

#endif

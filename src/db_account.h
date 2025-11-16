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
        void createRecurringTransactions();
        void createCarriedOverLogs();

    protected:
        struct Columns {
            static constexpr const char * name = "name";
            static constexpr ColumnType name_type = ColumnType::TEXT;

            static constexpr const char * code = "code";
            static constexpr ColumnType code_type = ColumnType::TEXT;
            
            static constexpr const char * openingDate = "opening_date";
            static constexpr ColumnType openingDate_type = ColumnType::DATE;
            
            static constexpr const char * openingBalance = "opening_balance";
            static constexpr ColumnType openingBalance_type = ColumnType::MONEY;
        };

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

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == Columns::name) {
                name = column.getValue();
            }
            else if (column.getName() == Columns::code) {
                code = column.getValue();
            }
            else if (column.getName() == Columns::openingDate) {
                openingDate = column.getValue();
            }
            else if (column.getName() == Columns::openingBalance) {
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

        const string getTableName() const override {
            return "account";
        }

        const string getClassName() const override {
            return "DBAccount";
        }

        const string getInsertStatement() override {
            vector<pair<ColumnDef, string>> columnValuePairs = {
                {{Columns::code, Columns::code_type}, code},
                {{Columns::name, Columns::name_type}, name},
                {{Columns::openingBalance, Columns::openingBalance_type}, openingBalance.rawStringValue()},
                {{Columns::openingDate, Columns::openingDate_type}, openingDate.shortDate()}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const string getUpdateStatement() override {
            vector<pair<ColumnDef, string>> columnValuePairs = {
                {{Columns::code, Columns::code_type}, code},
                {{Columns::name, Columns::name_type}, name},
                {{Columns::openingBalance, Columns::openingBalance_type}, openingBalance.rawStringValue()},
                {{Columns::openingDate, Columns::openingDate_type}, openingDate.shortDate()}
            };

            return buildUpdateStatement(getTableName(), columnValuePairs);
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

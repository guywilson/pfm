#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>

#include <sqlcipher/sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db.h"
#include "strdate.h"
#include "money.h"
#include "cfgmgr.h"


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
            
            static constexpr const char * balanceLimit = "balance_limit";
            static constexpr ColumnType balanceLimit_type = ColumnType::MONEY;
        };

    public:
        std::string name;
        std::string code;

        StrDate openingDate;

        Money openingBalance;
        Money balanceLimit;

        DBAccount() : DBEntity() {
            clear();
        }

        void clear() {
            DBEntity::clear();

            this->name = "";
            this->code = "";
            this->openingDate.clear();
            this->openingBalance = 0.0;
            this->balanceLimit = 0.0;
        }

        void set(const DBAccount & src) {
            DBEntity::set(src);

            this->name =            src.name;
            this->code =            src.code;
            this->openingDate =     src.openingDate;
            this->openingBalance =  src.openingBalance;
            this->balanceLimit =    src.balanceLimit;
        }

        void set(JRecord & record) {
            this->code = record.get("code");
            this->name = record.get("name");
            this->openingDate = record.get("openingDate");
            this->openingBalance = record.get("openingBalance");
            this->balanceLimit = record.get("balanceLimit");
        }

        JRecord getRecord() override  {
            JRecord r;

            r.add("name", name);
            r.add("code", code);
            r.add("openingDate", openingDate.shortDate());
            r.add("openingBalance", openingBalance.rawStringValue());
            r.add("balanceLimit", balanceLimit.rawStringValue());

            return r;
        }

        std::string getIDByCodeSubSelect() {
            std::string idColumnName = DBEntity::Columns::id;
            std::string statement = "(SELECT " + idColumnName + " FROM " + getTableName() + " WHERE " + Columns::code + " = '" + code + "')";
            return statement;
        }

        void backup(std::ofstream & os) override {
            DBResult<DBAccount> results;
            results.retrieveAll();

            os << getDeleteAllStatement() << std::endl;

            for (size_t i = 0;i < results.size();i++) {
                os << results[i].getInsertStatement() << std::endl;
            }

            os.flush();
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
            else if (column.getName() == Columns::balanceLimit) {
                balanceLimit = column.doubleValue();
            }
        }

        void print() {
            DBEntity::print();

            std::cout << "Name: '" << name << "'" << std::endl;
            std::cout << "Code: '" << code << "'" << std::endl;
            std::cout << "Opening date: '" <<  openingDate.shortDate() << "'" << std::endl;

            std::cout << std::fixed << std::setprecision(2);
            std::cout << "Opening balance: " << openingBalance.localeFormattedStringValue() << std::endl;
            std::cout << "Balance limit: " << balanceLimit.localeFormattedStringValue() << std::endl;
        }

        const std::string getTableName() const override {
            return "account";
        }

        const std::string getClassName() const override {
            return "DBAccount";
        }

        const std::string getJSONArrayName() const override {
            return "accounts";
        }

        const std::string getInsertStatement() override {
            std::vector<std::pair<ColumnDef, std::string>> columnValuePairs = {
                {{Columns::code, Columns::code_type}, code},
                {{Columns::name, Columns::name_type}, name},
                {{Columns::openingDate, Columns::openingDate_type}, openingDate.shortDate()},
                {{Columns::openingBalance, Columns::openingBalance_type}, openingBalance.rawStringValue()},
                {{Columns::balanceLimit, Columns::balanceLimit_type}, balanceLimit.rawStringValue()}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const std::string getUpdateStatement() override {
            std::vector<std::pair<ColumnDef, std::string>> columnValuePairs = {
                {{Columns::code, Columns::code_type}, code},
                {{Columns::name, Columns::name_type}, name},
                {{Columns::openingDate, Columns::openingDate_type}, openingDate.shortDate()},
                {{Columns::openingBalance, Columns::openingBalance_type}, openingBalance.rawStringValue()},
                {{Columns::balanceLimit, Columns::balanceLimit_type}, balanceLimit.rawStringValue()}
            };

            return buildUpdateStatement(getTableName(), columnValuePairs);
        }

        void doBalancePrerequisites();

        void beforeUpdate() override;
        
        Money calculateCurrentBalance();
        Money calculateBalanceAfterBills();
        Money calculateReconciledBalance();
        Money calculateRemainingBalance();
        Money calculateRemainingBalance(Money & balanceAfterBills);

        void retrieveByCode(std::string & code);

        bool isPrimary();
};

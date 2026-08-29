#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db.h"
#include "db_account.h"
#include "strdate.h"
#include "money.h"
#include "cfgmgr.h"


class DBCarriedOver : public DBEntity {
    private:
        const std::string getInsertStatementForRestore() {
            DBAccount account;
            account.retrieve(accountId);

            std::string accountSubSelect = account.getIDByCodeSubSelect();

            std::vector<std::pair<ColumnDef, std::string>> columnValuePairs = {
                {{Columns::accountId, Columns::accountId_type}, accountSubSelect},
                {{Columns::date, Columns::date_type}, date.shortDate()},
                {{Columns::description, Columns::description_type}, delimitSingleQuotes(description)},
                {{Columns::balance, Columns::balance_type}, balance.rawStringValue()}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

    protected:
        struct Columns {
            static constexpr const char * accountId = "account_id";
            static constexpr ColumnType accountId_type = ColumnType::ID;

            static constexpr const char * date = "date";
            static constexpr ColumnType date_type = ColumnType::DATE;

            static constexpr const char * description = "description";
            static constexpr ColumnType description_type = ColumnType::TEXT;

            static constexpr const char * balance = "balance";
            static constexpr ColumnType balance_type = ColumnType::MONEY;
        };

    public:
        pfm_id_t accountId;
        StrDate date;
        std::string description;
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

            std::cout << "AccountId: " << accountId.getValue() << std::endl;
            std::cout << "Date: '" << date.shortDate() << "'" << std::endl;
            std::cout << "Description: '" << description << "'" << std::endl;

            std::cout << std::fixed << std::setprecision(2);
            std::cout << "Balance: " << balance.localeFormattedStringValue() << std::endl;
        }

        const std::string getTableName() const override {
            return "carried_over_log";
        }

        const std::string getClassName() const override {
            return "DBCarriedOver";
        }

        const std::string getInsertStatement() override {
            std::vector<std::pair<ColumnDef, std::string>> columnValuePairs = {
                {{Columns::accountId, Columns::accountId_type}, accountId.getValue()},
                {{Columns::date, Columns::date_type}, date.shortDate()},
                {{Columns::description, Columns::description_type}, delimitSingleQuotes(description)},
                {{Columns::balance, Columns::balance_type}, balance.rawStringValue()}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const std::string getUpdateStatement() override {
            std::vector<std::pair<ColumnDef, std::string>> columnValuePairs = {
                {{Columns::accountId, Columns::accountId_type}, accountId.getValue()},
                {{Columns::date, Columns::date_type}, date.shortDate()},
                {{Columns::description, Columns::description_type}, delimitSingleQuotes(description)},
                {{Columns::balance, Columns::balance_type}, balance.rawStringValue()}
            };

            return buildUpdateStatement(getTableName(), columnValuePairs);
        }

        void backup(std::ofstream & os) override {
            DBResult<DBCarriedOver> results;
            results.retrieveAll();

            os << getDeleteAllStatement() << std::endl;

            for (size_t i = 0;i < results.size();i++) {
                os << results[i].getInsertStatementForRestore() << std::endl;
            }

            os.flush();
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


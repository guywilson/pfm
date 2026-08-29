#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db.h"
#include "strdate.h"


class DBPrimaryAccount : public DBEntity {
    protected:
        struct Columns {
            static constexpr const char * code = "account_code";
            static constexpr ColumnType code_type = ColumnType::TEXT;
        };

    public:
        std::string code;

        DBPrimaryAccount() : DBEntity() {
            clear();
        }

        void clear() {
            DBEntity::clear();

            this->code = "";
        }

        void set(const DBPrimaryAccount & src) {
            DBEntity::set(src);

            this->code = src.code;
        }

        void set(JRecord & record) {
            this->code = record.get("code");
        }

        JRecord getRecord() override  {
            JRecord r;

            r.add("code", code);

            return r;
        }

        void backup(std::ofstream & os) override {
            DBResult<DBPrimaryAccount> results;
            results.retrieveAll();

            os << getDeleteAllStatement() << std::endl;

            for (size_t i = 0;i < results.size();i++) {
                os << results[i].getInsertStatement() << std::endl;
            }

            os.flush();
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == Columns::code) {
                code = column.getValue();
            }
        }

        void print() {
            DBEntity::print();

            std::cout << "Code: '" << code << "'" << std::endl;
        }

        const std::string getTableName() const override {
            return "primary_account";
        }

        const std::string getClassName() const override {
            return "DBPrimaryAccount";
        }

        const std::string getInsertStatement() override {
            std::vector<std::pair<ColumnDef, std::string>> columnValuePairs = {
                {{Columns::code, Columns::code_type}, code}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const std::string getUpdateStatement() override {
            std::vector<std::pair<ColumnDef, std::string>> columnValuePairs = {
                {{Columns::code, Columns::code_type}, code}
            };

            return buildUpdateStatement(getTableName(), columnValuePairs);
        }

        static std::string getPrimaryAccountCode();
        static void setPrimaryAccount(std::string & accountCode);
};


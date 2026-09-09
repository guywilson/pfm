/*
 * Copyright (C) 2025-2026 Guy Wilson
 *
 * This file is part of PFM.
 *
 * PFM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PFM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PFM. If not, see <https://www.gnu.org/licenses/>.
 */

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

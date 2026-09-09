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
#include "cfgmgr.h"


class DBTransactionReport : public DBEntity {
    protected:
        struct Columns {
            static constexpr const char * description = "description";
            static constexpr ColumnType description_type = ColumnType::TEXT;

            static constexpr const char * sqlWhereClause = "sql_where_clause";
            static constexpr ColumnType sqlWhereClause_type = ColumnType::TEXT;
        };

    public:
        std::string description;
        std::string sqlWhereClause;

        DBTransactionReport() : DBEntity() {
            clear();
        }

        DBTransactionReport(const DBTransactionReport & src) : DBEntity(src) {
            set(src);
        }

        void clear() {
            DBEntity::clear();

            this->description = "";
            this->sqlWhereClause = "";
        }

        void set(const DBTransactionReport & src) {
            DBEntity::set(src);

            this->description = src.description;
            this->sqlWhereClause =  src.sqlWhereClause;
        }

        void set(JRecord & record) {
            this->description = record.get("description");
            this->sqlWhereClause = record.get("sql");
        }

        JRecord getRecord() override  {
            JRecord r;

            r.add("description", description);
            r.add("sql", sqlWhereClause);

            return r;
        }

        void print() {
            DBEntity::print();

            std::cout << "Description: '" << description << "'" << std::endl;
            std::cout << "SQL: '" << sqlWhereClause << "'" << std::endl;
        }

        const std::string getTableName() const override {
            return "transaction_report";
        }

        const std::string getClassName() const override {
            return "DBTransactionReport";
        }

        const std::string getJSONArrayName() const override {
            return "reports";
        }

        void onRowComplete(int sequence) override {
            this->sequence = sequence;
        }

        const std::string getInsertStatement() override {
            std::vector<std::pair<ColumnDef, std::string>> columnValuePairs = {
                {{Columns::description, Columns::description_type}, delimitSingleQuotes(description)},
                {{Columns::sqlWhereClause, Columns::sqlWhereClause_type}, delimitSingleQuotes(sqlWhereClause)}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const std::string getUpdateStatement() override {
            std::vector<std::pair<ColumnDef, std::string>> columnValuePairs = {
                {{Columns::description, Columns::description_type}, delimitSingleQuotes(description)},
                {{Columns::sqlWhereClause, Columns::sqlWhereClause_type}, delimitSingleQuotes(sqlWhereClause)}
            };

            return buildUpdateStatement(getTableName(), columnValuePairs);
        }

        void backup(std::ofstream & os) override {
            DBResult<DBTransactionReport> results;
            results.retrieveAll();

            os << getDeleteAllStatement() << std::endl;

            for (size_t i = 0;i < results.size();i++) {
                os << results[i].getInsertStatement() << std::endl;
            }

            os.flush();
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == Columns::description) {
                description = column.getValue();
            }
            else if (column.getName() == Columns::sqlWhereClause) {
                sqlWhereClause = column.getValue();
            }
        }
};

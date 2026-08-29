#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "db.h"
#include "db_base.h"
#include "jfile.h"
#include "strdate.h"


class DBCategory : public DBEntity {
    protected:
        struct Columns {
            static constexpr const char * description = "description";
            static constexpr ColumnType description_type = ColumnType::TEXT;

            static constexpr const char * code = "code";
            static constexpr ColumnType code_type = ColumnType::TEXT;
        };

    public:
        std::string description;
        std::string code;

        DBCategory() : DBEntity() {
            clear();
        }

        void clear() {
            DBEntity::clear();

            this->description = "";
            this->code = "";
        }

        void set(const DBCategory & src) {
            DBEntity::set(src);

            this->description = src.description;
            this->code = src.code;
        }

        void set(JRecord & record) {
            this->code = record.get("code");
            this->description = record.get("description");
        }

        JRecord getRecord() override  {
            JRecord r;

            r.add("code", this->code);
            r.add("description", this->description);

            return r;
        }

        void print() {
            DBEntity::print();

            std::cout << "Description: '" << description << "'" << std::endl;
            std::cout << "Code: '" << code << "'" << std::endl;
        }

        std::string getIDByCodeSubSelect() {
            std::string idColumnName = DBEntity::Columns::id;
            std::string statement = "(SELECT " + idColumnName + " FROM " + getTableName() + " WHERE " + Columns::code + " = '" + code + "')";
            return statement;
        }

        void backup(std::ofstream & os) override {
            DBResult<DBCategory> results;
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
            else if (column.getName() == Columns::description) {
                description = column.getValue();
            }
        }

        const std::string getTableName() const override {
            return "category";
        }

        const std::string getClassName() const override {
            return "DBCategory";
        }

        const std::string getJSONArrayName() const override {
            return "categories";
        }

        const std::string getInsertStatement() override {
            std::vector<std::pair<ColumnDef, std::string>> columnValuePairs = {
                {{Columns::description, Columns::description_type}, delimitSingleQuotes(description)},
                {{Columns::code, Columns::code_type}, code}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const std::string getUpdateStatement() override {
            std::vector<std::pair<ColumnDef, std::string>> columnValuePairs = {
                {{Columns::description, Columns::description_type}, delimitSingleQuotes(description)},
                {{Columns::code, Columns::code_type}, code}
            };

            return buildUpdateStatement(getTableName(), columnValuePairs);
        }

        void retrieveByCode(std::string & code);
        DBResult<DBCategory> retrieveOrderedByCode();
};


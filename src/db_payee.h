#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "db.h"
#include "db_base.h"
#include "jfile.h"
#include "strdate.h"


class DBPayee : public DBEntity {
    protected:
        struct Columns {
            static constexpr const char * name = "name";
            static constexpr ColumnType name_type = ColumnType::TEXT;

            static constexpr const char * code = "code";
            static constexpr ColumnType code_type = ColumnType::TEXT;
        };

    public:
        std::string name;
        std::string code;

        DBPayee() : DBEntity() {
            clear();
        }

        void clear() {
            DBEntity::clear();

            this->name = "";
            this->code = "";
        }

        void set(const DBPayee & src) {
            DBEntity::set(src);

            this->name = src.name;
            this->code = src.code;
        }

        void set(JRecord & record) {
            this->code = record.get("code");
            this->name = record.get("name");
        }

        JRecord getRecord() override  {
            JRecord r;

            r.add("code", this->code);
            r.add("name", this->name);

            return r;
        }

        void print() {
            DBEntity::print();

            std::cout << "Description: '" << name << "'" << std::endl;
            std::cout << "Code: '" << code << "'" << std::endl;
        }

        std::string getIDByCodeSubSelect() {
            std::string idColumnName = DBEntity::Columns::id;
            std::string statement = "(SELECT " + idColumnName + " FROM " + getTableName() + " WHERE " + Columns::code + " = '" + code + "')";
            return statement;
        }

        void backup(std::ofstream & os) override {
            DBResult<DBPayee> results;
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
            else if (column.getName() == Columns::name) {
                name = column.getValue();
            }
        }

        const std::string getTableName() const override {
            return "payee";
        }

        const std::string getClassName() const override {
            return "DBPayee";
        }

        const std::string getJSONArrayName() const override {
            return "payees";
        }

        const std::string getInsertStatement() override {
            std::vector<std::pair<ColumnDef, std::string>> columnValuePairs = {
                {{Columns::code, Columns::code_type}, code},
                {{Columns::name, Columns::name_type}, delimitSingleQuotes(name)}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const std::string getUpdateStatement() override {
            std::vector<std::pair<ColumnDef, std::string>> columnValuePairs = {
                {{Columns::code, Columns::code_type}, code},
                {{Columns::name, Columns::name_type}, delimitSingleQuotes(name)}
            };

            return buildUpdateStatement(getTableName(), columnValuePairs);
        }

        void retrieveByCode(std::string & code);
        DBResult<DBPayee> retrieveOrderedByCode();
};


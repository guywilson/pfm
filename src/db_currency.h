#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "db.h"
#include "db_base.h"
#include "strdate.h"


class DBCurrency : public DBEntity {
    protected:
        struct Columns {
            static constexpr const char * code = "code";
            static constexpr ColumnType code_type = ColumnType::TEXT;

            static constexpr const char * name = "name";
            static constexpr ColumnType name_type = ColumnType::TEXT;

            static constexpr const char * symbol = "symbol";
            static constexpr ColumnType symbol_type = ColumnType::TEXT;
        };

    public:
        std::string code;
        std::string name;
        std::string symbol;

        DBCurrency() : DBEntity() {
            clear();
        }

        void clear() {
            DBEntity::clear();

            this->code = "";
            this->name = "";
            this->symbol = "";
        }

        void set(const DBCurrency & src) {
            DBEntity::set(src);

            this->code.assign(src.code);
            this->name.assign(src.name);
            this->symbol.assign(src.symbol);
        }

        void print() {
            DBEntity::print();

            std::cout << "Code: '" << code << "'" << std::endl;
            std::cout << "Name: '" << name << "'" << std::endl;
            std::cout << "Symbol: '" << symbol << "'" << std::endl;
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == Columns::code) {
                code = column.getValue();
            }
            else if (column.getName() == Columns::name) {
                name = column.getValue();
            }
            else if (column.getName() == Columns::symbol) {
                symbol = column.getValue();
            }
        }

        const std::string getTableName() const override {
            return "currency";
        }

        const std::string getClassName() const override {
            return "DBCurrency";
        }

        const std::string getInsertStatement() override {
            std::vector<std::pair<ColumnDef, std::string>> columnValuePairs = {
                {{Columns::code, Columns::code_type}, code},
                {{Columns::name, Columns::name_type}, delimitSingleQuotes(name)},
                {{Columns::symbol, Columns::symbol_type}, symbol}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const std::string getUpdateStatement() override {
            std::vector<std::pair<ColumnDef, std::string>> columnValuePairs = {
                {{Columns::code, Columns::code_type}, code},
                {{Columns::name, Columns::name_type}, delimitSingleQuotes(name)},
                {{Columns::symbol, Columns::symbol_type}, symbol}
            };

            return buildUpdateStatement(getTableName(), columnValuePairs);
        }

        void retrieveByCode(std::string & code);
};


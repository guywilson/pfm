#include <iostream>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "db.h"
#include "db_base.h"
#include "strdate.h"

using namespace std;

#ifndef __INCL_CONFIG
#define __INCL_CONFIG

class DBConfig : public DBEntity {
    protected:
        struct Columns {
            static constexpr const char * key = "key";
            static constexpr const char * value = "value";
            static constexpr const char * description = "description";
            static constexpr const char * isReadOnly = "is_read_only";
            static constexpr const char * isVisible = "is_visible";
        };

    public:
        string key;
        string value;
        string description;
        bool isReadOnly;
        bool isVisible;

        DBConfig() : DBEntity() {
            clear();
        }

        void clear() {
            DBEntity::clear();

            this->key = "";
            this->value = "";
            this->description = "";
            this->isReadOnly = false;
            this->isVisible = true;
        }

        void set(const DBConfig & src) {
            DBEntity::set(src);

            this->key.assign(src.key);
            this->value.assign(src.value);
            this->description.assign(src.description);
            this->isReadOnly = src.isReadOnly;
            this->isVisible = src.isVisible;
        }

        void print() {
            DBEntity::print();

            cout << "Key: '" << key << "'" << endl;
            cout << "Value: '" << value << "'" << endl;
            cout << "Description: '" << description << "'" << endl;
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == Columns::key) {
                key = column.getValue();
            }
            else if (column.getName() == Columns::value) {
                value = column.getValue();
            }
            else if (column.getName() == Columns::description) {
                description = column.getValue();
            }
            else if (column.getName() == Columns::isReadOnly) {
                isReadOnly = column.getBoolValue();
            }
            else if (column.getName() == Columns::isVisible) {
                isVisible = column.getBoolValue();
            }
        }

        const char * getTableName() override {
            return "config";
        }

        const char * getClassName() override {
            return "DBConfig";
        }

        const string getInsertStatement() override {
            vector<pair<string, string>> columnValuePairs = {
                {Columns::key, key},
                {Columns::value, delimitSingleQuotes(value)},
                {Columns::description, delimitSingleQuotes(description)},
                {Columns::isReadOnly, "N"},
                {Columns::isVisible, "Y"}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const string getUpdateStatement() override {
            vector<pair<string, string>> columnValuePairs = {
                {Columns::key, key},
                {Columns::value, delimitSingleQuotes(value)},
                {Columns::description, delimitSingleQuotes(description)},
                {Columns::isReadOnly, "N"},
                {Columns::isVisible, "Y"}
            };

            return buildUpdateStatement(getTableName(), columnValuePairs);
        }

        void retrieveByKey(const string & key);
        DBResult<DBConfig> retrieveAllVisible();
};

#endif

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
            static constexpr ColumnType key_type = ColumnType::TEXT;

            static constexpr const char * value = "value";
            static constexpr ColumnType value_type = ColumnType::TEXT;

            static constexpr const char * description = "description";
            static constexpr ColumnType description_type = ColumnType::TEXT;

            static constexpr const char * isReadOnly = "is_read_only";
            static constexpr ColumnType isReadOnly_type = ColumnType::TEXT;

            static constexpr const char * isVisible = "is_visible";
            static constexpr ColumnType isVisible_type = ColumnType::TEXT;
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

            this->key = src.key;
            this->value = src.value;
            this->description = src.description;
            this->isReadOnly = src.isReadOnly;
            this->isVisible = src.isVisible;
        }

        void set(JRecord & record) {
            this->key = record.get("key");
            this->value = record.get("value");
            this->description = record.get("description");
            this->isReadOnly = record.getBoolValue("isReadOnly");
            this->isVisible = record.getBoolValue("isVisible");
        }

        JRecord getRecord() override  {
            JRecord r;

            r.add("key", key);
            r.add("value", value);
            r.add("description", description);
            r.add("isReadOnly", isReadOnly);
            r.add("isVisible", isVisible);

            return r;
        }

        void print() {
            DBEntity::print();

            cout << "Key: '" << key << "'" << endl;
            cout << "Value: '" << value << "'" << endl;
            cout << "Description: '" << description << "'" << endl;
        }

        void backup(ofstream & os) override {
            DBResult<DBConfig> results;
            results.retrieveAll();

            os << getDeleteAllStatement() << endl;

            for (int i = 0;i < results.size();i++) {
                os << results[i].getInsertStatement() << endl;
            }

            os.flush();
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

        const string getTableName() const override {
            return "config";
        }

        const string getClassName() const override {
            return "DBConfig";
        }

        const string getJSONArrayName() const override {
            return "config";
        }

        const string getInsertStatement() override {
            vector<pair<ColumnDef, string>> columnValuePairs = {
                {{Columns::key, Columns::key_type}, key},
                {{Columns::value, Columns::value_type}, delimitSingleQuotes(value)},
                {{Columns::description, Columns::description_type}, delimitSingleQuotes(description)},
                {{Columns::isReadOnly, Columns::isReadOnly_type}, "N"},
                {{Columns::isVisible, Columns::isVisible_type}, "Y"}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const string getUpdateStatement() override {
            vector<pair<ColumnDef, string>> columnValuePairs = {
                {{Columns::key, Columns::key_type}, key},
                {{Columns::value, Columns::value_type}, delimitSingleQuotes(value)},
                {{Columns::description, Columns::description_type}, delimitSingleQuotes(description)},
                {{Columns::isReadOnly, Columns::isReadOnly_type}, "N"},
                {{Columns::isVisible, Columns::isVisible_type}, "Y"}
            };

            return buildUpdateStatement(getTableName(), columnValuePairs);
        }

        void retrieveByKey(const string & key);
        DBResult<DBConfig> retrieveAllVisible();
};

#endif

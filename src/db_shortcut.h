#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <unordered_map>

#include <sqlcipher/sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db.h"
#include "strdate.h"
#include "cfgmgr.h"

using namespace std;

#ifndef __INCL_SHORTCUT
#define __INCL_SHORTCUT

class DBShortcut : public DBEntity {
    protected:
        struct Columns {
            static constexpr const char * shortcut = "shortcut_text";
            static constexpr ColumnType shortcut_type = ColumnType::TEXT;

            static constexpr const char * replacementText = "replacement_text";
            static constexpr ColumnType replacementText_type = ColumnType::TEXT;
        };

    public:
        string shortcut;
        string replacementText;

        DBShortcut() : DBEntity() {
            clear();
        }

        DBShortcut(const DBShortcut & src) : DBEntity(src) {
            set(src);
        }

        void clear() {
            DBEntity::clear();

            this->shortcut = "";
            this->replacementText = "";
        }

        void set(const DBShortcut & src) {
            DBEntity::set(src);

            this->shortcut = src.shortcut;
            this->replacementText =  src.replacementText;
        }

        void set(JRecord & record) {
            this->shortcut = record.get("shortcut");
            this->replacementText = record.get("replacementText");
        }

        JRecord getRecord() override  {
            JRecord r;

            r.add("shortcut", shortcut);
            r.add("replacementText", replacementText);

            return r;
        }

        void print() {
            DBEntity::print();

            cout << "Shortcut: '" << shortcut << "'" << endl;
            cout << "ReplacementText: '" << replacementText << "'" << endl;
        }

        const string getTableName() const override {
            return "shortcut";
        }

        const string getClassName() const override {
            return "DBShortcut";
        }

        const string getJSONArrayName() const override {
            return "shortcuts";
        }

        void onRowComplete(int sequence) override {
            this->sequence = sequence;
        }

        const string getInsertStatement() override {
            vector<pair<ColumnDef, string>> columnValuePairs = {
                {{Columns::shortcut, Columns::shortcut_type}, shortcut},
                {{Columns::replacementText, Columns::replacementText_type}, delimitSingleQuotes(replacementText)}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const string getUpdateStatement() override {
            vector<pair<ColumnDef, string>> columnValuePairs = {
                {{Columns::shortcut, Columns::shortcut_type}, shortcut},
                {{Columns::replacementText, Columns::replacementText_type}, delimitSingleQuotes(replacementText)}
            };

            return buildUpdateStatement(getTableName(), columnValuePairs);
        }

        void backup(ofstream & os) override {
            DBResult<DBShortcut> results;
            results.retrieveAll();

            os << getDeleteAllStatement() << endl;

            for (size_t i = 0;i < results.size();i++) {
                os << results[i].getInsertStatement() << endl;
            }

            os.flush();
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == Columns::shortcut) {
                shortcut = column.getValue();
            }
            else if (column.getName() == Columns::replacementText) {
                replacementText = column.getValue();
            }
        }

        static vector<pair<string, string>> populate() {
            vector<pair<string, string>> shortcutPairs;

            DBResult<DBShortcut> results;
            results.retrieveAll();

            for (size_t i = 0;i < results.size();i++) {
                DBShortcut shortcut = results[i];

                shortcutPairs.push_back({shortcut.shortcut, shortcut.replacementText});
            }

            return shortcutPairs;
        }
};

#endif

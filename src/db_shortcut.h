#pragma once

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


class DBShortcut : public DBEntity {
    protected:
        struct Columns {
            static constexpr const char * shortcut = "shortcut_text";
            static constexpr ColumnType shortcut_type = ColumnType::TEXT;

            static constexpr const char * replacementText = "replacement_text";
            static constexpr ColumnType replacementText_type = ColumnType::TEXT;
        };

    public:
        std::string shortcut;
        std::string replacementText;

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

            std::cout << "Shortcut: '" << shortcut << "'" << std::endl;
            std::cout << "ReplacementText: '" << replacementText << "'" << std::endl;
        }

        const std::string getTableName() const override {
            return "shortcut";
        }

        const std::string getClassName() const override {
            return "DBShortcut";
        }

        const std::string getJSONArrayName() const override {
            return "shortcuts";
        }

        void onRowComplete(int sequence) override {
            this->sequence = sequence;
        }

        const std::string getInsertStatement() override {
            std::vector<std::pair<ColumnDef, std::string>> columnValuePairs = {
                {{Columns::shortcut, Columns::shortcut_type}, shortcut},
                {{Columns::replacementText, Columns::replacementText_type}, delimitSingleQuotes(replacementText)}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const std::string getUpdateStatement() override {
            std::vector<std::pair<ColumnDef, std::string>> columnValuePairs = {
                {{Columns::shortcut, Columns::shortcut_type}, shortcut},
                {{Columns::replacementText, Columns::replacementText_type}, delimitSingleQuotes(replacementText)}
            };

            return buildUpdateStatement(getTableName(), columnValuePairs);
        }

        void backup(std::ofstream & os) override {
            DBResult<DBShortcut> results;
            results.retrieveAll();

            os << getDeleteAllStatement() << std::endl;

            for (size_t i = 0;i < results.size();i++) {
                os << results[i].getInsertStatement() << std::endl;
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

        static std::vector<std::pair<std::string, std::string>> populate() {
            std::vector<std::pair<std::string, std::string>> shortcutPairs;

            DBResult<DBShortcut> results;
            results.retrieveAll();

            for (size_t i = 0;i < results.size();i++) {
                DBShortcut shortcut = results[i];

                shortcutPairs.push_back({shortcut.shortcut, shortcut.replacementText});
            }

            return shortcutPairs;
        }
};


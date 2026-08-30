#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdint.h>

#include <sqlcipher/sqlite3.h>

#include "db.h"
#include "db_base.h"
#include "strdate.h"
#include "jfile.h"
#include "money.h"

class DBTempCSV : public DBEntity {
    protected:
        struct Columns {
            static constexpr const char * accountCode = "account_code";
            static constexpr ColumnType accountCode_type = ColumnType::TEXT;

            static constexpr const char * date = "date";
            static constexpr ColumnType date_type = ColumnType::DATE;

            static constexpr const char * reference = "reference";
            static constexpr ColumnType reference_type = ColumnType::TEXT;

            static constexpr const char * description = "description";
            static constexpr ColumnType description_type = ColumnType::TEXT;

            static constexpr const char * type = "type";
            static constexpr ColumnType type_type = ColumnType::TEXT;

            static constexpr const char * amount = "amount";
            static constexpr ColumnType amount_type = ColumnType::MONEY;
        };

    public:
        std::string accountCode;
        StrDate date;
        std::string reference;
        std::string description;
        std::string type;
        Money amount;

        DBTempCSV() : DBEntity() {
            clear();
        }

        DBTempCSV(const DBTempCSV & src) : DBEntity(src) {
            set(src);
        }

        void clear() {
            DBEntity::clear();

            this->accountCode.clear();
            this->date.clear();
            this->reference.clear();
            this->description.clear();
            this->type.clear();
            this->amount = 0.0;
        }

        void set(const DBTempCSV & src) {
            DBEntity::set(src);

            this->accountCode = src.accountCode;
            this->date = src.date;
            this->reference = src.reference;
            this->description = src.description;
            this->type = src.type;
            this->amount = src.amount;
        }

        void set(JRecord & record) {
            this->accountCode = record.get("accountCode");
            this->date = record.get("date");
            this->reference = record.get("reference");
            this->description = record.get("description");
            this->type = record.get("type");
            this->amount = record.get("amount");
        }

        JRecord getRecord() override {
            JRecord r;

            r.add("accountCode", this->accountCode);
            r.add("date", this->date.shortDate());
            r.add("reference", this->reference);
            r.add("description", this->description);
            r.add("type", this->type);
            r.add("amount", this->amount.rawStringValue());

            return r;
        }

        void print() {
            DBEntity::print();

            std::cout << "Account code: '" << accountCode << "'" << std::endl;
            std::cout << "Date: '" << date.shortDate() << "'" << std::endl;
            std::cout << "Reference: '" << reference << "'" << std::endl;
            std::cout << "Description: '" << description << "'" << std::endl;
            std::cout << "Type: '" << type << "'" << std::endl;

            std::cout << std::fixed << std::setprecision(2);
            std::cout << "Amount: " << amount.localeFormattedStringValue() << std::endl;
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == Columns::accountCode) {
                accountCode = column.getValue();
            }
            else if (column.getName() == Columns::date) {
                date = column.getValue();
            }
            else if (column.getName() == Columns::reference) {
                reference = column.getValue();
            }
            else if (column.getName() == Columns::description) {
                description = column.getValue();
            }
            else if (column.getName() == Columns::type) {
                type = column.getValue();
            }
            else if (column.getName() == Columns::amount) {
                amount = column.doubleValue();
            }
        }

        const std::string getTableName() const override {
            return "csv_temp_transaction";
        }

        const std::string getClassName() const override {
            return "DBTempCSV";
        }

        const std::string getJSONArrayName() const override {
            return "transactions";
        }

        const std::string getInsertStatement() override {
            std::vector<std::pair<ColumnDef, std::string>> columnValuePairs = {
                {{Columns::accountCode, Columns::accountCode_type}, accountCode},
                {{Columns::date, Columns::date_type}, date.shortDate()},
                {{Columns::reference, Columns::reference_type}, delimitSingleQuotes(reference)},
                {{Columns::description, Columns::description_type}, delimitSingleQuotes(description)},
                {{Columns::type, Columns::type_type}, type},
                {{Columns::amount, Columns::amount_type}, amount.rawStringValue()}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const std::string getUpdateStatement() override {
            std::vector<std::pair<ColumnDef, std::string>> columnValuePairs = {
                {{Columns::accountCode, Columns::accountCode_type}, accountCode},
                {{Columns::date, Columns::date_type}, date.shortDate()},
                {{Columns::reference, Columns::reference_type}, delimitSingleQuotes(reference)},
                {{Columns::description, Columns::description_type}, delimitSingleQuotes(description)},
                {{Columns::type, Columns::type_type}, type},
                {{Columns::amount, Columns::amount_type}, amount.rawStringValue()}
            };

            return buildUpdateStatement(getTableName(), columnValuePairs);
        }
};

#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "db.h"
#include "db_base.h"
#include "jfile.h"
#include "strdate.h"


class DBPublicHoliday : public DBEntity {
    protected:
        struct Columns {
            static constexpr const char * date = "date";
            static constexpr ColumnType date_type = ColumnType::DATE;

            static constexpr const char * description = "description";
            static constexpr ColumnType description_type = ColumnType::TEXT;
        };

    public:
        StrDate date;
        std::string description;

        DBPublicHoliday() : DBEntity() {
            clear();
        }

        void clear() {
            DBEntity::clear();

            this->date.clear();
            this->description = "";
        }

        void set(const DBPublicHoliday & src) {
            DBEntity::set(src);

            this->date = src.date;
            this->description = src.description;
        }

        void set(JRecord & record) {
            this->date = record.get("date");
            this->description = record.get("description");
        }

        JRecord getRecord() override  {
            JRecord r;

            r.add("date", this->date.shortDate());
            r.add("description", this->description);

            return r;
        }

        void print() {
            DBEntity::print();

            std::cout << "Date: '" << date.shortDate() << "'" << std::endl;
            std::cout << "Description: '" << description << "'" << std::endl;
        }

        void backup(std::ofstream & os) override {
            DBResult<DBPublicHoliday> results;
            results.retrieveAll();

            os << getDeleteAllStatement() << std::endl;

            for (size_t i = 0;i < results.size();i++) {
                os << results[i].getInsertStatement() << std::endl;
            }

            os.flush();
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == Columns::date) {
                date = column.getValue();
            }
            else if (column.getName() == Columns::description) {
                description = column.getValue();
            }
        }

        void onRowComplete(int sequence) override {
            this->sequence = sequence;
        }

        const std::string getTableName() const override {
            return "public_holiday";
        }

        const std::string getClassName() const override {
            return "DBPublicHoliday";
        }

        const std::string getJSONArrayName() const override {
            return "holidays";
        }

        const std::string getInsertStatement() override {
            std::vector<std::pair<ColumnDef, std::string>> columnValuePairs = {
                {{Columns::date, Columns::date_type}, date.shortDate()},
                {{Columns::description, Columns::description_type}, delimitSingleQuotes(description)}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const std::string getUpdateStatement() override {
            std::vector<std::pair<ColumnDef, std::string>> columnValuePairs = {
                {{Columns::date, Columns::date_type}, date.shortDate()},
                {{Columns::description, Columns::description_type}, delimitSingleQuotes(description)}
            };

            return buildUpdateStatement(getTableName(), columnValuePairs);
        }

        static void populatePublicHolidays() {
            DBResult<DBPublicHoliday> holidays;
            holidays.retrieveAll();

            for (size_t i = 0;i < holidays.size();i++) {
                DBPublicHoliday holiday = holidays[i];

                std::pair<StrDate, std::string> holidayPair;
                holidayPair.first = holiday.date;
                holidayPair.second = holiday.description;

                addPublicHoliday(holidayPair);
            }
        }
};


#include <iostream>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "db.h"
#include "db_base.h"
#include "jfile.h"
#include "strdate.h"

using namespace std;

#ifndef __INCL_PUBLIC_HOLIDAY
#define __INCL_PUBLIC_HOLIDAY

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
        string description;

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

            cout << "Date: '" << date.shortDate() << "'" << endl;
            cout << "Description: '" << description << "'" << endl;
        }

        void backup(ofstream & os) override {
            DBResult<DBPublicHoliday> results;
            results.retrieveAll();

            os << getDeleteAllStatement() << endl;

            for (size_t i = 0;i < results.size();i++) {
                os << results[i].getInsertStatement() << endl;
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

        const string getTableName() const override {
            return "public_holiday";
        }

        const string getClassName() const override {
            return "DBPublicHoliday";
        }

        const string getJSONArrayName() const override {
            return "holidays";
        }

        const string getInsertStatement() override {
            vector<pair<ColumnDef, string>> columnValuePairs = {
                {{Columns::date, Columns::date_type}, date.shortDate()},
                {{Columns::description, Columns::description_type}, delimitSingleQuotes(description)}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const string getUpdateStatement() override {
            vector<pair<ColumnDef, string>> columnValuePairs = {
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

                pair<StrDate, string> holidayPair;
                holidayPair.first = holiday.date;
                holidayPair.second = holiday.description;

                addPublicHoliday(holidayPair);
            }
        }
};

#endif

#include <string>
#include <string.h>
#include <stdint.h>

#include "pfm_error.h"
#include "db.h"
#include "db_base.h"
#include "strdate.h"
#include "money.h"
#include "jfile.h"
#include "db_category.h"
#include "db_payee.h"

using namespace std;

#ifndef __INCL_BUDGET
#define __INCL_BUDGET

class DBBudget : public DBEntity {
    private:
        const char * sqlSelectByPayeeCode = 
            "SELECT " \
            "id," \
            "start_date," \
            "end_date," \
            "category_code," \
            "payee_code," \
            "description," \
            "min_budget," \
            "max_budget," \
            "created," \
            "updated " \
            "FROM budget " \
            "WHERE payee_code = '%s';";

        const char * sqlSelectByCategoryCode = 
            "SELECT " \
            "id," \
            "start_date," \
            "end_date," \
            "category_code," \
            "payee_code," \
            "description," \
            "min_budget," \
            "max_budget," \
            "created," \
            "updated " \
            "FROM budget " \
            "WHERE category_code = '%s';";

        const char * sqlInsert = 
            "INSERT INTO budget (" \
            "start_date," \
            "end_date," \
            "category_code," \
            "payee_code," \
            "description," \
            "min_budget," \
            "max_budget," \
            "created," \
            "updated)" \
            "VALUES (" \
            "'%s', '%s', '%s', '%s', '%s'," \
            "'%s', '%s', '%s', '%s');";

        const char * sqlUpdate = 
            "UPDATE budget SET " \
            "start_date = '%s'," \
            "end_date = '%s'," \
            "category_code = '%s'," \
            "payee_code = '%s'," \
            "description = '%s'," \
            "min_budget = '%s'," \
            "max_budget = '%s'," \
            "updated = '%s' " \
            "WHERE id = %lld;";

    public:
        StrDate startDate;
        StrDate endDate;
        string categoryCode;
        string payeeCode;
        string description;
        Money minimumBudget;
        Money maximumBudget;

        DBBudget() : DBEntity() {
            clear();
        }

        DBBudget(const DBBudget & src) : DBEntity(src) {
            set(src);
        }

        void clear() {
            DBEntity::clear();

            this->startDate.clear();
            this->endDate.clear();
            this->payeeCode = "";
            this->categoryCode = "";
            this->description = "";
            this->minimumBudget = 0.0;
            this->maximumBudget = 0.0;
        }

        void set(const DBBudget & src) {
            DBEntity::set(src);

            this->startDate = src.startDate;
            this->endDate = src.endDate;
            this->payeeCode = src.payeeCode;
            this->categoryCode = src.categoryCode;
            this->description = src.description;
            this->minimumBudget = src.minimumBudget;
            this->maximumBudget = src.maximumBudget;
        }

        void set(JRecord & record) {
            this->startDate = record.get("startDate");
            this->endDate = record.get("endDate");
            this->payeeCode = record.get("payeeCode");
            this->categoryCode = record.get("categoryCode");
            this->description = record.get("description");
            this->minimumBudget = record.get("minimumBudget");
            this->maximumBudget = record.get("maximumBudget");
        }

        JRecord getRecord() {
            JRecord r;

            r.add("startDate", this->startDate.shortDate());
            r.add("endDate", this->endDate.shortDate());
            r.add("payeeCode", this->payeeCode);
            r.add("categoryCode", this->categoryCode);
            r.add("description", this->description);
            r.add("minimumBudget", this->minimumBudget.getRawStringValue());
            r.add("maximumBudget", this->maximumBudget.getRawStringValue());

            return r;
        }

        void print() {
            DBEntity::print();

            cout << "StartDate: '" << startDate.shortDate() << "'" << endl;
            cout << "EndDate: '" << endDate.shortDate() << "'" << endl;
            cout << "PayeeCode: '" << payeeCode << "'" << endl;
            cout << "CategoryCode: '" << categoryCode << "'" << endl;
            cout << "Description: '" << description << "'" << endl;
            cout << "MinimumBudget: " << minimumBudget.getRawStringValue() << endl;
            cout << "MaximumBudget: " << maximumBudget.getRawStringValue() << endl;
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == "payee_code") {
                payeeCode = column.getValue();
            }
            else if (column.getName() == "category_code") {
                categoryCode = column.getValue();
            }
            else if (column.getName() == "start_date") {
                startDate = column.getValue();
            }
            else if (column.getName() == "end_date") {
                endDate = column.getValue();
            }
            else if (column.getName() == "description") {
                description = column.getValue();
            }
            else if (column.getName() == "min_budget") {
                minimumBudget = column.getDoubleValue();
            }
            else if (column.getName() == "max_budget") {
                maximumBudget = column.getDoubleValue();
            }
        }

        void onRowComplete(int sequence) override {
            this->sequence = sequence;
        }

        const char * getTableName() override {
            return "budget";
        }

        const char * getInsertStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlInsert,
                startDate.shortDate().c_str(),
                endDate.shortDate().c_str(),
                categoryCode.c_str(),
                payeeCode.c_str(),
                description.c_str(),
                minimumBudget.getRawStringValue().c_str(),
                maximumBudget.getRawStringValue().c_str(),
                now.c_str(),
                now.c_str());

            return szStatement;
        }

        const char * getUpdateStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlUpdate,
                startDate.shortDate().c_str(),
                endDate.shortDate().c_str(),
                categoryCode.c_str(),
                payeeCode.c_str(),
                description.c_str(),
                minimumBudget.getRawStringValue().c_str(),
                maximumBudget.getRawStringValue().c_str(),
                now.c_str(),
                id);

            return szStatement;
        }

        void afterInsert() override;

        int createOutstandingTrackingRecords();

        DBResult<DBBudget> retrieveByPayeeCode(string & code);
        DBResult<DBBudget> retrieveByCategoryCode(string & code);
        DBResult<DBBudget> retrieveByCategoryOrPayeeCode(string & categoryCode, string & payeeCode);
};

#endif

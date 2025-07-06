#include <string>
#include <string.h>
#include <stdint.h>

#include "pfm_error.h"
#include "db.h"
#include "db_base.h"
#include "strdate.h"
#include "money.h"

using namespace std;

#ifndef __INCL_BUDGET_TRACK
#define __INCL_BUDGET_TRACK

class DBBudgetTrack : public DBEntity {
    private:
        const char * sqlSelectByBudgetId = 
            "SELECT " \
            "id," \
            "budget_id," \
            "date," \
            "balance," \
            "created," \
            "updated " \
            "FROM period_budget_track " \
            "WHERE budget_id = %lld;";

        const char * sqlSelectByBudgetIdAfterDate = 
            "SELECT " \
            "id," \
            "budget_id," \
            "date," \
            "balance," \
            "created," \
            "updated " \
            "FROM period_budget_track " \
            "WHERE budget_id = %lld " \
            "AND date >= '%s';";

        const char * sqlSelectLatestByBudgetId = 
            "SELECT " \
            "id," \
            "budget_id," \
            "date," \
            "balance," \
            "created," \
            "updated " \
            "FROM period_budget_track " \
            "WHERE budget_id = %lld " \
            "ORDER BY date DESC " \
            "LIMIT 1;";

        const char * sqlInsert = 
            "INSERT INTO period_budget_track (" \
            "budget_id," \
            "date," \
            "balance," \
            "created," \
            "updated)" \
            "VALUES (" \
            "%lld, '%s', '%s', '%s', '%s');";

        const char * sqlUpdate = 
            "UPDATE period_budget_track SET " \
            "budget_id = %lld," \
            "date = '%s'," \
            "balance = '%s'," \
            "updated = '%s' " \
            "WHERE id = %lld;";

    public:
        pfm_id_t budgetId;
        StrDate date;
        Money balance;

        DBBudgetTrack() : DBEntity() {
            clear();
        }

        DBBudgetTrack(const DBBudgetTrack & src) : DBEntity(src) {
            set(src);
        }

        void clear() {
            DBEntity::clear();

            this->budgetId = 0;
            this->date.clear();
            this->balance = 0.0;
        }

        void set(const DBBudgetTrack & src) {
            DBEntity::set(src);

            this->budgetId = src.budgetId;
            this->date = src.date;
            this->balance = src.balance;
        }

        void print() {
            DBEntity::print();

            cout << "BudgetId: " << budgetId << endl;
            cout << "Date: '" << date.shortDate() << "'" << endl;
            cout << "Balance: " << balance.getRawStringValue() << endl;
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == "budget_id") {
                budgetId = column.getIDValue();
            }
            else if (column.getName() == "date") {
                date = column.getValue();
            }
            else if (column.getName() == "balance") {
                balance = column.getDoubleValue();
            }
        }

        const char * getTableName() override {
            return "period_budget_track";
        }

        const char * getClassName() override {
            return "DBBudgetTrack";
        }

        const char * getInsertStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlInsert,
                budgetId,
                date.shortDate().c_str(),
                balance.getRawStringValue().c_str(),
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
                budgetId,
                date.shortDate().c_str(),
                balance.getRawStringValue().c_str(),
                now.c_str(),
                id);

            return szStatement;
        }

        DBResult<DBBudgetTrack> retrieveByBudgetId(pfm_id_t budgetId);
        DBResult<DBBudgetTrack> retrieveByBudgetIdAfterDate(pfm_id_t budgetId, StrDate & date);
        int retrieveLatestByBudgetId(pfm_id_t budgetId);
};

#endif

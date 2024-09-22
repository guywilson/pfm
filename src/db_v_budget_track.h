#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdint.h>

#include <sqlcipher/sqlite3.h>

#include "db_recurring_charge.h"
#include "db_category.h"
#include "db_payee.h"
#include "db_payment.h"
#include "strdate.h"
#include "db.h"
#include "db_base.h"
#include "strdate.h"

using namespace std;

#ifndef __INCL_BUDGET_TRACK_VIEW
#define __INCL_BUDGET_TRACK_VIEW

class DBBudgetTrackView : public DBEntity {
    public:
        string payeeCode;
        string categoryCode;
        string description;
        StrDate date;
        Money balance;

        DBBudgetTrackView() : DBEntity() {
            clear();
        }

        DBBudgetTrackView(const DBBudgetTrackView & src) : DBEntity(src) {
            set(src);
        }

        void clear() {
            DBEntity::clear();

            this->payeeCode = "";
            this->categoryCode = "";
            this->description = "";
            this->date.clear();
            this->balance = 0.0;
        }

        void set(const DBBudgetTrackView & src) {
            DBEntity::set(src);

            this->payeeCode = src.payeeCode;
            this->categoryCode = src.categoryCode;
            this->description = src.description;
            this->date = src.date;
            this->balance = src.balance;
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == "payee_code") {
                payeeCode = column.getValue();
            }
            else if (column.getName() == "category_code") {
                categoryCode = column.getValue();
            }
            else if (column.getName() == "description") {
                description = column.getValue();
            }
            else if (column.getName() == "date") {
                date = column.getValue();
            }
            else if (column.getName() == "balance") {
                balance = column.getDoubleValue();
            }
        }

        const char * getTableName() override {
            return "v_budget_track";
        }
};

#endif

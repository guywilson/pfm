#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdint.h>

#include <sqlcipher/sqlite3.h>

#include "db_carried_over.h"
#include "db.h"
#include "db_base.h"
#include "strdate.h"

using namespace std;

#ifndef __INCL_CARRIED_OVER_VIEW
#define __INCL_CARRIED_OVER_VIEW

class DBCarriedOverView : public DBEntity {
    public:
        string                  accountCode;
        StrDate                 date;
        string                  description;
        Money                   balance;

        DBCarriedOverView() : DBEntity() {
            clear();
        }

        DBCarriedOverView(const DBCarriedOverView & src) : DBEntity(src) {
            set(src);
        }

        void clear() {
            DBEntity::clear();

            this->accountCode = "";
            this->date.clear();
            this->description = "";
            this->balance = 0.0;
        }

        void set(const DBCarriedOverView & src) {
            DBEntity::set(src);

            this->accountCode = src.accountCode;
            this->date = src.date;
            this->description = src.description;
            this->balance = src.balance;
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == "account_code") {
                accountCode = column.getValue();
            }
            else if (column.getName() == "date") {
                date = column.getValue();
            }
            else if (column.getName() == "description") {
                description = column.getValue();
            }
            else if (column.getName() == "balance") {
                balance = column.getDoubleValue();
            }
        }

        const char * getTableName() override {
            return "v_carried_over_log";
        }

        const char * getClassName() override {
            return "DBCarriedOverView";
        }
};

#endif

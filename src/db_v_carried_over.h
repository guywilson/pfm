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

class DBCarriedOverView : public DBCarriedOver {
    protected:
        struct Columns {
            static constexpr const char * accountCode = "account_code";
            static constexpr ColumnType accountCode_type = ColumnType::TEXT;
        };

    public:
        string accountCode;

        DBCarriedOverView() : DBCarriedOver() {
            clear();
        }

        DBCarriedOverView(const DBCarriedOverView & src) : DBCarriedOver(src) {
            set(src);
        }

        void clear() {
            DBCarriedOver::clear();

            this->accountCode = "";
        }

        void set(const DBCarriedOverView & src) {
            DBCarriedOver::set(src);

            this->accountCode = src.accountCode;
        }

        void assignColumn(DBColumn & column) override {
            DBCarriedOver::assignColumn(column);
            
            if (column.getName() == Columns::accountCode) {
                accountCode = column.getValue();
            }
        }

        const string getTableName() const override {
            return "v_carried_over_log";
        }

        const string getClassName() const override {
            return "DBCarriedOverView";
        }
};

#endif

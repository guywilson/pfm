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

#ifndef __INCL_RECURRING_CHARGE_VIEW
#define __INCL_RECURRING_CHARGE_VIEW

class DBRecurringChargeView : public DBRecurringCharge {
    protected:
        struct Columns {
            static constexpr const char * payeeCode = "payee_code";
            static constexpr ColumnType payeeCode_type = ColumnType::TEXT;

            static constexpr const char * categoryCode = "category_code";
            static constexpr ColumnType categoryCode_type = ColumnType::TEXT;
        };

    public:
        string payeeCode;
        string categoryCode;

        DBRecurringChargeView() : DBRecurringCharge() {
            clear();
        }

        DBRecurringChargeView(const DBRecurringChargeView & src) : DBRecurringCharge(src) {
            set(src);
        }

        void clear() {
            DBRecurringCharge::clear();

            this->payeeCode = "";
            this->categoryCode = "";
        }

        void set(const DBRecurringChargeView & src) {
            DBRecurringCharge::set(src);

            this->payeeCode = src.payeeCode;
            this->categoryCode = src.categoryCode;
        }

        void print() {
            DBRecurringCharge::print();

            cout << "PayeeCode: '" << payeeCode << "'" << endl;
            cout << "CategoryCode: '" << categoryCode << "'" << endl;
        }

        void assignColumn(DBColumn & column) override {
            DBRecurringCharge::assignColumn(column);
            
            if (column.getName() == Columns::payeeCode) {
                payeeCode = column.getValue();
            }
            else if (column.getName() == Columns::categoryCode) {
                categoryCode = column.getValue();
            }
        }

        void onRowComplete(int sequence) override {
            DBRecurringCharge::onRowComplete(sequence);
        }

        const string getTableName() const override {
            return "v_recurring_charge_list";
        }

        const string getClassName() const override {
            return "DBRecurringChrageView";
        }

        DBResult<DBRecurringChargeView> retrieveByAccountID(pfm_id_t & accountId);
        DBResult<DBRecurringChargeView> retrieveByAccountIDBetweenDates(pfm_id_t & accountId, StrDate & dateAfter, StrDate & dateBefore);
};

#endif

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
    private:
        const char * sqlSelectByAccountID = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "category_code," \
                        "payee_code," \
                        "date," \
                        "end_date," \
                        "description," \
                        "amount," \
                        "frequency," \
                        "created," \
                        "updated " \
                        "FROM v_recurring_charge_list " \
                        "WHERE account_id = %lld;";

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
            DBPayment::set(src);

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
            
            if (column.getName() == "payee_code") {
                payeeCode = column.getValue();
            }
            else if (column.getName() == "category_code") {
                categoryCode = column.getValue();
            }
        }

        void onRowComplete(int sequence) override {
            this->sequence = sequence;
            
            setNextPaymentDate();
        }

        const char * getTableName() override {
            return "v_recurring_charge_list";
        }

        DBResult<DBRecurringChargeView> retrieveByAccountID(pfm_id_t accountId);
};

#endif

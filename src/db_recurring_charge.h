#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdint.h>

#include <sqlite3.h>

#include "db_category.h"
#include "db_payee.h"

using namespace std;

#ifndef __INCL_RECURRING_CHARGE
#define __INCL_RECURRING_CHARGE

class DBRecurringCharge {
    public:
        DBRecurringCharge() {
            clear();
        }

        void clear(void) {
            this->id = 0;

            this->sequence = 0;

            this->accountId = 0;
            this->categoryId = 0;
            this->payeeId = 0;

            this->category.clear();
            this->payee.clear();

            this->date = "";
            this->description = "";
            this->frequency = "";
            this->amount = 0.0;
        }

        void setRecurringCharge(const DBRecurringCharge & src) {
            this->id = src.id;

            this->sequence = src.sequence;

            this->accountId = src.accountId;
            this->categoryId = src.categoryId;
            this->payeeId = src.payeeId;

            this->date = src.date;
            this->description = src.description;
            this->frequency = src.frequency;
            this->amount = src.amount;
        }

        void print(void) {
            cout << "ID: " << id << endl;
            cout << "Sequence: " << sequence << endl;
            cout << "AccountID: " << accountId << endl;
            cout << "CategoryID: " << categoryId << endl;
            cout << "PayeeID: " << payeeId << endl;

            cout << "Date: '" << date << "'" << endl;
            cout << "Description: '" << description << "'" << endl;
            cout << "Frequency: '" << frequency << "'" << endl;

            cout << fixed << setprecision(2);
            cout << "Amount: " << amount << endl;

            cout << "DBCategory (encapsulated object):" << endl;
            category.print();

            cout << "DBPayee (encapsulated object):" << endl;
            payee.print();
        }

        sqlite3_int64           id;

        uint32_t                sequence;           // Not persistent
        string                  nextPaymentDate;    // Not persistent

        sqlite3_int64           accountId;
        sqlite3_int64           categoryId;
        sqlite3_int64           payeeId;

        DBCategory                category;
        DBPayee                   payee;

        string                  date;
        string                  description;
        string                  frequency;
        double                  amount;
};

class DBRecurringChargeResult {
    public:
        DBRecurringChargeResult() {
            numRows = 0;
        }

        void clear() {
            numRows = 0;
            results.clear();
        }

        int                     numRows;

        vector<DBRecurringCharge> results;
};

#endif

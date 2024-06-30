#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlite3.h>

using namespace std;

#ifndef __INCL_RECURRING_CHARGE
#define __INCL_RECURRING_CHARGE

class RecurringCharge {
    public:
        RecurringCharge() {
            clear();
        }

        void clear(void) {
            this->id = 0;

            this->accountId = 0;
            this->categoryId = 0;
            this->payeeId = 0;

            this->date = "";
            this->description = "";
            this->frequency = "";
            this->amount = 0.0;
        }

        void setRecurringCharge(const RecurringCharge & src) {
            this->id = src.id;

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
            cout << "AccountID: " << accountId << endl;
            cout << "CategoryID: " << categoryId << endl;
            cout << "PayeeID: " << payeeId << endl;

            cout << "Date: '" << date << "'" << endl;
            cout << "Description: '" << description << "'" << endl;
            cout << "Frequency: '" << frequency << "'" << endl;

            cout << fixed << setprecision(2);
            cout << "Amount: " << amount << endl;
        }

        sqlite3_int64           id;

        sqlite3_int64           accountId;
        sqlite3_int64           categoryId;
        sqlite3_int64           payeeId;

        string                  date;
        string                  description;
        string                  frequency;
        double                  amount;
};

class RecurringChargeResult {
    public:
        RecurringChargeResult() {
            numRows = 0;
        }

        int                     numRows;

        vector<RecurringCharge> results;
};

#endif

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdint.h>

#include <sqlite3.h>

#include "db_category.h"
#include "db_payee.h"
#include "db_base.h"

using namespace std;

#ifndef __INCL_PAYMENT
#define __INCL_PAYMENT

class DBPayment : public DBBase {
    public:
        DBPayment() : DBBase() {
            clear();
        }

        void clear(void) {
            DBBase::clear();

            this->accountId = 0;
            this->categoryId = 0;
            this->payeeId = 0;

            this->category.clear();
            this->payee.clear();

            this->date = "";
            this->description = "";
            this->amount = 0.0;
        }

        void set(const DBPayment & src) {
            DBBase::set(src);

            this->accountId = src.accountId;
            this->categoryId = src.categoryId;
            this->payeeId = src.payeeId;

            this->date = src.date;
            this->description = src.description;
            this->amount = src.amount;
        }

        void print(void) {
            DBBase::print();

            cout << "AccountID: " << accountId << endl;
            cout << "CategoryID: " << categoryId << endl;
            cout << "PayeeID: " << payeeId << endl;

            cout << "Date: '" << date << "'" << endl;
            cout << "Description: '" << description << "'" << endl;

            cout << fixed << setprecision(2);
            cout << "Amount: " << amount << endl;

            cout << "DBCategory (encapsulated object):" << endl;
            category.print();

            cout << "DBPayee (encapsulated object):" << endl;
            payee.print();
        }

        sqlite3_int64           accountId;
        sqlite3_int64           categoryId;
        sqlite3_int64           payeeId;

        DBCategory              category;
        DBPayee                 payee;

        string                  date;
        string                  description;
        double                  amount;
};

#endif

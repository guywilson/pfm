#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdint.h>

#include <sqlite3.h>

#include "db_category.h"
#include "db_payee.h"
#include "db_payment.h"

using namespace std;

#ifndef __INCL_TRANSACTION
#define __INCL_TRANSACTION

class DBTransaction : public DBPayment {
    public:
        DBTransaction() : DBPayment() {
            clear();
        }

        void clear(void) {
            DBPayment::clear();

            this->isCredit = false;
            this->isReconciled = false;
        }

        void set(const DBTransaction & src) {
            DBPayment::set(src);

            this->isCredit = src.isCredit;
            this->isReconciled = src.isReconciled;
        }

        void print(void) {
            DBPayment::print();

            cout << "Debit/Credit: '" << (isCredit ? "CR" : "DB") << "'" << endl;
            cout << "isReconciled: " << isReconciled << endl;
        }

        bool                    isCredit;
        bool                    isReconciled;
};

class DBTransactionResult {
    public:
        DBTransactionResult() {
            numRows = 0;
        }

        void clear() {
            numRows = 0;
            results.clear();
        }

        int                     numRows;

        vector<DBTransaction> results;
};

#endif

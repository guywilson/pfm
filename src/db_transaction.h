#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdint.h>

#include <sqlite3.h>

#include "db_category.h"
#include "db_payee.h"

using namespace std;

#ifndef __INCL_TRANSACTION
#define __INCL_TRANSACTION

class DBTransaction {
    public:
        DBTransaction() {
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
            this->isCredit = false;
            this->isReconciled = false;
            this->amount = 0.0;
        }

        void setTransaction(const DBTransaction & src) {
            this->id = src.id;

            this->sequence = src.sequence;

            this->accountId = src.accountId;
            this->categoryId = src.categoryId;
            this->payeeId = src.payeeId;

            this->date = src.date;
            this->description = src.description;
            this->isCredit = src.isCredit;
            this->isReconciled = src.isReconciled;
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
            cout << "Debit/Credit: '" << (isCredit ? "CR" : "DB") << "'" << endl;
            cout << "isReconciled: " << isReconciled << endl;

            cout << fixed << setprecision(2);
            cout << "Amount: " << amount << endl;

            cout << "DBCategory (encapsulated object):" << endl;
            category.print();

            cout << "DBPayee (encapsulated object):" << endl;
            payee.print();
        }

        sqlite3_int64           id;

        uint32_t                sequence;           // Not persistent

        sqlite3_int64           accountId;
        sqlite3_int64           categoryId;
        sqlite3_int64           payeeId;

        DBCategory                category;
        DBPayee                   payee;

        string                  date;
        string                  description;
        bool                    isCredit;
        bool                    isReconciled;
        double                  amount;
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

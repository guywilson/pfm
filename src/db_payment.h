#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdint.h>

#include <sqlcipher/sqlite3.h>

#include "db_account.h"
#include "db_category.h"
#include "db_payee.h"
#include "db.h"
#include "db_base.h"
#include "strdate.h"
#include "jfile.h"
#include "money.h"

using namespace std;

#ifndef __INCL_PAYMENT
#define __INCL_PAYMENT

class DBPayment : public DBEntity {
    public:
        pfm_id_t accountId;
        pfm_id_t categoryId;
        pfm_id_t payeeId;

        StrDate date;
        string description;
        Money amount;

        DBCategory category;
        DBPayee payee;

        DBPayment() : DBEntity() {
            clear();
        }

        DBPayment(const DBPayment & src) : DBEntity(src) {
            set(src);
        }

        void clear() {
            DBEntity::clear();

            this->accountId = 0;
            this->categoryId = 0;
            this->payeeId = 0;

            this->category.clear();
            this->payee.clear();

            this->date.clear();

            this->description = "";
            this->amount = 0.0;
        }

        void set(const DBPayment & src) {
            DBEntity::set(src);

            this->accountId = src.accountId;
            this->categoryId = src.categoryId;
            this->payeeId = src.payeeId;

            this->date = src.date;
            this->description = src.description;
            this->amount = src.amount;

            this->category.set(src.category);
            this->payee.set(src.payee);
        }

        void set(JRecord & record) {
            DBAccount account;
            string accountCode = record.get("accountCode");
            account.retrieveByCode(accountCode);

            DBCategory category;
            string categoryCode = record.get("categoryCode");
            category.retrieveByCode(categoryCode);

            DBPayee payee;
            string payeeCode = record.get("payeeCode");
            payee.retrieveByCode(payeeCode);

            this->accountId = account.id;
            this->categoryId = category.id;
            this->payeeId = payee.id;
            this->amount = record.get("amount");
            this->date = record.get("date");
            this->description = record.get("description");
        }

        void print() {
            DBEntity::print();

            cout << "AccountID: " << accountId << endl;
            cout << "CategoryID: " << categoryId << endl;
            cout << "PayeeID: " << payeeId << endl;

            cout << "Date: '" << date.shortDate() << "'" << endl;
            cout << "Description: '" << description << "'" << endl;

            cout << fixed << setprecision(2);
            cout << "Amount: " << amount.getFormattedStringValue() << endl;

            cout << "DBCategory (encapsulated object):" << endl;
            category.print();

            cout << "DBPayee (encapsulated object):" << endl;
            payee.print();
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == "account_id") {
                accountId = column.getIDValue();
            }
            else if (column.getName() == "category_id") {
                categoryId = column.getIDValue();
            }
            else if (column.getName() == "payee_id") {
                payeeId = column.getIDValue();
            }
            else if (column.getName() == "date") {
                date = column.getValue();
            }
            else if (column.getName() == "description") {
                description = column.getValue();
            }
            else if (column.getName() == "amount") {
                amount = column.getDoubleValue();
            }
        }
};

#endif

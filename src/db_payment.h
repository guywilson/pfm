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
#include "cfgmgr.h"

using namespace std;

#ifndef __INCL_PAYMENT
#define __INCL_PAYMENT

class DBPayment : public DBEntity {
    protected:
        struct Columns {
            static constexpr const char * accountId = "account_id";
            static constexpr ColumnType accountId_type = ColumnType::ID;

            static constexpr const char * categoryId = "category_id";
            static constexpr ColumnType categoryId_type = ColumnType::ID;

            static constexpr const char * payeeId = "payee_id";
            static constexpr ColumnType payeeId_type = ColumnType::ID;

            static constexpr const char * date = "date";
            static constexpr ColumnType date_type = ColumnType::DATE;

            static constexpr const char * description = "description";
            static constexpr ColumnType description_type = ColumnType::TEXT;

            static constexpr const char * amount = "amount";
            static constexpr ColumnType amount_type = ColumnType::MONEY;

            static constexpr const char * isTransfer = "is_transfer";
            static constexpr ColumnType isTransfer_type = ColumnType::BOOL;
        };

        string getAccountCode() {
            DBAccount account;
            account.retrieve(this->accountId);

            return account.code;
        }

    public:
        pfm_id_t accountId;
        pfm_id_t categoryId;
        pfm_id_t payeeId;

        StrDate date;
        string description;
        Money amount;

        bool isTransfer;

        DBAccount account;
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

            this->accountId.clear();
            this->categoryId.clear();
            this->payeeId.clear();

            this->account.clear();
            this->category.clear();
            this->payee.clear();

            this->date.clear();

            this->description = "";
            this->amount = 0.0;

            this->isTransfer = false;
        }

        void set(const DBPayment & src) {
            DBEntity::set(src);

            this->accountId = src.accountId;
            this->categoryId = src.categoryId;
            this->payeeId = src.payeeId;

            this->date = src.date;
            this->description = src.description;
            this->amount = src.amount;

            this->isTransfer = src.isTransfer;

            this->account.set(src.account);
            this->category.set(src.category);
            this->payee.set(src.payee);
        }

        void set(JRecord & record) {
            string accountCode = record.get("accountCode");
            account.retrieveByCode(accountCode);

            try {
                string categoryCode = record.get("categoryCode");
                category.retrieveByCode(categoryCode);
            }
            catch (pfm_error & e) {
                // Do nothing
            }

            try {
                string payeeCode = record.get("payeeCode");
                payee.retrieveByCode(payeeCode);
            }
            catch (pfm_error & e) {
                // Do nothing
            }

            this->accountId = account.id;
            this->categoryId = category.id;
            this->payeeId = payee.id;
            this->amount = record.get("amount");
            this->date = record.get("date");
            this->description = record.get("description");
            this->isTransfer = (record.get("isTransfer").compare("Y") == 0 ? true : false);
        }

        JRecord getRecord() override {
            JRecord r;

            DBAccount account;
            account.retrieve(this->accountId);
            r.add("accountCode", account.code);

            if (!this->categoryId.isNull()) {
                DBCategory category;
                category.retrieve(this->categoryId);
                r.add("categoryCode", category.code);
            }

            if (!this->payeeId.isNull()) {
                DBPayee payee;
                payee.retrieve(this->payeeId);
                r.add("payeeCode", payee.code);
            }

            r.add("date", this->date.shortDate());
            r.add("description", this->description);
            r.add("amount", this->amount.rawStringValue());
            r.add("isTransfer", getIsTransferValue());

            return r;
        }

        void print() {
            DBEntity::print();

            cout << "AccountID: " << accountId.getValue() << endl;
            cout << "CategoryID: " << categoryId.getValue() << endl;
            cout << "PayeeID: " << payeeId.getValue() << endl;

            cout << "Date: '" << date.shortDate() << "'" << endl;
            cout << "Description: '" << description << "'" << endl;

            cout << fixed << setprecision(2);
            cout << "Amount: " << amount.localeFormattedStringValue() << endl;

            cout << "isTransfer: " << isTransfer << endl;

            cout << "DBCategory (encapsulated object):" << endl;
            category.print();

            cout << "DBPayee (encapsulated object):" << endl;
            payee.print();
        }

        void onRowComplete(int sequence) override {
            this->sequence = sequence;
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == Columns::accountId) {
                accountId = column.getIDValue();
            }
            else if (column.getName() == Columns::categoryId) {
                categoryId = column.getIDValue();
            }
            else if (column.getName() == Columns::payeeId) {
                payeeId = column.getIDValue();
            }
            else if (column.getName() == Columns::date) {
                date = column.getValue();
            }
            else if (column.getName() == Columns::description) {
                description = column.getValue();
            }
            else if (column.getName() == Columns::amount) {
                amount = column.doubleValue();
            }
            else if (column.getName() == Columns::isTransfer) {
                isTransfer = column.getBoolValue();
            }
        }

        const char * getIsTransferValue() {
            return (isTransfer ? "Y" : "N");
        }
};

#endif

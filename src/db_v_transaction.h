#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdint.h>

#include <sqlcipher/sqlite3.h>

#include "db.h"
#include "db_base.h"
#include "db_transaction.h"
#include "strdate.h"

using namespace std;

#ifndef __INCL_TRANSACTION_VIEW
#define __INCL_TRANSACTION_VIEW

class DBTransactionView : public DBTransaction {
    protected:
        struct Columns {
            static constexpr const char * account = "account";
            static constexpr const char * category = "category";
            static constexpr const char * payee = "payee";
            static constexpr const char * type = "type";
            static constexpr const char * recurring = "recurring";
            static constexpr const char * isReconciled = "reconciled";
            static constexpr const char * total = "total";
        };

    public:
        string account;
        string payee;
        string category;
        bool isRecurring;

        Money total; // For category, payee reports

        DBTransactionView() : DBTransaction() {
            clear();
        }

        void clear() {
            DBTransaction::clear();

            this->account = "";
            this->payee = "";
            this->category = "";
            this->isRecurring = false;
        }

        void set(const DBTransactionView & src) {
            DBTransaction::set(src);

            this->account = src.account;
            this->payee = src.payee;
            this->category = src.category;
            this->isRecurring = src.isRecurring;
        }

        void print() {
            DBTransaction::print();

            cout << "Account: '" << account << "'" << endl;
            cout << "IsRecurring: '" << isRecurring << "'" << endl;
            cout << "Payee: '" << payee << "'" << endl;
            cout << "Category: '" << category << "'" << endl;
        }

        void assignColumn(DBColumn & column) override {
            DBTransaction::assignColumn(column);
            
            if (column.getName() == Columns::payee) {
                payee = column.getValue();
            }
            else if (column.getName() == Columns::category) {
                category = column.getValue();
            }
            else if (column.getName() == Columns::account) {
                account = column.getValue();
            }
            else if (column.getName() == Columns::recurring) {
                isRecurring = column.getBoolValue();
            }
            else if (column.getName() == Columns::type) {
                type = column.getValue();
            }
            else if (column.getName() == Columns::isReconciled) {
                isReconciled = column.getBoolValue();
            }
            else if (column.getName() == Columns::total) {
                total = column.doubleValue();
            }
        }

        void onRowComplete(int sequence) override {
            this->sequence = sequence;
        }

        const char * getTableName() override {
            return "v_transaction_list";
        }

        const char * getClassName() override {
            return "DBTransactionView";
        }

        DBResult<DBTransactionView> retrieveByAccountID(pfm_id_t & accountId);
        DBResult<DBTransactionView> retrieveByAccountID(pfm_id_t & accountId, DBCriteria::sql_order dateSortDirection, int rowLimit);
        DBResult<DBTransactionView> retrieveReconciledByAccountID(pfm_id_t & accountId);
        DBResult<DBTransactionView> retrieveNonRecurringByAccountID(pfm_id_t & accountId, DBCriteria::sql_order dateSortDirection, int rowLimit);
        DBResult<DBTransactionView> retrieveByAccountIDForPeriod(pfm_id_t & accountId, DBCriteria::sql_order dateSortDirection, StrDate & firstDate, StrDate & secondDate);
        DBResult<DBTransactionView> retrieveNonRecurringByAccountIDForPeriod(pfm_id_t & accountId, StrDate & firstDate, StrDate & secondDate);
        DBResult<DBTransactionView> retrieveReconciledByAccountIDForPeriod(pfm_id_t & accountId, StrDate & firstDate, StrDate & secondDate);
        DBResult<DBTransactionView> findTransactions(const string & sql);
        DBResult<DBTransactionView> findTransactionsForCriteria(DBCriteria & criteria);
        DBResult<DBTransactionView> findTransactionsForAccountID(pfm_id_t & accountId, DBCriteria & criteria);
        DBResult<DBTransactionView> reportByCategory();
        DBResult<DBTransactionView> reportByPayee();
};

#endif

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
    private:
        const char * sqlSelectByAccountID = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "category_code," \
                        "payee_code," \
                        "recurring_charge_id," \
                        "date," \
                        "reference," \
                        "description," \
                        "credit_debit," \
                        "amount," \
                        "is_reconciled," \
                        "created," \
                        "updated " \
                        "FROM v_transaction_list " \
                        "WHERE account_id = %lld;";

        const char * sqlSelectByAccountIDSortedByDate = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "category_code," \
                        "payee_code," \
                        "recurring_charge_id," \
                        "date," \
                        "reference," \
                        "description," \
                        "credit_debit," \
                        "amount," \
                        "is_reconciled," \
                        "created," \
                        "updated " \
                        "FROM v_transaction_list " \
                        "WHERE account_id = %lld " \
                        "ORDER BY date %s";

        const char * sqlSelectNonRecurringByAccountIDSortedByDate = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "category_code," \
                        "payee_code," \
                        "recurring_charge_id," \
                        "date," \
                        "reference," \
                        "description," \
                        "credit_debit," \
                        "amount," \
                        "is_reconciled," \
                        "created," \
                        "updated " \
                        "FROM v_transaction_list " \
                        "WHERE account_id = %lld " \
                        "AND recurring_charge_id = 0 " \
                        "ORDER BY date %s";

        const char * sqlSelectByAccountIDBetweenDates = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "category_code," \
                        "payee_code," \
                        "recurring_charge_id," \
                        "date," \
                        "reference," \
                        "description," \
                        "credit_debit," \
                        "amount," \
                        "is_reconciled," \
                        "created," \
                        "updated " \
                        "FROM v_transaction_list " \
                        "WHERE account_id = %lld " \
                        "AND date >= '%s' " \
                        "AND date <= '%s';";

        const char * sqlSelectNonRecurringByAccountIDBetweenDates = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "category_code," \
                        "payee_code," \
                        "recurring_charge_id," \
                        "date," \
                        "reference," \
                        "description," \
                        "credit_debit," \
                        "amount," \
                        "is_reconciled," \
                        "created," \
                        "updated " \
                        "FROM v_transaction_list " \
                        "WHERE account_id = %lld " \
                        "AND recurring_charge_id = 0 " \
                        "AND date >= '%s' " \
                        "AND date <= '%s';";

        DBResult<DBTransactionView> retrieveByStatementAndID(const char * statement, pfm_id_t id);

    public:
        string payeeCode;
        string categoryCode;

        DBTransactionView() : DBTransaction() {
            clear();
        }

        void clear() {
            DBTransaction::clear();

            this->payeeCode = "";
            this->categoryCode = "";
        }

        void set(const DBTransactionView & src) {
            DBTransaction::set(src);

            this->payeeCode = src.payeeCode;
            this->categoryCode = src.categoryCode;
        }

        void print() {
            DBTransaction::print();

            cout << "PayeeCode: '" << payeeCode << "'" << endl;
            cout << "CategoryCode: '" << categoryCode << "'" << endl;
        }

        void assignColumn(DBColumn & column) override {
            DBTransaction::assignColumn(column);
            
            if (column.getName() == "payee_code") {
                payeeCode = column.getValue();
            }
            else if (column.getName() == "category_code") {
                categoryCode = column.getValue();
            }
        }

        void onRowComplete(int sequence) override {
            this->sequence = sequence;
        }

        const char * getTableName() override {
            return "v_transaction_view";
        }

        DBResult<DBTransactionView> retrieveByAccountID(pfm_id_t accountId);
        DBResult<DBTransactionView> retrieveByAccountID(pfm_id_t accountId, db_sort_t dateSortDirection, int rowLimit);
        DBResult<DBTransactionView> retrieveNonRecurringByAccountID(pfm_id_t accountId, db_sort_t dateSortDirection, int rowLimit);
        DBResult<DBTransactionView> retrieveByAccountIDForPeriod(pfm_id_t accountId, StrDate & firstDate, StrDate & secondDate);
        DBResult<DBTransactionView> retrieveNonRecurringByAccountIDForPeriod(pfm_id_t accountId, StrDate & firstDate, StrDate & secondDate);
        DBResult<DBTransactionView> findTransactionsForAccountID(pfm_id_t accountId, string & criteria);
};

#endif

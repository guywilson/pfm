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
        const char * sqlSelectByCriteria = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "account_code," \
                        "category_id," \
                        "category_code," \
                        "payee_id," \
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
                        "WHERE ";

        const char * sqlSelectByAccountID = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "account_code," \
                        "category_id," \
                        "category_code," \
                        "payee_id," \
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
                        "WHERE account_id = %s;";

        const char * sqlSelectReconciledByAccountID = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "account_code," \
                        "category_id," \
                        "category_code," \
                        "payee_id," \
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
                        "WHERE account_id = %s " \
                        "AND is_reconciled = 'Y'";

        const char * sqlSelectByAccountIDSortedByDate = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "account_code," \
                        "category_id," \
                        "category_code," \
                        "payee_id," \
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
                        "WHERE account_id = %s " \
                        "ORDER BY date %s";

        const char * sqlSelectNonRecurringByAccountIDSortedByDate = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "account_code," \
                        "category_id," \
                        "category_code," \
                        "payee_id," \
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
                        "WHERE account_id = %s " \
                        "AND recurring_charge_id IS NULL " \
                        "ORDER BY date %s";

        const char * sqlSelectByAccountIDBetweenDates = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "account_code," \
                        "category_id," \
                        "category_code," \
                        "payee_id," \
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
                        "WHERE account_id = %s " \
                        "AND date >= '%s' " \
                        "AND date <= '%s';";

        const char * sqlSelectReconciledByAccountIDBetweenDates = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "account_code," \
                        "category_id," \
                        "category_code," \
                        "payee_id," \
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
                        "WHERE account_id = %s " \
                        "AND is_reconciled = 'Y' " \
                        "AND date >= '%s' " \
                        "AND date <= '%s';";

        const char * sqlSelectNonRecurringByAccountIDBetweenDates = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "account_code," \
                        "category_id," \
                        "category_code," \
                        "payee_id," \
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
                        "WHERE account_id = %s " \
                        "AND recurring_charge_id IS NULL " \
                        "AND date >= '%s' " \
                        "AND date <= '%s';";

        DBResult<DBTransactionView> retrieveByStatementAndID(const char * statement, pfm_id_t id);

    public:
        string accountCode;
        string payeeCode;
        string categoryCode;

        DBTransactionView() : DBTransaction() {
            clear();
        }

        void clear() {
            DBTransaction::clear();

            this->accountCode = "";
            this->payeeCode = "";
            this->categoryCode = "";
        }

        void set(const DBTransactionView & src) {
            DBTransaction::set(src);

            this->accountCode = src.accountCode;
            this->payeeCode = src.payeeCode;
            this->categoryCode = src.categoryCode;
        }

        void print() {
            DBTransaction::print();

            cout << "AccountCode: '" << accountCode << "'" << endl;
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
            else if (column.getName() == "account_code") {
                accountCode = column.getValue();
            }
        }

        void onRowComplete(int sequence) override {
            this->sequence = sequence;
        }

        const char * getTableName() override {
            return "v_transaction_view";
        }

        const char * getClassName() override {
            return "DBTransactionView";
        }

        DBResult<DBTransactionView> retrieveByAccountID(pfm_id_t accountId);
        DBResult<DBTransactionView> retrieveByAccountID(pfm_id_t accountId, db_sort_t dateSortDirection, int rowLimit);
        DBResult<DBTransactionView> retrieveReconciledByAccountID(pfm_id_t accountId);
        DBResult<DBTransactionView> retrieveNonRecurringByAccountID(pfm_id_t accountId, db_sort_t dateSortDirection, int rowLimit);
        DBResult<DBTransactionView> retrieveByAccountIDForPeriod(pfm_id_t accountId, StrDate & firstDate, StrDate & secondDate);
        DBResult<DBTransactionView> retrieveNonRecurringByAccountIDForPeriod(pfm_id_t accountId, StrDate & firstDate, StrDate & secondDate);
        DBResult<DBTransactionView> retrieveReconciledByAccountIDForPeriod(pfm_id_t accountId, StrDate & firstDate, StrDate & secondDate);
        DBResult<DBTransactionView> findTransactionsForCriteria(const string & criteria);
        DBResult<DBTransactionView> findTransactionsForAccountID(pfm_id_t accountId, const string & criteria);
};

#endif

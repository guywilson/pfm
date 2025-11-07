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
                        "account," \
                        "category_id," \
                        "category," \
                        "payee_id," \
                        "payee," \
                        "recurring_charge_id," \
                        "recurring," \
                        "date," \
                        "reference," \
                        "description," \
                        "type," \
                        "amount," \
                        "reconciled," \
                        "created," \
                        "updated " \
                        "FROM v_transaction_list " \
                        "WHERE ";

        const char * sqlSelectByAccountID = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "account," \
                        "category_id," \
                        "category," \
                        "payee_id," \
                        "payee," \
                        "recurring_charge_id," \
                        "recurring," \
                        "date," \
                        "reference," \
                        "description," \
                        "type," \
                        "amount," \
                        "reconciled," \
                        "created," \
                        "updated " \
                        "FROM v_transaction_list " \
                        "WHERE account_id = %s;";

        const char * sqlSelectReconciledByAccountID = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "account," \
                        "category_id," \
                        "category," \
                        "payee_id," \
                        "payee," \
                        "recurring_charge_id," \
                        "recurring," \
                        "date," \
                        "reference," \
                        "description," \
                        "type," \
                        "amount," \
                        "reconciled," \
                        "created," \
                        "updated " \
                        "FROM v_transaction_list " \
                        "WHERE account_id = %s " \
                        "AND reconciled = 'Y'";

        const char * sqlSelectByAccountIDSortedByDate = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "account," \
                        "category_id," \
                        "category," \
                        "payee_id," \
                        "payee," \
                        "recurring_charge_id," \
                        "recurring," \
                        "date," \
                        "reference," \
                        "description," \
                        "type," \
                        "amount," \
                        "reconciled," \
                        "created," \
                        "updated " \
                        "FROM v_transaction_list " \
                        "WHERE account_id = %s " \
                        "ORDER BY date %s";

        const char * sqlSelectNonRecurringByAccountIDSortedByDate = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "account," \
                        "category_id," \
                        "category," \
                        "payee_id," \
                        "payee," \
                        "recurring_charge_id," \
                        "recurring," \
                        "date," \
                        "reference," \
                        "description," \
                        "type," \
                        "amount," \
                        "reconciled," \
                        "created," \
                        "updated " \
                        "FROM v_transaction_list " \
                        "WHERE account_id = %s " \
                        "AND recurring = 'N' " \
                        "ORDER BY date %s";

        const char * sqlSelectByAccountIDBetweenDates = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "account," \
                        "category_id," \
                        "category," \
                        "payee_id," \
                        "payee," \
                        "recurring_charge_id," \
                        "recurring," \
                        "date," \
                        "reference," \
                        "description," \
                        "type," \
                        "amount," \
                        "reconciled," \
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
                        "account," \
                        "category_id," \
                        "category," \
                        "payee_id," \
                        "payee," \
                        "recurring_charge_id," \
                        "recurring," \
                        "date," \
                        "reference," \
                        "description," \
                        "type," \
                        "amount," \
                        "reconciled," \
                        "created," \
                        "updated " \
                        "FROM v_transaction_list " \
                        "WHERE account_id = %s " \
                        "AND reconciled = 'Y' " \
                        "AND date >= '%s' " \
                        "AND date <= '%s';";

        const char * sqlSelectNonRecurringByAccountIDBetweenDates = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "account," \
                        "category_id," \
                        "category," \
                        "payee_id," \
                        "payee," \
                        "recurring_charge_id," \
                        "recurring," \
                        "date," \
                        "reference," \
                        "description," \
                        "type," \
                        "amount," \
                        "reconciled," \
                        "created," \
                        "updated " \
                        "FROM v_transaction_list " \
                        "WHERE account_id = %s " \
                        "AND recurring = 'N' " \
                        "AND date >= '%s' " \
                        "AND date <= '%s';";

        const char * sqlReportByCategoryNonRecurring =
                        "SELECT category," \
                        "SUM(amount) AS total " \
                        "FROM v_transaction_list " \
                        "WHERE recurring = 'N' " \
                        "AND type = 'DB' " \
                        "GROUP BY category;";

        const char * sqlReportByPayeeNonRecurring =
                        "SELECT payee," \
                        "SUM(amount) as total " \
                        "FROM v_transaction_list " \
                        "WHERE recurring = 'N' " \
                        "AND type = 'DB' " \
                        "GROUP BY payee;";

        DBResult<DBTransactionView> retrieveByStatementAndID(const char * statement, pfm_id_t id);

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
            
            if (column.getName() == "payee") {
                payee = column.getValue();
            }
            else if (column.getName() == "category") {
                category = column.getValue();
            }
            else if (column.getName() == "account") {
                account = column.getValue();
            }
            else if (column.getName() == "type") {
                isCredit = getIsCredit(column.getValue());
            }
            else if (column.getName() == "reconciled") {
                isReconciled = column.getBoolValue();
            }
            else if (column.getName() == "recurring") {
                isRecurring = column.getBoolValue();
            }
            else if (column.getName() == "total") {
                total = column.doubleValue();
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
        DBResult<DBTransactionView> reportByCategory();
        DBResult<DBTransactionView> reportByPayee();
};

#endif

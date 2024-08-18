#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdint.h>

#include <sqlite3.h>

#include "db_category.h"
#include "db_payee.h"
#include "db_recurring_charge.h"
#include "db_payment.h"
#include "db.h"
#include "db_base.h"
#include "strdate.h"

using namespace std;

#ifndef __INCL_TRANSACTION
#define __INCL_TRANSACTION

class DBTransactionResult;

class DBTransaction : public DBPayment {
    private:
        const char * sqlSelectByID = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "category_id," \
                        "payee_id," \
                        "date," \
                        "description," \
                        "credit_debit," \
                        "amount," \
                        "is_reconciled," \
                        "created," \
                        "updated " \
                        "FROM account_transaction " \
                        "WHERE id = %lld;";

        const char * sqlSelectByAccountID = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "category_id," \
                        "payee_id," \
                        "date," \
                        "description," \
                        "credit_debit," \
                        "amount," \
                        "is_reconciled," \
                        "created," \
                        "updated " \
                        "FROM account_transaction " \
                        "WHERE account_id = %lld;";

        const char * sqlSelectLatestByChargeID = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "category_id," \
                        "payee_id," \
                        "date," \
                        "description," \
                        "credit_debit," \
                        "amount," \
                        "is_reconciled," \
                        "created," \
                        "updated " \
                        "FROM account_transaction " \
                        "WHERE recurring_charge_id = %lld " \
                        "ORDER BY date DESC " \
                        "LIMIT 1;";

        const char * sqlInsert = 
                        "INSERT INTO account_transaction (" \
                        "account_id," \
                        "category_id," \
                        "payee_id," \
                        "date," \
                        "description," \
                        "credit_debit," \
                        "amount," \
                        "is_reconciled," \
                        "created," \
                        "updated) " \
                        "VALUES (%lld, %lld, %lld, '%s', '%s', " \
                        "'%s', %s, '%s', '%s', '%s');";

        const char * sqlUpdate = 
                        "UPDATE account_transaction " \
                        "SET category_id = %lld," \
                        "payee_id = %lld," \
                        "date = '%s'," \
                        "description = '%s'," \
                        "credit_debit = '%s'," \
                        "amount = %s," \
                        "is_reconciled = '%s'," \
                        "updated = '%s' " \
                        "WHERE id = %lld;";

        const char * sqlDelete = 
                        "DELETE FROM account_transaction WHERE id = %lld;";

        DBTransactionResult retrieveByStatementAndID(const char * statement, pfm_id_t id);

    public:
        bool                    isCredit;
        bool                    isReconciled;

        DBTransaction() : DBPayment() {
            clear();
        }

        void clear() {
            DBPayment::clear();

            this->isCredit = false;
            this->isReconciled = false;
        }

        void set(const DBTransaction & src) {
            DBPayment::set(src);

            this->isCredit = src.isCredit;
            this->isReconciled = src.isReconciled;
        }

        void setFromRecurringCharge(const DBRecurringCharge & src) {
            DBPayment::set(src);

            this->date = src.nextPaymentDate;
            this->isCredit = false;
            this->isReconciled = false;
        }

        void print() {
            DBPayment::print();

            cout << "Debit/Credit: '" << (isCredit ? "CR" : "DB") << "'" << endl;
            cout << "isReconciled: " << isReconciled << endl;
        }

        string getCreditDebitValue() {
            return (isCredit ? "CR" : "DB");
        }

        bool getIsCredit(string & value) {
            if (value == "CR") {
                return true;
            }
            else if (value == "DB") {
                return false;
            }
            else {
                return false;
            }
        }

        const char * getIsReconciledValue() {
            return (isReconciled ? "Y" : "N");
        }

        const char * getInsertStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlInsert,
                accountId,
                categoryId,
                payeeId,
                date.c_str(),
                description.c_str(),
                getCreditDebitValue().c_str(),
                amount.getRawStringValue().c_str(),
                getIsReconciledValue(),
                now.c_str(),
                now.c_str());

            return szStatement;
        }

        const char * getUpdateStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlUpdate,
                categoryId,
                payeeId,
                date.c_str(),
                description.c_str(),
                getCreditDebitValue().c_str(),
                amount.getRawStringValue().c_str(),
                getIsReconciledValue(),
                now.c_str(),
                id);

            return szStatement;
        }

        const char * getDeleteStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlDelete,
                id);

            return szStatement;
        }

        void retrieveByID(pfm_id_t id);
        int findLatestByRecurringChargeID(pfm_id_t chargeId);

        DBTransactionResult retrieveByAccountID(pfm_id_t accountId);
        DBTransactionResult findTransactionsForAccountID(
                                    pfm_id_t accountId, 
                                    DBCriteria * criteria, 
                                    int numCriteria);
};

class DBTransactionResult : public DBResult {
    private:
        vector<DBTransaction> results;

    public:
        DBTransactionResult() : DBResult() {}

        void clear() {
            DBResult::clear();
            results.clear();
        }

        DBTransaction getResultAt(int i) {
            if (getNumRows() > i) {
                return results[i];
            }
            else {
                throw pfm_error(
                        pfm_error::buildMsg(
                            "getResultAt(): Index out of range: numRows: %d, requested row: %d", getNumRows(), i), 
                        __FILE__, 
                        __LINE__);
            }
        }

        void processRow(DBRow & row) {
            DBTransaction transaction;

            for (size_t i = 0;i < row.getNumColumns();i++) {
                DBColumn column = row.getColumnAt(i);

                if (column.getName() == "id") {
                    transaction.id = column.getIDValue();
                }
                else if (column.getName() == "account_id") {
                    transaction.accountId = column.getIDValue();
                }
                else if (column.getName() == "category_id") {
                    transaction.categoryId = column.getIDValue();
                }
                else if (column.getName() == "payee_id") {
                    transaction.payeeId = column.getIDValue();
                }
                else if (column.getName() == "date") {
                    transaction.date = column.getValue();
                }
                else if (column.getName() == "description") {
                    transaction.description = column.getValue();
                }
                else if (column.getName() == "amount") {
                    transaction.amount = column.getDoubleValue();
                }
                else if (column.getName() == "credit_debit") {
                    transaction.isCredit = column.getBoolValue();
                }
                else if (column.getName() == "is_reconciled") {
                    transaction.isReconciled = column.getBoolValue();
                }
                else if (column.getName() == "created") {
                    transaction.createdDate = column.getValue();
                }
                else if (column.getName() == "updated") {
                    transaction.updatedDate = column.getValue();
                }
            }
            
            DBCategory category;
            category.retrieveByID(transaction.categoryId);
            transaction.category.set(category);

            DBPayee payee;
            payee.retrieveByID(transaction.payeeId);
            transaction.payee.set(payee);

            transaction.sequence = sequenceCounter++;

            results.push_back(transaction);
            incrementNumRows();
        }
};

#endif

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

class DBTransaction : public DBPayment {
    private:
        const char * sqlSelectByAccountID = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "category_id," \
                        "payee_id," \
                        "recurring_charge_id," \
                        "date," \
                        "reference," \
                        "description," \
                        "credit_debit," \
                        "amount," \
                        "is_reconciled," \
                        "created," \
                        "updated " \
                        "FROM account_transaction " \
                        "WHERE account_id = %lld;";

        const char * sqlSelectByAccountIDSortedByDate = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "category_id," \
                        "payee_id," \
                        "recurring_charge_id," \
                        "date," \
                        "reference," \
                        "description," \
                        "credit_debit," \
                        "amount," \
                        "is_reconciled," \
                        "created," \
                        "updated " \
                        "FROM account_transaction " \
                        "WHERE account_id = %lld " \
                        "ORDER BY date %s";

        const char * sqlSelectByAccountIDBetweenDates = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "category_id," \
                        "payee_id," \
                        "recurring_charge_id," \
                        "date," \
                        "reference," \
                        "description," \
                        "credit_debit," \
                        "amount," \
                        "is_reconciled," \
                        "created," \
                        "updated " \
                        "FROM account_transaction " \
                        "WHERE account_id = %lld " \
                        "AND date >= '%s' " \
                        "AND date <= '%s';";

        const char * sqlSelectByRecurringChargeID = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "category_id," \
                        "payee_id," \
                        "recurring_charge_id," \
                        "date," \
                        "reference," \
                        "description," \
                        "credit_debit," \
                        "amount," \
                        "is_reconciled," \
                        "created," \
                        "updated " \
                        "FROM account_transaction " \
                        "WHERE recurring_charge_id = %lld;";

        const char * sqlSelectLatestByChargeID = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "category_id," \
                        "payee_id," \
                        "recurring_charge_id," \
                        "date," \
                        "reference," \
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
                        "recurring_charge_id," \
                        "date," \
                        "reference," \
                        "description," \
                        "credit_debit," \
                        "amount," \
                        "is_reconciled," \
                        "created," \
                        "updated) " \
                        "VALUES (%lld, %lld, %lld, %lld, '%s', '%s', " \
                        "'%s', '%s', %s, '%s', '%s', '%s');";

        const char * sqlUpdate = 
                        "UPDATE account_transaction " \
                        "SET category_id = %lld," \
                        "payee_id = %lld," \
                        "recurring_charge_id = %lld,"
                        "date = '%s'," \
                        "reference = '%s'," \
                        "description = '%s'," \
                        "credit_debit = '%s'," \
                        "amount = %s," \
                        "is_reconciled = '%s'," \
                        "updated = '%s' " \
                        "WHERE id = %lld;";

        const char * sqlDelete = 
                        "DELETE FROM account_transaction WHERE id = %lld;";

        string makeUpperCase(string s) {
            for (int i = 0;i < s.length();i++) {
                s[i] = (char)toupper(s.at(i));
            }

            return s;
        }

        bool doesExistInString(string & src, const char * checkStr) {
            string upper = makeUpperCase(src);

            if (upper.find(checkStr) == string::npos) {
                return false;
            }

            return true;
        }

        bool containsInsert(string & criteria) {
            return doesExistInString(criteria, "INSERT");
        }

        bool containsUpdate(string & criteria) {
            return doesExistInString(criteria, "UPDATE");
        }

        bool containsDelete(string & criteria) {
            return doesExistInString(criteria, "DELETE");
        }

        bool containsJoin(string & criteria) {
            return doesExistInString(criteria, "JOIN");
        }

        DBResult<DBTransaction> retrieveByStatementAndID(const char * statement, pfm_id_t id);

    protected:
        void validateCriteria(string & criteria) {
            if (containsInsert(criteria) || containsUpdate(criteria) || containsDelete(criteria) || containsJoin(criteria)) {
                throw pfm_error("Invalid criteria string, SQL keywords are not permitted");
            }
        }

    public:
        pfm_id_t recurringChargeId;
        string reference;
        bool isCredit;
        bool isReconciled;

        DBTransaction() : DBPayment() {
            clear();
        }

        void clear() {
            DBPayment::clear();

            this->recurringChargeId = 0;
            this->reference = "";
            this->isCredit = false;
            this->isReconciled = false;
        }

        void set(const DBTransaction & src) {
            DBPayment::set(src);

            this->recurringChargeId = src.recurringChargeId;
            this->reference = src.reference;
            this->isCredit = src.isCredit;
            this->isReconciled = src.isReconciled;
        }

        void createFromRecurringChargeAndDate(const DBRecurringCharge & src, StrDate & transactionDate) {
            DBPayment::set(src);

            // We want to create (insert) a record, so clear the ID...
            this->id = 0;

            this->date = transactionDate;
            this->recurringChargeId = src.id;
            this->isCredit = false;
            this->isReconciled = false;

            this->save();
        }

        void print() {
            DBPayment::print();

            cout << "RecurringChargeId: " << recurringChargeId << endl;
            cout << "Reference: '" << reference << "'" << endl;
            cout << "Debit/Credit: '" << (isCredit ? "CR" : "DB") << "'" << endl;
            cout << "isReconciled: " << isReconciled << endl;
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
            else if (column.getName() == "recurring_charge_id") {
                recurringChargeId = column.getIDValue();
            }
            else if (column.getName() == "date") {
                date = column.getValue();
            }
            else if (column.getName() == "reference") {
                reference = column.getValue();
            }
            else if (column.getName() == "description") {
                description = column.getValue();
            }
            else if (column.getName() == "amount") {
                amount = column.getDoubleValue();
            }
            else if (column.getName() == "credit_debit") {
                isCredit = column.getBoolValue();
            }
            else if (column.getName() == "is_reconciled") {
                isReconciled = column.getBoolValue();
            }
        }

        void onRowComplete(int sequence) override {
            if (categoryId != 0) {
                category.retrieve(categoryId);
            }
            if (payeeId != 0) {
                payee.retrieve(payeeId);
            }

            this->sequence = sequence;
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

        Money getSignedAmount() {
            return (isCredit ? amount : (amount * -1));
        }

        const char * getIsReconciledValue() {
            return (isReconciled ? "Y" : "N");
        }

        const char * getTableName() override {
            return "account_transaction";
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
                recurringChargeId,
                date.shortDate().c_str(),
                reference.c_str(),
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
                recurringChargeId,
                date.shortDate().c_str(),
                reference.c_str(),
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

        void afterInsert() override;
        void beforeUpdate() override;
        void afterRemove() override;

        int findLatestByRecurringChargeID(pfm_id_t chargeId);
        int createNextTransactionForCharge(DBRecurringCharge & charge, StrDate & latestDate);

        DBResult<DBTransaction> retrieveByAccountID(pfm_id_t accountId);
        DBResult<DBTransaction> retrieveByAccountID(pfm_id_t accountId, db_sort_t dateSortDirection, int rowLimit);
        DBResult<DBTransaction> retrieveByRecurringChargeID(pfm_id_t recurringChargeId);
        DBResult<DBTransaction> retrieveByAccountIDBetweenDates(pfm_id_t accountId, StrDate & firstDate, StrDate & secondDate);
        DBResult<DBTransaction> findTransactionsForAccountID(pfm_id_t accountId, string & criteria);
};

#endif

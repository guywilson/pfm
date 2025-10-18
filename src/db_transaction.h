#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdint.h>

#include <sqlcipher/sqlite3.h>

#include "db_category.h"
#include "db_payee.h"
#include "db_recurring_charge.h"
#include "db_v_recurring_charge.h"
#include "db_payment.h"
#include "db.h"
#include "db_base.h"
#include "jfile.h"
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
                        "WHERE account_id = %s;";

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
                        "WHERE account_id = %s " \
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
                        "WHERE account_id = %s " \
                        "AND date >= '%s' " \
                        "AND date <= '%s';";

        const char * sqlSelectReconciledByAccountID = 
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
                        "WHERE account_id = %s " \
                        "AND is_reconciled = 'Y';";

        const char * sqlSelectReconciledByAccountIDBetweenDates = 
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
                        "WHERE account_id = %s " \
                        "AND is_reconciled = 'Y' " \
                        "AND date >= '%s' " \
                        "AND date <= '%s';";

        const char * sqlSelectNonRecurringByAccountIDBetweenDates = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "category_id," \
                        "payee_id," \
                        "date," \
                        "reference," \
                        "description," \
                        "credit_debit," \
                        "amount," \
                        "is_reconciled," \
                        "created," \
                        "updated " \
                        "FROM account_transaction " \
                        "WHERE account_id = %s " \
                        "AND recurring_charge_id IS NULL " \
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
                        "WHERE recurring_charge_id = %s;";

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
                        "WHERE recurring_charge_id = %s " \
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
                        "VALUES (%s, %s, %s, %s, '%s', '%s', " \
                        "'%s', '%s', '%s', '%s', '%s', '%s');";

        const char * sqlUpdate = 
                        "UPDATE account_transaction " \
                        "SET category_id = %s," \
                        "payee_id = %s," \
                        "recurring_charge_id = %s,"
                        "date = '%s'," \
                        "reference = '%s'," \
                        "description = '%s'," \
                        "credit_debit = '%s'," \
                        "amount = '%s'," \
                        "is_reconciled = '%s'," \
                        "updated = '%s' " \
                        "WHERE id = %s;";

        const char * sqlReconcileByAccountIDBeforeDate = 
                        "UPDATE account_transaction " \
                        "SET is_reconciled = 'Y'," \
                        "updated = '%s' " \
                        "WHERE account_id = %s " \
                        "AND date <= '%s'" \
                        "AND is_reconciled = 'N';";

        const char * sqlDeleteByRecurringCharge = 
                        "DELETE FROM account_transaction WHERE recurring_charge_id = %s;";

        const char * sqlDeleteAllRecurringForAccount = 
                        "DELETE FROM account_transaction WHERE account_id = %s AND recurring_charge_id <> 0;";

        DBResult<DBTransaction> retrieveByStatementAndID(const char * statement, pfm_id_t id);

    public:
        pfm_id_t recurringChargeId;
        string reference;
        bool isCredit;
        bool isReconciled;

        DBTransaction() : DBPayment() {
            clear();
        }

        static const string getCSVHeader() {
            return "accountCode,categoryCode,payeeCode,date,description,reference,creditDebit,isReconciled,amount\n";
        }

        string getCSVRecord() {
            string record = 
                    "\"" + getAccountCode() + "\"," + 
                    "\"" + category.code + "\"," +
                    "\"" + payee.code + "\"," +
                    "\"" + date.shortDate() + "\"," +
                    "\"" + description + "\"," +
                    "\"" + reference + "\"," +
                    "\"" + getCreditDebitValue() + "\"," +
                    "\"" + (isReconciled ? "Y" : "N") + "\"," +
                    "" + amount.rawStringValue() + "\n";

            return record;
        }

        void clear() {
            DBPayment::clear();

            this->recurringChargeId.clear();
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

        void set(JRecord & record) {
            DBPayment::set(record);

            this->reference = record.get("reference");
            this->isCredit = getIsCredit(record.get("creditDebit"));
            this->isReconciled = (record.get("isReconciled").compare("Y") == 0 ? true : false);
        }

        JRecord getRecord() override  {
            JRecord r = DBPayment::getRecord();

            r.add("reference", this->reference);
            r.add("creditDebit", this->getCreditDebitValue());
            r.add("isReconciled", this->getIsReconciledValue());

            return r;
        }

        void setFromRecurringCharge(const DBRecurringCharge & src) {
            DBPayment::set(src);

            this->id.clear();
            this->recurringChargeId = src.id;
        }

        void print() {
            DBPayment::print();

            cout << "RecurringChargeId: " << recurringChargeId.getValue() << endl;
            cout << "Reference: '" << reference << "'" << endl;
            cout << "Debit/Credit: '" << (isCredit ? "CR" : "DB") << "'" << endl;
            cout << "isReconciled: " << isReconciled << endl;
        }

        void assignColumn(DBColumn & column) override {
            DBPayment::assignColumn(column);
            
            if (column.getName() == "recurring_charge_id") {
                recurringChargeId = column.getIDValue();
            }
            else if (column.getName() == "reference") {
                reference = column.getValue();
            }
            else if (column.getName() == "credit_debit") {
                isCredit = getIsCredit(column.getValue());
            }
            else if (column.getName() == "is_reconciled") {
                isReconciled = column.getBoolValue();
            }
        }

        void onRowComplete(int sequence) override {
            if (!categoryId.isNull()) {
                category.retrieve(categoryId);
            }
            if (!payeeId.isNull()) {
                payee.retrieve(payeeId);
            }

            this->sequence = sequence;
        }

        string getCreditDebitValue() {
            return (isCredit ? "CR" : "DB");
        }

        bool getIsCredit(const string & value) {
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

        const char * getClassName() override {
            return "DBTransaction";
        }

        const char * getInsertStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            string dDescription = delimitSingleQuotes(description);
            string dReference = delimitSingleQuotes(reference);

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlInsert,
                accountId.c_str(),
                categoryId.c_str(),
                payeeId.c_str(),
                recurringChargeId.c_str(),
                date.shortDate().c_str(),
                dReference.c_str(),
                dDescription.c_str(),
                getCreditDebitValue().c_str(),
                amount.rawStringValue().c_str(),
                getIsReconciledValue(),
                now.c_str(),
                now.c_str());

            return szStatement;
        }

        const char * getUpdateStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            string dDescription = delimitSingleQuotes(description);
            string dReference = delimitSingleQuotes(reference);

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlUpdate,
                categoryId.c_str(),
                payeeId.c_str(),
                recurringChargeId.c_str(),
                date.shortDate().c_str(),
                dReference.c_str(),
                dDescription.c_str(),
                getCreditDebitValue().c_str(),
                amount.rawStringValue().c_str(),
                getIsReconciledValue(),
                now.c_str(),
                id.c_str());

            return szStatement;
        }

        void afterInsert() override;
        void beforeUpdate() override;
        void afterRemove() override;

        int findLatestByRecurringChargeID(pfm_id_t chargeId);
        int createNextTransactionForCharge(DBRecurringCharge & charge, StrDate & latestDate);

        void deleteByRecurringChargeId(pfm_id_t recurringChargeId);
        void deleteAllRecurringTransactionsForAccount(pfm_id_t accountId);

        void reconcileAllForAccountIDBeforeDate(pfm_id_t accountId, StrDate & referenceDate);

        static void createFromRecurringChargeAndDate(const DBRecurringCharge & src, StrDate & transactionDate);

        DBResult<DBTransaction> retrieveByAccountID(pfm_id_t accountId);
        DBResult<DBTransaction> retrieveByAccountID(pfm_id_t accountId, db_sort_t dateSortDirection, int rowLimit);
        DBResult<DBTransaction> retrieveReconciledByAccountID(pfm_id_t accountId);
        DBResult<DBTransaction> retrieveByRecurringChargeID(pfm_id_t recurringChargeId);
        DBResult<DBTransaction> retrieveByAccountIDForPeriod(pfm_id_t accountId, StrDate & firstDate, StrDate & secondDate);
        DBResult<DBTransaction> retrieveReconciledByAccountIDForPeriod(pfm_id_t accountId, StrDate & firstDate, StrDate & secondDate);
        DBResult<DBTransaction> retrieveNonRecurringByAccountIDForPeriod(pfm_id_t accountId, StrDate & firstDate, StrDate & secondDate);
        DBResult<DBTransaction> findTransactionsForAccountID(pfm_id_t accountId, string & criteria);
};

#endif

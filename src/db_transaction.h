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

#define TYPE_CREDIT         "CR"
#define TYPE_DEBIT          "DB"

class DBTransaction : public DBPayment {
    private:
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

    protected:
        struct Columns {
            static constexpr const char * recurringChargeId = "recurring_charge_id";
            static constexpr const char * reference = "reference";
            static constexpr const char * type = "credit_debit";
            static constexpr const char * isReconciled = "is_reconciled";
        };

    public:
        pfm_id_t recurringChargeId;
        string reference;
        string type;
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
                    "\"" + type + "\"," +
                    "\"" + (isReconciled ? "Y" : "N") + "\"," +
                    "" + amount.rawStringValue() + "\n";

            return record;
        }

        void clear() {
            DBPayment::clear();

            this->recurringChargeId.clear();
            this->reference = "";
            this->type = TYPE_DEBIT;
            this->isReconciled = false;
        }

        void set(const DBTransaction & src) {
            DBPayment::set(src);

            this->recurringChargeId = src.recurringChargeId;
            this->reference = src.reference;
            this->type = src.type;
            this->isReconciled = src.isReconciled;
        }

        void set(JRecord & record) {
            DBPayment::set(record);

            this->reference = record.get("reference");
            this->type = record.get("type");
            this->isReconciled = (record.get("isReconciled").compare("Y") == 0 ? true : false);
        }

        JRecord getRecord() override  {
            JRecord r = DBPayment::getRecord();

            r.add("reference", reference);
            r.add("type", type);
            r.add("isReconciled", getIsReconciledValue());

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
            cout << "Type: '" << type << "'" << endl;
            cout << "isReconciled: " << isReconciled << endl;
        }

        void assignColumn(DBColumn & column) override {
            DBPayment::assignColumn(column);
            
            if (column.getName() == Columns::recurringChargeId) {
                recurringChargeId = column.getIDValue();
            }
            else if (column.getName() == Columns::reference) {
                reference = column.getValue();
            }
            else if (column.getName() == Columns::type) {
                type = column.getValue();
            }
            else if (column.getName() == Columns::isReconciled) {
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

        bool isCredit() const {
            if (type == TYPE_CREDIT) {
                return true;
            }
            else if (type == TYPE_DEBIT) {
                return false;
            }
            else {
                return false;
            }
        }

        Money getSignedAmount() {
            return (isCredit() ? amount : (amount * -1));
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

        const string getInsertStatement() override {
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
                type.c_str(),
                amount.rawStringValue().c_str(),
                getIsReconciledValue(),
                now.c_str(),
                now.c_str());

            return string(szStatement);
        }

        const string getUpdateStatement() override {
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
                type.c_str(),
                amount.rawStringValue().c_str(),
                getIsReconciledValue(),
                now.c_str(),
                id.c_str());

            return string(szStatement);
        }

        void afterInsert() override;
        void beforeUpdate() override;
        void afterRemove() override;

        int findLatestByRecurringChargeID(pfm_id_t & chargeId);
        int createNextTransactionForCharge(DBRecurringCharge & charge, StrDate & latestDate);

        void deleteByRecurringChargeId(pfm_id_t & recurringChargeId);
        void deleteAllRecurringTransactionsForAccount(pfm_id_t & accountId);

        void reconcileAllForAccountIDBeforeDate(pfm_id_t & accountId, StrDate & referenceDate);

        static void createFromRecurringChargeAndDate(DBRecurringCharge & src, StrDate & transactionDate);

        static void createTransferPairFromSource(DBTransaction & source, DBAccount & accountTo);

        DBResult<DBTransaction> retrieveByAccountID(pfm_id_t & accountId);
        DBResult<DBTransaction> retrieveByAccountID(pfm_id_t & accountId, DBCriteria::sql_order dateSortDirection, int rowLimit);
        DBResult<DBTransaction> retrieveReconciledByAccountID(pfm_id_t & accountId);
        DBResult<DBTransaction> retrieveByRecurringChargeID(pfm_id_t & recurringChargeId);
        DBResult<DBTransaction> retrieveByAccountIDForPeriod(pfm_id_t & accountId, StrDate & firstDate, StrDate & secondDate);
        DBResult<DBTransaction> retrieveReconciledByAccountIDForPeriod(pfm_id_t & accountId, StrDate & firstDate, StrDate & secondDate);
        DBResult<DBTransaction> retrieveNonRecurringByAccountIDForPeriod(pfm_id_t & accountId, StrDate & firstDate, StrDate & secondDate);
        DBResult<DBTransaction> findTransactionsForAccountID(pfm_id_t & accountId, string & criteria);
};

#endif

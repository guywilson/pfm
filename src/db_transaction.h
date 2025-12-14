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
    protected:
        struct Columns {
            static constexpr const char * recurringChargeId = "recurring_charge_id";
            static constexpr ColumnType recurringChargeId_type = ColumnType::ID;

            static constexpr const char * reference = "reference";
            static constexpr ColumnType reference_type = ColumnType::TEXT;

            static constexpr const char * type = "credit_debit";
            static constexpr ColumnType type_type = ColumnType::TEXT;

            static constexpr const char * isReconciled = "is_reconciled";
            static constexpr ColumnType isReconciled_type = ColumnType::BOOL;
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
            return "accountCode,categoryCode,payeeCode,date,description,reference,creditDebit,isReconciled,isTransfer,amount\n";
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
                    "\"" + (isTransfer ? "Y" : "N") + "\"," +
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
            this->isReconciled = record.getBoolValue("isReconciled");
        }

        JRecord getRecord() override  {
            JRecord r = DBPayment::getRecord();

            r.add("reference", reference);
            r.add("type", type);
            r.add("isReconciled", isReconciled);

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

        const string getTableName() const override {
            return "account_transaction";
        }

        const string getClassName() const override {
            return "DBTransaction";
        }

        const string getInsertStatement() override {
            vector<pair<ColumnDef, string>> columnValuePairs = {
                {{DBPayment::Columns::accountId, DBPayment::Columns::accountId_type}, accountId.getValue()},
                {{DBPayment::Columns::categoryId, DBPayment::Columns::categoryId_type}, categoryId.getValue()},
                {{DBPayment::Columns::payeeId, DBPayment::Columns::payeeId_type}, payeeId.getValue()},
                {{DBPayment::Columns::date, DBPayment::Columns::date_type}, date.shortDate()},
                {{Columns::recurringChargeId, Columns::recurringChargeId_type}, recurringChargeId.getValue()},
                {{Columns::reference, Columns::reference_type}, delimitSingleQuotes(reference)},
                {{DBPayment::Columns::description, DBPayment::Columns::description_type}, delimitSingleQuotes(description)},
                {{Columns::type, Columns::type_type}, type},
                {{DBPayment::Columns::amount, DBPayment::Columns::amount_type}, amount.rawStringValue()},
                {{Columns::isReconciled, Columns::isReconciled_type}, getIsReconciledValue()},
                {{DBPayment::Columns::isTransfer, DBPayment::Columns::isTransfer_type}, getIsTransferValue()}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const string getUpdateStatement() override {
            vector<pair<ColumnDef, string>> columnValuePairs = {
                {{DBPayment::Columns::categoryId, DBPayment::Columns::categoryId_type}, categoryId.getValue()},
                {{DBPayment::Columns::payeeId, DBPayment::Columns::payeeId_type}, payeeId.getValue()},
                {{DBPayment::Columns::date, DBPayment::Columns::date_type}, date.shortDate()},
                {{Columns::recurringChargeId, Columns::recurringChargeId_type}, recurringChargeId.getValue()},
                {{Columns::reference, Columns::reference_type}, delimitSingleQuotes(reference)},
                {{DBPayment::Columns::description, DBPayment::Columns::description_type}, delimitSingleQuotes(description)},
                {{Columns::type, Columns::type_type}, type},
                {{DBPayment::Columns::amount, DBPayment::Columns::amount_type}, amount.rawStringValue()},
                {{Columns::isReconciled, Columns::isReconciled_type}, getIsReconciledValue()},
                {{DBPayment::Columns::isTransfer, DBPayment::Columns::isTransfer_type}, getIsTransferValue()}
            };

            return buildUpdateStatement(getTableName(), columnValuePairs);
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
        static void linkTransferTransactions();

        DBResult<DBTransaction> retrieveByAccountID(pfm_id_t & accountId);
        DBResult<DBTransaction> retrieveByAccountID(pfm_id_t & accountId, DBCriteria::sql_order dateSortDirection, int rowLimit);
        DBResult<DBTransaction> retrieveReconciledByAccountID(pfm_id_t & accountId);
        DBResult<DBTransaction> retrieveByRecurringChargeID(pfm_id_t & recurringChargeId);
        DBResult<DBTransaction> retrieveByRecurringChargeIDAfterDate(pfm_id_t & recurringChargeId, StrDate & after);
        DBResult<DBTransaction> retrieveByAccountIDForPeriod(pfm_id_t & accountId, StrDate & firstDate, StrDate & secondDate);
        DBResult<DBTransaction> retrieveReconciledByAccountIDForPeriod(pfm_id_t & accountId, StrDate & firstDate, StrDate & secondDate);
        DBResult<DBTransaction> retrieveNonRecurringByAccountIDForPeriod(pfm_id_t & accountId, StrDate & firstDate, StrDate & secondDate);
        DBResult<DBTransaction> findTransactionsForAccountID(pfm_id_t & accountId, string & criteria);
};

#endif

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdint.h>

#include <sqlite3.h>

#include "db_category.h"
#include "db_payee.h"
#include "db_payment.h"
#include "db.h"
#include "db_base.h"
#include "jfile.h"
#include "strdate.h"

using namespace std;

#ifndef __INCL_RECURRING_CHARGE
#define __INCL_RECURRING_CHARGE

#define CHARGE_OK                   0
#define CHARGE_NOT_DUE              -1

class DBRecurringCharge : public DBPayment {
    private:
        const char * sqlSelectByAccountID = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "category_id," \
                        "payee_id," \
                        "date," \
                        "end_date," \
                        "description," \
                        "amount," \
                        "frequency," \
                        "created," \
                        "updated " \
                        "FROM recurring_charge " \
                        "WHERE account_id = %lld;";

        const char * sqlInsert = 
                        "INSERT INTO recurring_charge (" \
                        "account_id," \
                        "category_id," \
                        "payee_id," \
                        "date," \
                        "end_date," \
                        "description," \
                        "amount," \
                        "frequency," \
                        "created," \
                        "updated) " \
                        "VALUES (%lld, %lld, %lld, '%s', '%s'," \
                        "'%s', '%s', '%s', '%s', '%s');";

        const char * sqlUpdate = 
                        "UPDATE recurring_charge " \
                        "SET category_id = %lld," \
                        "payee_id = %lld," \
                        "date = '%s'," \
                        "end_date = '%s'," \
                        "description = '%s'," \
                        "amount = '%s'," \
                        "frequency = '%s'," \
                        "updated = '%s' " \
                        "WHERE id = %lld;";

        const char * sqlDelete = 
                        "DELETE FROM recurring_charge WHERE id = %lld;";

        bool isDateWithinCurrentPeriod(StrDate & date);

    public:
        StrDate nextPaymentDate;    // Not persistent

        string frequency;
        StrDate endDate;

        DBRecurringCharge() : DBPayment() {
            clear();
        }

        DBRecurringCharge(const DBRecurringCharge & src) : DBPayment(src) {
            set(src);
        }

        void clear() {
            DBPayment::clear();

            this->frequency = "";
            this->nextPaymentDate.clear();
            this->endDate.clear();
        }

        void set(const DBRecurringCharge & src) {
            DBPayment::set(src);

            this->nextPaymentDate = src.nextPaymentDate;
            this->frequency = src.frequency;
            this->endDate = src.endDate;
        }

        void set(JRecord & record) {
            DBPayment::set(record);

            this->frequency = record.get("frequency");
            this->endDate = record.get("endDate");
        }

        void print() {
            DBPayment::print();

            cout << "Frequency: '" << frequency << "'" << endl;
            cout << "EndDate: '" << endDate.shortDate() << "'" << endl;
            cout << "NextPaymentDate: '" << nextPaymentDate.shortDate() << "'" << endl;
        }

        void assignColumn(DBColumn & column) override {
            DBPayment::assignColumn(column);
            
            if (column.getName() == "end_date") {
                endDate = column.getDecryptedValue();
            }
            else if (column.getName() == "frequency") {
                frequency = column.getDecryptedValue();
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
            
            setNextPaymentDate();
        }

        int getFrequencyValue();
        char getFrequencyUnit();

        bool isChargeDueThisPeriod();
        StrDate calculateNextPaymentDate();
        void setNextPaymentDate();

        const char * getTableName() override {
            return "recurring_charge";
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
                date.shortDate().c_str(),
                encryptField(endDate.shortDate()).c_str(),
                encryptField(description).c_str(),
                encryptField(amount.getRawStringValue()).c_str(),
                encryptField(frequency).c_str(),
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
                date.shortDate().c_str(),
                encryptField(endDate.shortDate()).c_str(),
                encryptField(description).c_str(),
                encryptField(amount.getRawStringValue()).c_str(),
                encryptField(frequency).c_str(),
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

        void validate() override;
        DBResult<DBRecurringCharge> retrieveByAccountID(pfm_id_t accountId);
};

#endif

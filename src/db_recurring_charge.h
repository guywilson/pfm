#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdint.h>

#include <sqlite3.h>

#include "db_category.h"
#include "db_payee.h"
#include "db_payment.h"
#include "strdate.h"
#include "db.h"
#include "db_base.h"
#include "strdate.h"

using namespace std;

#ifndef __INCL_RECURRING_CHARGE
#define __INCL_RECURRING_CHARGE

#define CHARGE_OK                   0
#define CHARGE_NOT_DUE              -1

class DBRecurringChargeResult;

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

        const char * sqlSelectAll = 
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
                        "FROM recurring_charge;";

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
                        "'%s', %s, '%s', '%s', '%s');";

        const char * sqlUpdate = 
                        "UPDATE recurring_charge " \
                        "SET category_id = %lld," \
                        "payee_id = %lld," \
                        "date = '%s'," \
                        "end_date = '%s'," \
                        "description = '%s'," \
                        "amount = %s," \
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

        void print() {
            DBPayment::print();

            cout << "Frequency: '" << frequency << "'" << endl;
            cout << "EndDate: '" << endDate.shortDate() << "'" << endl;
            cout << "NextPaymentDate: '" << nextPaymentDate.shortDate() << "'" << endl;
        }

        int getFrequencyValue();
        char getFrequencyUnit();

        bool isChargeDueThisPeriod();
        StrDate calculateNextPaymentDate();
        void setNextPaymentDate();
        int createNextTransactionForCharge(StrDate & latestDate);

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
                endDate.shortDate().c_str(),
                description.c_str(),
                amount.getRawStringValue().c_str(),
                frequency.c_str(),
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
                endDate.shortDate().c_str(),
                description.c_str(),
                amount.getRawStringValue().c_str(),
                frequency.c_str(),
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

        DBRecurringChargeResult retrieveByAccountID(pfm_id_t accountId);
        DBRecurringChargeResult retrieveAll();
};

class DBRecurringChargeResult : public DBResult {
    private:
        vector<DBRecurringCharge> results;

    public:
        DBRecurringChargeResult() : DBResult() {}

        void clear() {
            DBResult::clear();
            results.clear();
        }

        DBRecurringCharge getResultAt(int i) {
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
            DBRecurringCharge charge;

            for (size_t i = 0;i < row.getNumColumns();i++) {
                DBColumn column = row.getColumnAt(i);

                if (column.getName() == "id") {
                    charge.id = column.getIDValue();
                }
                else if (column.getName() == "account_id") {
                    charge.accountId = column.getIDValue();
                }
                else if (column.getName() == "category_id") {
                    charge.categoryId = column.getIDValue();
                }
                else if (column.getName() == "payee_id") {
                    charge.payeeId = column.getIDValue();
                }
                else if (column.getName() == "date") {
                    charge.date = column.getValue();
                }
                else if (column.getName() == "end_date") {
                    charge.endDate = column.getValue();
                }
                else if (column.getName() == "description") {
                    charge.description = column.getValue();
                }
                else if (column.getName() == "amount") {
                    charge.amount = column.getDoubleValue();
                }
                else if (column.getName() == "frequency") {
                    charge.frequency = column.getValue();
                }
                else if (column.getName() == "created") {
                    charge.createdDate = column.getValue();
                }
                else if (column.getName() == "updated") {
                    charge.updatedDate = column.getValue();
                }
            }
            
            charge.category.id = charge.categoryId;

            DBCategoryResult categoryResult;
            charge.category.retrieveByID(&categoryResult);
            charge.category.set(categoryResult.getResultAt(0));

            charge.payee.id = charge.payeeId;

            DBPayeeResult payeeResult;
            charge.payee.retrieveByID(&payeeResult);
            charge.payee.set(payeeResult.getResultAt(0));
            
            charge.sequence = sequenceCounter++;
            
            charge.setNextPaymentDate();
            
            results.push_back(charge);
            incrementNumRows();
        }
};

#endif

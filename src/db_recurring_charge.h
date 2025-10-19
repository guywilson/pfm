#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdint.h>

#include <sqlcipher/sqlite3.h>

#include "db_category.h"
#include "db_payee.h"
#include "db_payment.h"
#include "db_recurring_transfer.h"
#include "db.h"
#include "db_base.h"
#include "jfile.h"
#include "strdate.h"

using namespace std;

#ifndef __INCL_RECURRING_CHARGE
#define __INCL_RECURRING_CHARGE

#define CHARGE_OK                   0
#define CHARGE_NOT_DUE              -1

enum class FrequencyUnit { Days, Weeks, Months, Years };

struct Frequency {
    string _fstr;

    int count;
    FrequencyUnit unit;

    const char * c_str() {
        return _fstr.c_str();
    }

    string toString() {
        return _fstr;
    }

    void set(const string & freqStr) {
        _fstr = freqStr;
    }

    static Frequency parse(const string & freqStr) {
        if (freqStr.empty()) {
            throw std::invalid_argument("Empty frequency string");
        }

        int count = 0;
        size_t i = 0;

        while (i < freqStr.size() && isdigit(freqStr[i])) {
            count = count * 10 + (freqStr[i] - '0');
            ++i;
        }

        if (count <= 0 || i >= freqStr.size()) {
            throw std::invalid_argument("Invalid frequency format: " + freqStr);
        }

        char unitChar = freqStr[i];

        switch (unitChar) {
            case 'd': 
                return { freqStr, count, FrequencyUnit::Days };

            case 'w': 
                return { freqStr, count, FrequencyUnit::Weeks };

            case 'm': 
                return { freqStr, count, FrequencyUnit::Months };

            case 'y': 
                return { freqStr, count, FrequencyUnit::Years };

            default: 
                throw std::invalid_argument("Unknown frequency unit: " + std::string(1, unitChar));
        }
    }
};

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
                        "last_payment_date," \
                        "frequency," \
                        "created," \
                        "updated " \
                        "FROM recurring_charge " \
                        "WHERE account_id = %s;";

        const char * sqlSelectByAccountIDBetweenDates = 
                        "SELECT " \
                        "id," \
                        "account_id," \
                        "category_id," \
                        "payee_id," \
                        "date," \
                        "end_date," \
                        "description," \
                        "amount," \
                        "last_payment_date," \
                        "frequency," \
                        "created," \
                        "updated " \
                        "FROM recurring_charge " \
                        "WHERE account_id = %s " \
                        "AND date >= '%s' " \
                        "AND date < '%s';";

        const char * sqlInsert = 
                        "INSERT INTO recurring_charge (" \
                        "account_id," \
                        "category_id," \
                        "payee_id," \
                        "date," \
                        "end_date," \
                        "description," \
                        "amount," \
                        "last_payment_date," \
                        "frequency," \
                        "created," \
                        "updated) " \
                        "VALUES (%s, %s, %s, '%s', '%s'," \
                        "'%s', '%s', '%s', '%s', '%s', '%s');";

        const char * sqlUpdate = 
                        "UPDATE recurring_charge " \
                        "SET category_id = %s," \
                        "payee_id = %s," \
                        "date = '%s'," \
                        "end_date = '%s'," \
                        "description = '%s'," \
                        "amount = '%s'," \
                        "last_payment_date = '%s'," \
                        "frequency = '%s'," \
                        "updated = '%s' " \
                        "WHERE id = %s;";

        bool isDateWithinCurrentPeriod(StrDate & date);

        int getPeriodStartDay();
        int getPeriodEndDay();
        int getPeriodEndDay(StrDate & referenceDate);

        // Step forward by this charge's frequency (no weekend adjustment).
        StrDate nextByFrequency(StrDate & from);

        // Next *scheduled* (nominal) date, without weekend adjustment.
        // Starts strictly after `from` to avoid infinite loops on equal dates.
        StrDate nextScheduledNoWeekend(StrDate & from);

        // Move forward to Monday if the given day is a weekend.
        static StrDate adjustForwardToBusinessDay(StrDate & d);

    public:
        DBRecurringTransfer transfer;

        StrDate lastPaymentDate;
        Frequency frequency;
        StrDate endDate;

        DBRecurringCharge() : DBPayment() {
            clear();
        }

        DBRecurringCharge(const DBRecurringCharge & src) : DBPayment(src) {
            set(src);
        }

        ~DBRecurringCharge() {
            clear();
        }

        static const string getCSVHeader() {
            return "accountCode,categoryCode,payeeCode,date,endDate,description,frequency,amount\n";
        }

        string getCSVRecord() {
            string record = 
                    "\"" + getAccountCode() + "\"," + 
                    "\"" + category.code + "\"," +
                    "\"" + payee.code + "\"," +
                    "\"" + date.shortDate() + "\"," +
                    "\"" + endDate.shortDate() + "\"," +
                    "\"" + description + "\"," +
                    "\"" + lastPaymentDate.shortDate() + "\"," +
                    "\"" + frequency.toString() + "\"," +
                    "" + amount.rawStringValue() + "\n";

            return record;
        }

        void clear() {
            DBPayment::clear();

            this->transfer.clear();
            this->lastPaymentDate.clear();
            this->frequency.set("");
            this->endDate.clear();
        }

        void set(const DBRecurringCharge & src) {
            DBPayment::set(src);

            this->transfer = src.transfer;
            this->lastPaymentDate = src.lastPaymentDate;
            this->frequency = src.frequency;
            this->endDate = src.endDate;
        }

        void set(JRecord & record) {
            DBPayment::set(record);

            this->lastPaymentDate = record.get("lastPaymentDate");
            this->frequency = Frequency::parse(record.get("frequency"));
            this->endDate = record.get("endDate");
        }

        JRecord getRecord() override  {
            JRecord r = DBPayment::getRecord();

            r.add("lastPaymentDate", this->lastPaymentDate.shortDate());
            r.add("endDate", this->endDate.shortDate());
            r.add("frequency", this->frequency.toString());

            return r;
        }

        void print() {
            DBPayment::print();

            cout << "LastPaymentDate: '" << lastPaymentDate.shortDate() << "'" << endl;
            cout << "Frequency: '" << frequency.toString() << "'" << endl;
            cout << "EndDate: '" << endDate.shortDate() << "'" << endl;
        }

        void assignColumn(DBColumn & column) override {
            DBPayment::assignColumn(column);
            
            if (column.getName() == "end_date") {
                endDate = column.getValue();
            }
            else if (column.getName() == "frequency") {
                frequency = Frequency::parse(column.getValue());
            }
            else if (column.getName() == "last_payment_date") {
                lastPaymentDate = column.getValue();
            }
        }

        void onRowComplete(int sequence) override {
            if (!categoryId.isNull()) {
                category.retrieve(categoryId);
            }
            if (!payeeId.isNull()) {
                payee.retrieve(payeeId);
            }

            transfer.retrieveByRecurringChargeId(id);

            this->sequence = sequence;
        }

        bool isTransfer() {
            return (!transfer.isNull() ? true : false);
        }

        bool isActive();

        void updateLastPaymentDate(StrDate & date);

        void beforeRemove() override;
        void beforeUpdate() override;
        void afterInsert() override;

        // Expose a nominal next date helper if you want to inspect schedules elsewhere.
        StrDate getNextRecurringScheduledDate(StrDate & startDate);

        bool isChargeDueThisPeriod();
        bool isChargeDueThisPeriod(StrDate & referenceDate);
        StrDate calculateNextPaymentDate();
        StrDate getNextRecurringTransactionDate(StrDate & startDate);

        void migrateToTransferCharge(pfm_id_t & accountToId);

        const char * getTableName() override {
            return "recurring_charge";
        }

        const char * getClassName() override {
            return "DBRecurringCharge";
        }

        const char * getInsertStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            string dDescription = delimitSingleQuotes(description);

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlInsert,
                accountId.c_str(),
                categoryId.c_str(),
                payeeId.c_str(),
                date.shortDate().c_str(),
                endDate.shortDate().c_str(),
                dDescription.c_str(),
                amount.rawStringValue().c_str(),
                lastPaymentDate.shortDate().c_str(),
                frequency.c_str(),
                now.c_str(),
                now.c_str());

            return szStatement;
        }

        const char * getUpdateStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            string dDescription = delimitSingleQuotes(description);

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlUpdate,
                categoryId.c_str(),
                payeeId.c_str(),
                date.shortDate().c_str(),
                endDate.shortDate().c_str(),
                dDescription.c_str(),
                amount.rawStringValue().c_str(),
                lastPaymentDate.shortDate().c_str(),
                frequency.c_str(),
                now.c_str(),
                id.c_str());

            return szStatement;
        }

        DBResult<DBRecurringCharge> retrieveByAccountID(pfm_id_t & accountId);
        DBResult<DBRecurringCharge> retrieveByAccountIDBetweenDates(pfm_id_t & accountId, StrDate & dateAfter, StrDate & dateBefore);
};

#endif

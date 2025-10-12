#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdint.h>

#include <sqlcipher/sqlite3.h>

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
                        "WHERE account_id = %lld;";

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
                        "WHERE account_id = %lld " \
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
                        "VALUES (%lld, %lld, %lld, '%s', '%s'," \
                        "'%s', '%s', '%s', '%s', '%s', '%s');";

        const char * sqlUpdate = 
                        "UPDATE recurring_charge " \
                        "SET category_id = %lld," \
                        "payee_id = %lld," \
                        "date = '%s'," \
                        "end_date = '%s'," \
                        "description = '%s'," \
                        "amount = '%s'," \
                        "last_payment_date = '%s'," \
                        "frequency = '%s'," \
                        "updated = '%s' " \
                        "WHERE id = %lld;";

        bool isDateWithinCurrentPeriod(StrDate & date);

        int getPeriodStartDay();
        int getPeriodEndDay();
        int getPeriodEndDay(StrDate & referenceDate);

    public:
        StrDate nextPaymentDate;    // Not persistent

        StrDate lastPaymentDate;
        Frequency frequency;
        StrDate endDate;

        DBRecurringCharge() : DBPayment() {
            clear();
        }

        DBRecurringCharge(const DBRecurringCharge & src) : DBPayment(src) {
            set(src);
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

            this->nextPaymentDate.clear();
            this->lastPaymentDate.clear();
            this->frequency.set("");
            this->endDate.clear();
        }

        void set(const DBRecurringCharge & src) {
            DBPayment::set(src);

            this->nextPaymentDate = src.nextPaymentDate;
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
            cout << "NextPaymentDate: '" << nextPaymentDate.shortDate() << "'" << endl;
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
            if (categoryId != 0) {
                category.retrieve(categoryId);
            }
            if (payeeId != 0) {
                payee.retrieve(payeeId);
            }

            this->sequence = sequence;
            
            setNextPaymentDate();
        }

        bool isActive() {
            StrDate today;

            return (endDate.isNull() || (!endDate.isNull() && endDate >= today));
        }

        void beforeRemove() override;
        void beforeUpdate() override;
        
        bool isChargeDueThisPeriod();
        bool isChargeDueThisPeriod(StrDate & referenceDate);
        StrDate calculateNextPaymentDate();
        void setNextPaymentDate();
        StrDate getNextRecurringTransactionDate(StrDate & startDate);

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
                accountId,
                categoryId,
                payeeId,
                date.shortDate().c_str(),
                endDate.shortDate().c_str(),
                dDescription.c_str(),
                amount.rawStringValue().c_str(),
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
                categoryId,
                payeeId,
                date.shortDate().c_str(),
                endDate.shortDate().c_str(),
                dDescription.c_str(),
                amount.rawStringValue().c_str(),
                frequency.c_str(),
                now.c_str(),
                id);

            return szStatement;
        }

        DBResult<DBRecurringCharge> retrieveByAccountID(pfm_id_t accountId);
        DBResult<DBRecurringCharge> retrieveByAccountIDBetweenDates(pfm_id_t accountId, StrDate & dateAfter, StrDate & dateBefore);
};

#endif

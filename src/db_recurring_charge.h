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
        bool inline isNumeric(string & cfgDate) {
            bool isNumeric = true;
            for (int i = 0;i < (int)cfgDate.length();i++) {
                if (!isdigit(cfgDate.at(i))) {
                    isNumeric = false;
                    break;
                }
            }

            return isNumeric;
        }

        StrDate inline advanceDateByFrequency(StrDate & from) {
            StrDate d = from;
            const int n = frequency.count;
            const FrequencyUnit u = frequency.unit;

            switch (u) {
                case FrequencyUnit::Years:
                    return d.addYears(n);

                case FrequencyUnit::Months:
                    return d.addMonths(n);

                case FrequencyUnit::Weeks:
                    return d.addWeeks(n);

                case FrequencyUnit::Days:
                    return d.addDays(n);

                default:
                    throw pfm_error(
                            "Unknown frequency unit when stepping date",
                            __FILE__, 
                            __LINE__);
            }
        }
 
        // Move forward to Monday if the given day is a weekend.
        StrDate inline adjustForwardToBusinessDay(StrDate& d) {
            StrDate x = d;

            if (x.isSaturday()) {
                return x.addDays(2);
            }
            else if (x.isSunday()) {
            return x.addDays(1);
            }

            return x;
        }

        StrDate inline getPeriodStartDate() {
            StrDate today;
            return getPeriodStartDate(today);
        }

        StrDate inline getPeriodEndDate() {
            StrDate today;
            return getPeriodEndDate(today);
        }

        StrDate inline getPeriodStartDate(StrDate & referenceDate) {
            const int periodStartDay = getPeriodStartDay();
            StrDate periodStart(referenceDate.year(), referenceDate.month(), periodStartDay);

            return periodStart;
        }

        StrDate inline getPeriodEndDate(StrDate & referenceDate) {
            const int periodEndDay = getPeriodEndDay(referenceDate);
            StrDate periodEnd(referenceDate.year(), referenceDate.month(), periodEndDay);

            return periodEnd;
        }

        int getPeriodStartDay();
        int getPeriodEndDay();
        int getPeriodEndDay(StrDate & referenceDate);

        // Next *scheduled* (nominal) date, without weekend adjustment.
        StrDate getNextScheduledDate();

    protected:
        struct Columns {
            static constexpr const char * lastPaymentDate = "last_payment_date";
            static constexpr const char * frequency = "frequency";
            static constexpr const char * endDate = "end_date";
        };

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
            
            if (column.getName() == Columns::endDate) {
                endDate = column.getValue();
            }
            else if (column.getName() == Columns::frequency) {
                frequency = Frequency::parse(column.getValue());
            }
            else if (column.getName() == Columns::lastPaymentDate) {
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

        bool inline isActive() {
            StrDate today;
            return (endDate.isNull() || (!endDate.isNull() && endDate >= today));
        }

        void inline updateLastPaymentDate(StrDate & date) {
            DBRecurringCharge rc;
            rc.id = this->id;

            rc.retrieve();
            rc.lastPaymentDate = date;
            rc.save();
        }

        void beforeRemove() override;
        void beforeUpdate() override;
        void afterInsert() override;

        bool isWithinCurrentPeriod(StrDate & referenceDate);
        bool isChargeDueThisPeriod();

        StrDate getNextRecurringTransactionDate(StrDate & startDate);

        void migrateToTransferCharge(pfm_id_t & accountToId);

        const string getTableName() const override {
            return "recurring_charge";
        }

        const string getClassName() const override {
            return "DBRecurringCharge";
        }

        const string getInsertStatement() override {
            vector<pair<string, string>> columnValuePairs = {
                {DBPayment::Columns::accountId, accountId.getValue()},
                {DBPayment::Columns::categoryId, categoryId.getValue()},
                {DBPayment::Columns::payeeId, payeeId.getValue()},
                {DBPayment::Columns::date, date.shortDate()},
                {Columns::endDate, endDate.shortDate()},
                {DBPayment::Columns::description, delimitSingleQuotes(description)},
                {DBPayment::Columns::amount, amount.rawStringValue()},
                {Columns::lastPaymentDate, lastPaymentDate.shortDate()},
                {Columns::frequency, frequency.toString()}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const string getUpdateStatement() override {
            vector<pair<string, string>> columnValuePairs = {
                {DBPayment::Columns::categoryId, categoryId.getValue()},
                {DBPayment::Columns::payeeId, payeeId.getValue()},
                {DBPayment::Columns::date, date.shortDate()},
                {Columns::endDate, endDate.shortDate()},
                {DBPayment::Columns::description, delimitSingleQuotes(description)},
                {DBPayment::Columns::amount, amount.rawStringValue()},
                {Columns::lastPaymentDate, lastPaymentDate.shortDate()},
                {Columns::frequency, frequency.toString()}
            };

            return buildUpdateStatement(getTableName(), columnValuePairs);
        }

        DBResult<DBRecurringCharge> retrieveByAccountID(pfm_id_t & accountId);
        DBResult<DBRecurringCharge> retrieveByAccountIDBetweenDates(pfm_id_t & accountId, StrDate & dateAfter, StrDate & dateBefore);
};

#endif

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db_transaction.h"
#include "db_recurring_charge.h"
#include "db.h"
#include "logger.h"
#include "strdate.h"

using namespace std;

DBResult<DBRecurringCharge> DBRecurringCharge::retrieveByAccountID(pfm_id_t accountId) {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBRecurringCharge> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByAccountID, 
        accountId);

    result.retrieve(szStatement);

    return result;
}

DBResult<DBRecurringCharge> DBRecurringCharge::retrieveByAccountIDBetweenDates(pfm_id_t accountId, StrDate & dateAfter, StrDate & dateBefore) {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBRecurringCharge> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByAccountIDBetweenDates, 
        accountId,
        dateAfter.shortDate().c_str(),
        dateBefore.shortDate().c_str());

    result.retrieve(szStatement);

    return result;
}

int DBRecurringCharge::getFrequencyValue() {
    return atoi(frequency.substr(0, frequency.length() - 1).c_str());
}

char DBRecurringCharge::getFrequencyUnit() {
    return frequency.substr(frequency.length() - 1, 1).c_str()[0];
}

bool DBRecurringCharge::isDateWithinCurrentPeriod(StrDate & date) {
    StrDate dateToday;
    int periodStartDay = 1;
    int periodEndDay = dateToday.daysInMonth();

    if (date.month() == dateToday.month() && date.day() >= periodStartDay && date.day() <= periodEndDay) {
        return true;
    }

    return false;
}

bool DBRecurringCharge::isChargeDueThisPeriod(StrDate & referenceDate) {
    Logger & log = Logger::getInstance();

    StrDate periodStart = referenceDate.firstDayInMonth();
    StrDate periodEnd = referenceDate.lastDayInMonth();

    char frequencyValue = this->getFrequencyValue();
    char frequencyUnit = this->getFrequencyUnit();

    if (this->date <= periodEnd) {
        StrDate nextPaymentDate = this->date;

        switch (frequencyUnit) {
            case 'y':
                nextPaymentDate = nextPaymentDate.addYears(frequencyValue * (periodStart.year() - nextPaymentDate.year()));
                break;

            case 'm':
                nextPaymentDate = nextPaymentDate.addMonths(frequencyValue * (periodStart.month() - nextPaymentDate.month()));
                break;

            case 'w':
                nextPaymentDate = nextPaymentDate.addWeeks(frequencyValue * ((periodStart.day() - nextPaymentDate.day()) / 7));
                break;

            case 'd':
                while (nextPaymentDate.year() < periodEnd.year() || nextPaymentDate.month() < periodEnd.month()) {
                    nextPaymentDate = nextPaymentDate.addDays(frequencyValue);
                }
                break;
        }

        if (nextPaymentDate <= periodEnd && nextPaymentDate >= periodStart) {
            if (log.isLogLevel(LOG_LEVEL_DEBUG)) {
                cout << "| " << nextPaymentDate.shortDate() << " | " << frequency << " | " << setw(16) << right << amount.getFormattedStringValue() << " | " << description << endl;
            }
            return true;
        }
    }

    return false;
}

StrDate DBRecurringCharge::calculateNextPaymentDate() {
    StrDate     chargeStartDate(date);
    StrDate     dateToday;
    char        frequencyUnit;
    int         frequencyValue;

    frequencyValue = getFrequencyValue();
    frequencyUnit = getFrequencyUnit();

    StrDate nextPaymentDate = chargeStartDate;

    if (chargeStartDate <= dateToday) {
        switch (frequencyUnit) {
            case 'y':
                nextPaymentDate = nextPaymentDate.addYears(frequencyValue * (dateToday.year() - nextPaymentDate.year()));

                if (nextPaymentDate.month() <= dateToday.month() && 
                    nextPaymentDate.day() <= dateToday.day())
                {
                    nextPaymentDate = nextPaymentDate.addYears(1);
                }
                break;

            case 'm':
                nextPaymentDate = nextPaymentDate.addMonths(frequencyValue * (dateToday.month() - nextPaymentDate.month()));

                if (nextPaymentDate.day() <= dateToday.day()) {
                    nextPaymentDate = nextPaymentDate.addMonths(1);
                }
                break;

            case 'w':
                nextPaymentDate = nextPaymentDate.addWeeks(frequencyValue * ((dateToday.day() - nextPaymentDate.day()) / 7));
                break;

            case 'd':
                while (nextPaymentDate.year() < dateToday.year() || nextPaymentDate.month() < dateToday.month()) {
                    nextPaymentDate =   nextPaymentDate.addDays(frequencyValue);
                }
                break;

            default:
                throw pfm_validation_error(
                            pfm_error::buildMsg(
                                "Invalid frequency unit '%c'", 
                                frequencyUnit), 
                            __FILE__, 
                            __LINE__);
                break;
        }
    }

    return nextPaymentDate;
}

StrDate DBRecurringCharge::getNextRecurringTransactionDate(StrDate & startDate) {
    char frequencyValue = getFrequencyValue();
    char frequencyUnit = getFrequencyUnit();

    StrDate nextPaymentDate = startDate;

    /*
    ** If the start date has been modified (as it fell on a weekend),
    ** reset the nextPaymentDate here to what it should be...
    */
    if (startDate.day() != this->date.day()) {
        nextPaymentDate.set(startDate.year(), startDate.month(), this->date.day());
    }

    switch (frequencyUnit) {
        case 'y':
            nextPaymentDate = nextPaymentDate.addYears(frequencyValue);
            break;

        case 'm':
            nextPaymentDate = nextPaymentDate.addMonths(frequencyValue);
            break;

        case 'w':
            nextPaymentDate = nextPaymentDate.addWeeks(frequencyValue);
            break;

        case 'd':
            nextPaymentDate = nextPaymentDate.addDays(frequencyValue);
            break;
    }

    if (nextPaymentDate.isSaturday()) {
        nextPaymentDate = nextPaymentDate.addDays(2);
    }
    else if (nextPaymentDate.isSunday()) {
        nextPaymentDate = nextPaymentDate.addDays(1);
    }
    
    return nextPaymentDate;
}

void DBRecurringCharge::setNextPaymentDate() {
    this->nextPaymentDate = calculateNextPaymentDate();
}

void DBRecurringCharge::beforeUpdate() {
    DBRecurringCharge currentCharge;
    currentCharge.id = this->id;
    currentCharge.retrieve();

    if (this->description.compare(currentCharge.description) || 
        this->categoryId != currentCharge.categoryId ||
        this->payeeId != currentCharge.payeeId)
    {
        DBTransaction tr;
        DBResult<DBTransaction> trResult = tr.retrieveByRecurringChargeID(id);

        for (int i = 0;i < trResult.getNumRows();i++) {
            DBTransaction transaction = trResult.getResultAt(i);

            transaction.description = this->description;
            transaction.categoryId = this->categoryId;
            transaction.payeeId = this->payeeId;

            transaction.save();
        }
    }
}

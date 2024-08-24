#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db_transaction.h"
#include "db_recurring_charge.h"
#include "db.h"
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

    result.executeSelect(szStatement);

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

bool DBRecurringCharge::isChargeDueThisPeriod() {
    StrDate dateToday;
    StrDate chargeStartDate(this->date);

    char frequencyValue = this->getFrequencyValue();
    char frequencyUnit = this->getFrequencyUnit();

    if (this->date <= dateToday) {
        StrDate nextPaymentDate = this->date;

        switch (frequencyUnit) {
            case 'y':
                nextPaymentDate.addYears(frequencyValue * (dateToday.year() - nextPaymentDate.year()));

                if (isDateWithinCurrentPeriod(nextPaymentDate)) {
                    return true;
                }
                break;

            case 'm':
                nextPaymentDate.addMonths(frequencyValue * (dateToday.month() - nextPaymentDate.month()));

                if (isDateWithinCurrentPeriod(nextPaymentDate)) {
                    return true;
                }
                break;

            case 'w':
                nextPaymentDate.addWeeks(frequencyValue * ((dateToday.day() - nextPaymentDate.day()) / 7));

                if (isDateWithinCurrentPeriod(nextPaymentDate)) {
                    return true;
                }
                break;

            case 'd':
                while (nextPaymentDate.year() < dateToday.year() || nextPaymentDate.month() < dateToday.month()) {
                    nextPaymentDate.addDays(frequencyValue);
                }

                if (isDateWithinCurrentPeriod(nextPaymentDate)) {
                    return true;
                }
                break;
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

    StrDate nextPaymentDate;

    if (chargeStartDate <= dateToday) {
        nextPaymentDate = chargeStartDate;

        switch (frequencyUnit) {
            case 'y':
                nextPaymentDate.addYears(frequencyValue * (dateToday.year() - nextPaymentDate.year()));

                if (nextPaymentDate.month() <= dateToday.month() && 
                    nextPaymentDate.day() <= dateToday.day())
                {
                    nextPaymentDate.addYears(1);
                }
                break;

            case 'm':
                nextPaymentDate.addMonths(frequencyValue * (dateToday.month() - nextPaymentDate.month()));

                if (nextPaymentDate.day() <= dateToday.day()) {
                    nextPaymentDate.addMonths(1);
                }
                break;

            case 'w':
                nextPaymentDate.addWeeks(frequencyValue * ((dateToday.day() - nextPaymentDate.day()) / 7));
                break;

            case 'd':
                while (nextPaymentDate.year() < dateToday.year() || nextPaymentDate.month() < dateToday.month()) {
                    nextPaymentDate.addDays(frequencyValue);
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

void DBRecurringCharge::setNextPaymentDate() {
    this->nextPaymentDate = calculateNextPaymentDate();
}

int DBRecurringCharge::createNextTransactionForCharge(StrDate & latestDate) {
    StrDate dateToday;

    if (latestDate <= dateToday) {
        StrDate nextPaymentDate = latestDate;

        char frequencyValue = this->getFrequencyValue();
        char frequencyUnit = this->getFrequencyUnit();

        switch (frequencyUnit) {
            case 'y':
                nextPaymentDate.addYears(frequencyValue);
                break;

            case 'm':
                nextPaymentDate.addMonths(frequencyValue);
                break;

            case 'w':
                nextPaymentDate.addWeeks(frequencyValue);
                break;

            case 'd':
                nextPaymentDate.addDays(frequencyValue);
                break;
        }

        if (nextPaymentDate <= dateToday) {
            DBTransaction transaction;
            transaction.createFromRecurringChargeAndDate(*this, nextPaymentDate);

            return CHARGE_OK;
        }
    }

    return CHARGE_NOT_DUE;
}

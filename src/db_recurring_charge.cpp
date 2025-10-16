#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db_transaction.h"
#include "db_recurring_charge.h"
#include "db_config.h"
#include "db.h"
#include "cfgmgr.h"
#include "logger.h"
#include "strdate.h"

using namespace std;

static bool isNumeric(string & cfgDate) {
    bool isNumeric = true;
    for (int i = 0;i < (int)cfgDate.length();i++) {
        if (!isdigit(cfgDate.at(i))) {
            isNumeric = false;
            break;
        }
    }

    return isNumeric;
}

int DBRecurringCharge::getPeriodStartDay() {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringCharge::getPeriodStartDay()");

    cfgmgr & cfg = cfgmgr::getInstance();

    int periodStart = cfg.getValueAsInteger("cycle.start");

    log.exit("DBRecurringCharge::getPeriodStartDay()");

    return periodStart;
}

int DBRecurringCharge::getPeriodEndDay() {
    StrDate today;
    return getPeriodEndDay(today);
}

int DBRecurringCharge::getPeriodEndDay(StrDate & referenceDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringCharge::getPeriodEndDay()");

    cfgmgr & cfg = cfgmgr::getInstance();

    string cycleEnd = cfg.getValue("cycle.end");

    log.info("Value of config item 'cycle.end' is '%s'", cycleEnd.c_str());

    int periodEnd;

    if (isNumeric(cycleEnd)) {
        periodEnd = atoi(cycleEnd.c_str());

        StrDate specificDate(referenceDate.year(), referenceDate.month(), periodEnd);

        while (specificDate.isWeekend()) {
            specificDate = specificDate.addDays(-1);
        }

        periodEnd = specificDate.day();
    }
    else if (cycleEnd.compare("last-working-day") == 0) {
        StrDate lastWorkingDay = referenceDate;
        lastWorkingDay = lastWorkingDay.lastDayInMonth();

        while (lastWorkingDay.isWeekend()) {
            lastWorkingDay = lastWorkingDay.addDays(-1);
        }

        periodEnd = lastWorkingDay.day();
    }
    else if (cycleEnd.compare("last-friday") == 0) {
        StrDate lastFriday = referenceDate;
        lastFriday = lastFriday.lastDayInMonth();

        while (lastFriday.dayOfTheWeek() != StrDate::sd_friday) {
            lastFriday = lastFriday.addDays(-1);
        }

        periodEnd = lastFriday.day();
    }
    else {
        StrDate lastDay = referenceDate.lastDayInMonth();
        periodEnd = lastDay.day();
    }

    log.debug("Got period end day as %d for date '%s'", periodEnd, referenceDate.shortDate().c_str());

    log.exit("DBRecurringCharge::getPeriodEndDay()");

    return periodEnd;
}

DBResult<DBRecurringCharge> DBRecurringCharge::retrieveByAccountID(pfm_id_t accountId) {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringCharge::retrieveByAccountID()");

    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBRecurringCharge> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByAccountID, 
        accountId);

    result.retrieve(szStatement);

    log.exit("DBRecurringCharge::retrieveByAccountID()");

    return result;
}

DBResult<DBRecurringCharge> DBRecurringCharge::retrieveByAccountIDBetweenDates(pfm_id_t accountId, StrDate & dateAfter, StrDate & dateBefore) {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringCharge::retrieveByAccountIDBetweenDates()");

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

    log.exit("DBRecurringCharge::retrieveByAccountIDBetweenDates()");

    return result;
}

bool DBRecurringCharge::isActive() {
    StrDate today;

    return (endDate.isNull() || (!endDate.isNull() && endDate >= today));
}

bool DBRecurringCharge::isDateWithinCurrentPeriod(StrDate & date) {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringCharge::isDateWithinCurrentPeriod()");

    StrDate today;
    const int startDay = getPeriodStartDay();
    const int endDay = getPeriodEndDay(today);

    StrDate periodStart(today.year(), today.month(), startDay);
    StrDate periodEnd(today.year(), today.month(), endDay);

    const bool within = (date >= periodStart && date <= periodEnd);

    log.debug("The date '%s' is %swithin the current period [%s..%s]",
              date.shortDate().c_str(),
              within ? "" : "NOT ",
              periodStart.shortDate().c_str(),
              periodEnd.shortDate().c_str());

    log.exit("DBRecurringCharge::isDateWithinCurrentPeriod()");

    return within;
}

bool DBRecurringCharge::isChargeDueThisPeriod() {
    StrDate today;
    return isChargeDueThisPeriod(today);
}

bool DBRecurringCharge::isChargeDueThisPeriod(StrDate & referenceDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringCharge::isChargeDueThisPeriod()");

    if (!isActive()) {
        log.exit("DBRecurringCharge::isChargeDueThisPeriod()");
        return false;
    }

    const int periodStartDay = getPeriodStartDay();
    const int periodEndDay = getPeriodEndDay(referenceDate);

    StrDate periodStart(referenceDate.year(), referenceDate.month(), periodStartDay);
    StrDate periodEnd  (referenceDate.year(), referenceDate.month(), periodEndDay);

    // Find the next *nominal* (no weekend adjustment) scheduled date
    // that comes on or after 'referenceDate'.
    StrDate nominalNext = nextScheduledNoWeekend(referenceDate);

    // If there is no next occurrence, it's not due.
    if (nominalNext.isNull()) {
        log.exit("DBRecurringCharge::isChargeDueThisPeriod()");
        return false;
    }

    // Weekend fix:
    // - We judge the *period* membership using the nominal date.
    // - We include items due *today* (>=).
    const bool inThisPeriod =
        (nominalNext >= referenceDate) &&
        (nominalNext >= periodStart)   &&
        (nominalNext <= periodEnd);

    if (inThisPeriod) {
        // Keep nextPaymentDate (non-persistent) aligned for display/use elsewhere:
        this->nextPaymentDate = nominalNext; // nominal for clarity in UIs
        log.debug("Charge '%s' is due this period on nominal date '%s'",
                  this->description.c_str(), nominalNext.shortDate().c_str());
        log.exit("DBRecurringCharge::isChargeDueThisPeriod()");
        return true;
    }

    log.debug("Charge '%s' is NOT due this period (nominal next: %s, period: %s..%s)",
              this->description.c_str(),
              nominalNext.shortDate().c_str(),
              periodStart.shortDate().c_str(),
              periodEnd.shortDate().c_str());

    log.exit("DBRecurringCharge::isChargeDueThisPeriod()");

    return false;
}

StrDate DBRecurringCharge::calculateNextPaymentDate() {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringCharge::calculateNextPaymentDate()");

    StrDate today;
    if (!this->isActive()) {
        StrDate none; none.clear();
        this->nextPaymentDate = none;
        log.debug("Charge '%s' inactive; nextPaymentDate cleared", this->description.c_str());
        log.exit("DBRecurringCharge::calculateNextPaymentDate()");
        return none;
    }

    // Nominal next date on/after today
    StrDate nominal = nextScheduledNoWeekend(today);

    // Store nominal (non-persistent) for display; keep behavior explicit
    this->nextPaymentDate = nominal;

    log.debug("Calculated nominal nextPaymentDate of charge '%s' as '%s'",
              this->description.c_str(), nominal.shortDate().c_str());

    log.exit("DBRecurringCharge::calculateNextPaymentDate()");

    return nominal;
}

StrDate DBRecurringCharge::getNextRecurringTransactionDate(StrDate & startDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringCharge::getNextRecurringTransactionDate()");

    if (!isActive()) {
        StrDate none; none.clear();
        log.exit("DBRecurringCharge::getNextRecurringTransactionDate()");
        return none;
    }

    // Next *nominal* schedule after startDate…
    StrDate nominal = nextScheduledNoWeekend(startDate);

    // …then adjusted forward to a business day for the actual transaction date.
    StrDate txnDate = adjustForwardToBusinessDay(nominal);

    log.debug("Next transaction date for charge '%s' is '%s' (nominal: %s)",
              this->description.c_str(),
              txnDate.shortDate().c_str(),
              nominal.shortDate().c_str());

    log.exit("DBRecurringCharge::getNextRecurringTransactionDate()");
    return txnDate;
}

StrDate DBRecurringCharge::getNextRecurringScheduledDate(StrDate& startDate) {
    return nextScheduledNoWeekend(startDate);
}

StrDate DBRecurringCharge::nextByFrequency(StrDate & from) {
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

StrDate DBRecurringCharge::nextScheduledNoWeekend(StrDate & from) {
    // Nominal schedule is independent of "business day" movement.
    // Start from the *initial* schedule anchor (this->date) or after `from`.
    if (!isActive()) {
        StrDate none;
        none.clear();
        return none;
    }

    StrDate d = this->date;

    // If a lastPaymentDate exists, we should advance from there so we don't
    // re-produce already-paid occurrences.
    if (!this->lastPaymentDate.isNull()) {
        // We want the first occurrence strictly *after* lastPaymentDate
        d = this->lastPaymentDate;
    }

    // Ensure d is at least the anchor (start date)
    if (d < this->date) d = this->date;

    // If 'd' is not strictly after 'from', step forward until it is.
    while (!(d > from)) {
        d = nextByFrequency(d);
    }

    // IMPORTANT: do not adjust weekends here; this is the *nominal* schedule.
    return d;
}

StrDate DBRecurringCharge::adjustForwardToBusinessDay(StrDate& d) {
    StrDate x = d;

    if (x.isSaturday()) {
        return x.addDays(2);
    }
    else if (x.isSunday()) {
       return x.addDays(1);
    }

    return x;
}

void DBRecurringCharge::setNextPaymentDate() {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringCharge::setNextPaymentDate()");

    this->nextPaymentDate = calculateNextPaymentDate();

    log.exit("DBRecurringCharge::setNextPaymentDate()");
}

void DBRecurringCharge::beforeRemove() {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringCharge::beforeRemove()");

    DBTransaction tr;
    DBResult<DBTransaction> recurringTransactions = tr.retrieveByRecurringChargeID(this->id);

    for (int i = 0;i < recurringTransactions.size();i++) {
        DBTransaction recurringTransaction = recurringTransactions[i];

        recurringTransaction.recurringChargeId = 0;
        recurringTransaction.save();
    }

    log.exit("DBRecurringCharge::beforeRemove()");
}

void DBRecurringCharge::beforeUpdate() {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringCharge::beforeUpdate()");

    DBRecurringCharge currentCharge;
    currentCharge.id = this->id;
    currentCharge.retrieve();

    if (this->description.compare(currentCharge.description) || 
        this->categoryId != currentCharge.categoryId ||
        this->payeeId != currentCharge.payeeId)
    {
        log.debug("Updating transactions for charge '%s'", this->description.c_str());
        
        DBTransaction tr;
        DBResult<DBTransaction> trResult = tr.retrieveByRecurringChargeID(id);

        for (int i = 0;i < trResult.size();i++) {
            DBTransaction transaction = trResult.at(i);

            transaction.description = this->description;
            transaction.categoryId = this->categoryId;
            transaction.payeeId = this->payeeId;

            transaction.save();
        }
    }

    if (isTransfer()) {
        if (this->transfer->accountToId != currentCharge.transfer->accountToId) {
            this->transfer->save();
        }
    }

    log.exit("DBRecurringCharge::beforeUpdate()");
}

void DBRecurringCharge::afterInsert() {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringCharge::afterInsert()");

    if (isTransfer()) {
        transfer->accountToId = transfer->accountTo.id;
        transfer->recurringChargeId = id;

        transfer->save();
    }

    log.exit("DBRecurringCharge::afterInsert()");
}
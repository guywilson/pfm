#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db_transaction.h"
#include "db_recurring_charge.h"
#include "db_recurring_transfer.h"
#include "db_config.h"
#include "db.h"
#include "cfgmgr.h"
#include "logger.h"
#include "strdate.h"

using namespace std;

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

DBResult<DBRecurringCharge> DBRecurringCharge::retrieveByAccountID(pfm_id_t & accountId) {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringCharge::retrieveByAccountID()");

    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBRecurringCharge> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByAccountID, 
        accountId.c_str());

    result.retrieve(szStatement);

    log.exit("DBRecurringCharge::retrieveByAccountID()");

    return result;
}

DBResult<DBRecurringCharge> DBRecurringCharge::retrieveByAccountIDBetweenDates(pfm_id_t & accountId, StrDate & dateAfter, StrDate & dateBefore) {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringCharge::retrieveByAccountIDBetweenDates()");

    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBRecurringCharge> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByAccountIDBetweenDates, 
        accountId.c_str(),
        dateAfter.shortDate().c_str(),
        dateBefore.shortDate().c_str());

    result.retrieve(szStatement);

    log.exit("DBRecurringCharge::retrieveByAccountIDBetweenDates()");

    return result;
}

bool DBRecurringCharge::isWithinCurrentPeriod(StrDate & referenceDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringCharge::isWithinCurrentPeriod()");

    StrDate periodStart = getPeriodStartDate();
    StrDate periodEnd = getPeriodEndDate();

    bool inThisPeriod =
        (referenceDate >= periodStart) &&
        (referenceDate <= periodEnd);

    log.debug("The date '%s' is %swithin the current period [%s..%s]",
            date.shortDate().c_str(),
            inThisPeriod ? "" : "NOT ",
            periodStart.shortDate().c_str(),
            periodEnd.shortDate().c_str());

    log.exit("DBRecurringCharge::isWithinCurrentPeriod()");

    return inThisPeriod;
}

StrDate DBRecurringCharge::getNextScheduledDate() {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringCharge::getNextScheduledDate()");

    StrDate periodStart = getPeriodStartDate();
    StrDate d = this->date;

    // If a lastPaymentDate exists, we should advance from there so we don't
    // re-produce already-paid occurrences.
    if (!this->lastPaymentDate.isNull()) {
        d = this->lastPaymentDate;
        log.debug("Last payment date for charge '%s' is '%s'", this->description.c_str(), d.shortDate().c_str());
    }

    // Ensure d is at least the anchor (start date)
    if (d < this->date) {
        d = this->date;
    }

    while (d < periodStart) {
        d = advanceDateByFrequency(d);
    }

    if (!lastPaymentDate.isNull()) {
        if (d <= lastPaymentDate) {
            d = advanceDateByFrequency(d);
        }
    }

    log.debug("Next scheduled date for charge '%s' is '%s'", this->description.c_str(), d.shortDate().c_str());

    log.exit("DBRecurringCharge::getNextScheduledDate()");

    // IMPORTANT: do not adjust weekends here; this is the *nominal* schedule.
    return d;
}

bool DBRecurringCharge::isChargeDueThisPeriod() {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringCharge::isChargeDueThisPeriod()");

    bool inThisPeriod = false;

    if (isActive()) {
        StrDate periodStart = getPeriodStartDate();
        StrDate periodEnd = getPeriodEndDate();

        // Find the next *nominal* (no weekend adjustment) scheduled date
        StrDate nominalNext = getNextScheduledDate();

        // If there is no next occurrence, it's not due.
        if (!nominalNext.isNull()) {
            inThisPeriod = isWithinCurrentPeriod(nominalNext);

            if (inThisPeriod) {
                log.debug(
                    "Charge '%s' is due this period on nominal date '%s'",
                    this->description.c_str(), 
                    nominalNext.shortDate().c_str());
            }
            else {
                log.debug(
                    "Charge '%s' is NOT due this period on nominal date '%s'",
                    this->description.c_str(), 
                    nominalNext.shortDate().c_str());
            }
        }
    }

    log.exit("DBRecurringCharge::isChargeDueThisPeriod()");

    return inThisPeriod;
}

StrDate DBRecurringCharge::getNextRecurringTransactionDate(StrDate & startDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringCharge::getNextRecurringTransactionDate()");

    StrDate txnDate;

    if (isActive()) {
        StrDate nominal = getNextScheduledDate();
        txnDate = adjustForwardToBusinessDay(nominal);

        log.debug("Next transaction date for charge '%s' is '%s' (nominal: %s)",
                this->description.c_str(),
                txnDate.shortDate().c_str(),
                nominal.shortDate().c_str());
    }

    log.exit("DBRecurringCharge::getNextRecurringTransactionDate()");

    return txnDate;
}

void DBRecurringCharge::migrateToTransferCharge(pfm_id_t & accountToId) {
    DBTransaction tr;
    DBResult<DBTransaction> recurringTransactions = tr.retrieveByRecurringChargeID(id);

    for (int i = 0;i < recurringTransactions.size();i++) {
        DBTransaction sourceTransaction = recurringTransactions[i];

        DBAccount accountTo;
        accountTo.id = accountToId;
        accountTo.retrieve();

        sourceTransaction.reference = "TR > " + accountTo.code;

        sourceTransaction.save();

        DBTransaction targetTransaction;

        targetTransaction.accountId = accountToId;
        targetTransaction.categoryId = categoryId;

        targetTransaction.date = sourceTransaction.date;
        targetTransaction.description = sourceTransaction.description;

        DBAccount accountFrom;
        accountFrom.id = accountId;
        accountFrom.retrieve();

        targetTransaction.reference = "TR < " + accountFrom.code;

        targetTransaction.isCredit = true;
        targetTransaction.amount = sourceTransaction.amount;
        targetTransaction.isReconciled = true;

        targetTransaction.save();
    }

    DBRecurringTransfer transfer;
    transfer.accountToId = accountToId;
    transfer.recurringChargeId = this->id;

    transfer.save();
}

void DBRecurringCharge::beforeRemove() {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringCharge::beforeRemove()");

    DBTransaction tr;
    DBResult<DBTransaction> recurringTransactions = tr.retrieveByRecurringChargeID(this->id);

    for (int i = 0;i < recurringTransactions.size();i++) {
        DBTransaction recurringTransaction = recurringTransactions[i];

        recurringTransaction.recurringChargeId.clear();
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
        if (this->transfer.accountToId != currentCharge.transfer.accountToId) {
            this->transfer.save();
        }
    }

    log.exit("DBRecurringCharge::beforeUpdate()");
}

void DBRecurringCharge::afterInsert() {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringCharge::afterInsert()");

    if (isTransfer()) {
        transfer.accountToId = transfer.accountTo.id;
        transfer.recurringChargeId = id;

        transfer.save();
    }

    log.exit("DBRecurringCharge::afterInsert()");
}
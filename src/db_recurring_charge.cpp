#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <exception>

#include <sqlcipher/sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db_transaction.h"
#include "db_recurring_charge.h"
#include "db_recurring_transfer.h"
#include "db_transfer_transaction_record.h"
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

    DBCriteria criteria;
    criteria.add(DBPayment::Columns::accountId, DBCriteria::equal_to, accountId);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBRecurringCharge> result;

    result.retrieve(statement);

    log.exit("DBRecurringCharge::retrieveByAccountID()");

    return result;
}

DBResult<DBRecurringCharge> DBRecurringCharge::retrieveByAccountIDBetweenDates(pfm_id_t & accountId, StrDate & dateAfter, StrDate & dateBefore) {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringCharge::retrieveByAccountIDBetweenDates()");

    DBCriteria criteria;
    criteria.add(DBPayment::Columns::accountId, DBCriteria::equal_to, accountId);
    criteria.add(DBPayment::Columns::date, DBCriteria::greater_than_or_equal, dateAfter);
    criteria.add(DBPayment::Columns::date, DBCriteria::less_than, dateBefore);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBRecurringCharge> result;

    result.retrieve(statement);

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

StrDate DBRecurringCharge::getNextNominalScheduledDate() {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringCharge::getNextNominalScheduledDate()");

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

    /*
    ** If the nominal next payment day has drifted from the start day,
    ** then push it back to the start day. Remember this is the nominal
    ** date, accounting for weekends will be taken care in the caller...
    */
    if (d.day() > this->date.day()) {
        d.set(d.year(), d.month(), this->date.day());
    }

    log.debug("Next scheduled date for charge '%s' is '%s'", this->description.c_str(), d.shortDate().c_str());

    log.exit("DBRecurringCharge::getNextNominalScheduledDate()");

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
        StrDate nominalNext = getNextNominalScheduledDate();

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
        StrDate nominal = getNextNominalScheduledDate();
        txnDate = adjustForwardToBusinessDay(nominal);

        /*
        ** If our date has rolled over to the next month because the nominal date
        ** fell on a weekend and the next working day is the 1st of the next month,
        ** then our charge will be missed with the algorithm. So in this scenario,
        ** wind back the payment date to the previous business day.
        **
        ** An example:
        **
        ** nominal next payment date is calculated as Saturday 29th November 2025,
        ** therefore txnDate is calculated as Monday 1st December 2025 as the next 
        ** business day. Here we wind it back to Friday 28th November, although
        ** incorrect compared with reality (the payment will likely be taken on
        ** Monday 1st December), it prevents the payment being missed entirely.
        */
        if (txnDate.month() > nominal.month()) {
            log.debug("Winding back next transaction date for charge '%s'", txnDate.shortDate().c_str());

            while (txnDate >= nominal || txnDate.isWeekend()) {
                --txnDate;
            }
        }

        log.debug("Next transaction date for charge '%s' is '%s' (nominal: %s)",
                this->description.c_str(),
                txnDate.shortDate().c_str(),
                nominal.shortDate().c_str());
    }

    log.exit("DBRecurringCharge::getNextRecurringTransactionDate()");

    return txnDate;
}

void DBRecurringCharge::migrateToTransferCharge(pfm_id_t & accountToId) {
    DBAccount accountTo;
    accountTo.retrieve(accountToId);

    DBTransaction tr;
    DBResult<DBTransaction> recurringTransactions = tr.retrieveByRecurringChargeIDAfterDate(id, accountTo.openingDate);

    for (int i = 0;i < recurringTransactions.size();i++) {
        DBTransaction sourceTransaction = recurringTransactions[i];

        DBAccount accountTo;
        accountTo.retrieve(accountToId);

        DBTransaction::createTransferPairFromSource(sourceTransaction, accountTo);
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

    PFM_DB & db = PFM_DB::getInstance();

    if (isTransfer()) {
        try {
            db.begin();

            transfer.accountToId = transfer.accountTo.id;
            transfer.recurringChargeId = id;

            transfer.save();

            db.commit();
        }
        catch (exception & e) {
            log.error("Failed to create transfer record for charge '%s'", this->description.c_str());
            db.rollback();
        }
    }

    log.exit("DBRecurringCharge::afterInsert()");
}
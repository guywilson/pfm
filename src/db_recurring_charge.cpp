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

/*******************************************************************************
Got it 👍 You already have a C++ personal finance manager with accounts, transactions, and recurring charges stored in SQL. You now want to auto-generate transactions whenever a recurring charge is due, based on a frequency string (like "7d", "2w", "1m", "1y").

I’ll sketch out a clean, extensible way to do this using modern C++ best practices (RAII, strong typing, clear separation of concerns).

Here’s a possible design:

⸻

1. Define a Frequency type

This encapsulates parsing and handling of "7d" etc.

#include <string>
#include <chrono>
#include <stdexcept>
#include <cctype>

enum class FrequencyUnit { Days, Weeks, Months, Years };

struct Frequency {
    int amount;
    FrequencyUnit unit;

    static Frequency parse(const std::string& freqStr) {
        if (freqStr.empty()) throw std::invalid_argument("Empty frequency string");

        int number = 0;
        size_t i = 0;
        while (i < freqStr.size() && std::isdigit(freqStr[i])) {
            number = number * 10 + (freqStr[i] - '0');
            ++i;
        }

        if (number <= 0 || i >= freqStr.size()) 
            throw std::invalid_argument("Invalid frequency format: " + freqStr);

        char unitChar = freqStr[i];
        switch (unitChar) {
            case 'd': return { number, FrequencyUnit::Days };
            case 'w': return { number, FrequencyUnit::Weeks };
            case 'm': return { number, FrequencyUnit::Months };
            case 'y': return { number, FrequencyUnit::Years };
            default: 
                throw std::invalid_argument("Unknown frequency unit: " + std::string(1, unitChar));
        }
    }
};


⸻

2. Add a helper to compute the next due date

Use std::chrono for day/week arithmetic, but months/years usually need calendar-aware logic (e.g. std::chrono::year_month_day from <chrono> in C++20).

#include <chrono>

using namespace std::chrono;

inline sys_days addFrequency(sys_days start, const Frequency& freq) {
    year_month_day ymd{floor<days>(start)};

    switch (freq.unit) {
        case FrequencyUnit::Days:   return start + days{freq.amount};
        case FrequencyUnit::Weeks:  return start + weeks{freq.amount};
        case FrequencyUnit::Months: return sys_days{ymd + months{freq.amount}};
        case FrequencyUnit::Years:  return sys_days{ymd + years{freq.amount}};
    }
    throw std::logic_error("Unhandled frequency unit");
}


⸻

3. Model a RecurringCharge

This holds the template for generating transactions.

#include <string>

struct RecurringCharge {
    int id;  // database ID
    int accountId;
    double amount;
    std::string description;
    sys_days startDate;
    sys_days lastGenerated;   // last time we created a transaction
    Frequency frequency;
};


⸻

4. Generate due transactions

This method checks if one or more transactions should be created, updates the lastGenerated, and returns the new ones.

#include <vector>

struct Transaction {
    int accountId;
    double amount;
    std::string description;
    sys_days date;
};

inline std::vector<Transaction> generateDueTransactions(
        RecurringCharge& rc, sys_days today) 
{
    std::vector<Transaction> result;

    sys_days nextDate = rc.lastGenerated;
    nextDate = addFrequency(nextDate, rc.frequency);

    while (nextDate <= today) {
        result.push_back(Transaction{
            rc.accountId,
            rc.amount,
            rc.description,
            nextDate
        });

        rc.lastGenerated = nextDate;
        nextDate = addFrequency(nextDate, rc.frequency);
    }

    return result;
}


⸻

5. Database integration (pseudo-code)

The database layer should:
	•	Load all RecurringCharges.
	•	For each, call generateDueTransactions().
	•	Insert returned transactions into transactions table.
	•	Update lastGenerated in recurring_charges.

void processRecurringCharges(Database& db, sys_days today) {
    auto charges = db.loadRecurringCharges();
    for (auto& rc : charges) {
        auto txs = generateDueTransactions(rc, today);
        for (const auto& tx : txs) {
            db.insertTransaction(tx);
        }
        if (!txs.empty()) {
            db.updateRecurringChargeLastGenerated(rc.id, rc.lastGenerated);
        }
    }
}


⸻

✅ Best practices applied here:
	•	Strongly typed enums (FrequencyUnit).
	•	RAII with std::chrono and year_month_day for correct date math.
	•	No raw pointers; everything is value/reference-based.
	•	Database layer abstracted (your code can decide how to implement Database).
	•	Logic for recurrence isolated in its own helper (addFrequency, generateDueTransactions).

⸻

Would you like me to also sketch a SQL schema update + sample queries for storing lastGenerated and frequency strings cleanly, so this integrates smoothly with your database?
*******************************************************************************/
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

int DBRecurringCharge::getFrequencyValue() {
    return atoi(frequency.substr(0, frequency.length() - 1).c_str());
}

char DBRecurringCharge::getFrequencyUnit() {
    return frequency.substr(frequency.length() - 1, 1).c_str()[0];
}

bool DBRecurringCharge::isDateWithinCurrentPeriod(StrDate & date) {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringCharge::isDateWithinCurrentPeriod()");

    StrDate dateToday;
    int periodStartDay = 1;
    int periodEndDay = dateToday.daysInMonth();

    if (date.month() == dateToday.month() && date.day() >= periodStartDay && date.day() <= periodEndDay) {
        log.debug("The date '%s' is within the current period", date.shortDate().c_str());
        return true;
    }

    log.exit("DBRecurringCharge::isDateWithinCurrentPeriod()");

    return false;
}

bool DBRecurringCharge::isChargeDueThisPeriod() {
    StrDate today;
    return isChargeDueThisPeriod(today);
}

bool DBRecurringCharge::isChargeDueThisPeriod(StrDate & referenceDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringCharge::isChargeDueThisPeriod()");

    int periodStartDay = getPeriodStartDay();
    int periodEndDay = getPeriodEndDay(referenceDate);

    StrDate periodStart(referenceDate.year(), referenceDate.month(), periodStartDay);
    StrDate periodEnd(referenceDate.year(), referenceDate.month(), periodEndDay);

    char frequencyValue = this->getFrequencyValue();
    char frequencyUnit = this->getFrequencyUnit();

    if (this->date <= periodEnd && this->isActive()) {
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

        if (nextPaymentDate <= periodEnd && nextPaymentDate > referenceDate) {
            if (log.isLogLevel(LOG_LEVEL_DEBUG)) {
                cout << "| " << nextPaymentDate.shortDate() << " | " << frequency << " | " << setw(16) << right << amount.localeFormattedStringValue() << " | " << description << endl;
            }

            log.debug("Charge '%s' is due this period", this->description.c_str());

            return true;
        }
    }

    log.exit("DBRecurringCharge::isChargeDueThisPeriod()");

    return false;
}

StrDate DBRecurringCharge::calculateNextPaymentDate() {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringCharge::calculateNextPaymentDate()");

    StrDate     dateToday;
    char        frequencyUnit;
    int         frequencyValue;

    frequencyValue = getFrequencyValue();
    frequencyUnit = getFrequencyUnit();

    StrDate nextPaymentDate;

    if (this->isActive()) {
        nextPaymentDate = this->date;

        switch (frequencyUnit) {
            case 'y':
                nextPaymentDate = nextPaymentDate.addYears(frequencyValue * (dateToday.year() - nextPaymentDate.year()));

                if ((nextPaymentDate.month() < dateToday.month()) || 
                    (nextPaymentDate.month() == dateToday.month() && nextPaymentDate.day() <= dateToday.day()))
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
                    nextPaymentDate = nextPaymentDate.addDays(frequencyValue);
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
    else if (!isActive()) {
        nextPaymentDate.clear();
    }

    log.debug("Calculated nextPaymentDate of charge '%s' as '%s'", this->description.c_str(), nextPaymentDate.shortDate().c_str());
    log.exit("DBRecurringCharge::calculateNextPaymentDate()");

    return nextPaymentDate;
}

StrDate DBRecurringCharge::getNextRecurringTransactionDate(StrDate & startDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringCharge::getNextRecurringTransactionDate()");

    char frequencyValue = getFrequencyValue();
    char frequencyUnit = getFrequencyUnit();

    StrDate nextPaymentDate = startDate;

    if (isActive()) {
        nextPaymentDate = startDate;
    }
    else {
        nextPaymentDate.clear();
        return nextPaymentDate;
    }

    if (isActive()) {
        nextPaymentDate = startDate;

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
    }
    else {
        nextPaymentDate.clear();
    }

    log.debug("Got next transaction date for charge '%s' as '%s'", this->description.c_str(), nextPaymentDate.shortDate().c_str());
    log.exit("DBRecurringCharge::getNextRecurringTransactionDate()");

    return nextPaymentDate;
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
    DBResult<DBTransaction> recurringTransactions = tr.retrieveByRecurringChargeIDForAccount(accountId, id);

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
        DBResult<DBTransaction> trResult = tr.retrieveByRecurringChargeIDForAccount(accountId, id);

        for (int i = 0;i < trResult.size();i++) {
            DBTransaction transaction = trResult.at(i);

            transaction.description = this->description;
            transaction.categoryId = this->categoryId;
            transaction.payeeId = this->payeeId;

            transaction.save();
        }
    }

    log.exit("DBRecurringCharge::beforeUpdate()");
}

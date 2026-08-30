#include <string>
#include <exception>

#include "strdate.h"
#include "money.h"
#include "db.h"
#include "db_base.h"
#include "csv.h"
#include "reconcile.h"

#define CSV_TEMP_TABLENAME                  "csv_temp_transaction"

static const char * tempCSVTable = 
    "CREATE TABLE " \
    CSV_TEMP_TABLENAME \
    " (" \
    "id INTEGER PRIMARY KEY," \
    "account_code TEXT NOT NULL," \
    "date TEXT NOT NULL," \
    "reference TEXT," \
    "description TEXT," \
    "type TEXT NOT NULL," \
    "amount NUMERIC NOT NULL" \
    ");";


uint64_t TransactionReconciler::findSingleQuotePos(std::string & s, int startingPos = 0) const {
    uint64_t pos = s.find(SINGLE_QUOTE_CHAR, startingPos);

    if (pos != std::string::npos) {
        /*
        ** If the single quote is before the end of the string, check that 
        ** we haven't already got a double single quote. Otherwise, if the 
        ** single quote is at the end of the string, return the position. 
        ** This avoids out-of-range run-time exceptions...
        */
        if (pos < s.length() - 1) {
            if (s.at(pos + 1) != SINGLE_QUOTE_CHAR) {
                return pos;
            }
        }
        else {
            return pos;
        }
    }

    return std::string::npos;
}

const std::string TransactionReconciler::delimitSingleQuotes(std::string & s) const {
    std::string delimited = s;

    uint64_t searchPos = findSingleQuotePos(delimited);

    while (searchPos != std::string::npos) {
        delimited.insert(searchPos, 1, SINGLE_QUOTE_CHAR);
        searchPos = findSingleQuotePos(delimited, searchPos + 2);
    }

    return delimited;
}

void TransactionReconciler::populateCSVTempTable(const std::string & accountCode, CSV & csv) {
    PFM_DB & db = PFM_DB::getInstance();

    while (csv.hasMoreRows()) {
        CSV::Row row = csv.readRow();

        if (!row.contains("date") || !row.contains("amount")) {
            throw pfm_validation_error(
                        pfm_error::buildMsg(
                            "CSV file '%s' does not conatin a date or amount column", 
                            csv.csvSourceFile.c_str()));
        }

        StrDate date = row["date"].value;
        Money amount = row["amount"].value;
        std::string type = amount.doubleValue() > 0 ? "CR" : "DB";

        if (amount < 0.00) {
            amount = amount * -1;
        }

        std::string description;
        if (row.contains("description")) {
            description = row["description"].value;
        }
        
        std::string reference;
        if (row.contains("reference")) {
            reference = row["reference"].value;
        }

        std::string insertStatement = 
            "INSERT INTO " + std::string(CSV_TEMP_TABLENAME) + " (account_code, date, description, reference, type, amount) VALUES ('" +
            accountCode + "', '" +
            date.shortDate() + "', '" +
            delimitSingleQuotes(description) + "', '" +
            delimitSingleQuotes(reference) + "', '" +
            type + "', " +
            amount.rawStringValue() +
            ");";

        db.executeInsert(insertStatement);
    }
}

void TransactionReconciler::reconcileTransactions(const std::string & accountCode, const std::string & bankCSVName, const std::string & csvMappingName) {
    PFM_DB & db = PFM_DB::getInstance();
    Logger & log = Logger::getInstance();

    db.begin();

    try {
        db.createTable(tempCSVTable);

        CSV csv = CSV(bankCSVName, csvMappingName);
        populateCSVTempTable(accountCode, csv);
    }
    catch (pfm_error & e) {
        db.rollback();
        log.error("Failed reconciling '%s' csv with account '%s': %s", bankCSVName.c_str(), accountCode.c_str(), e.what());
        throw pfm_error(pfm_error::buildMsg("Failed reconciling transactions: ", e.what()));
    }

    db.commit();
}

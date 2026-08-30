#include <string>
#include <exception>

#include "strdate.h"
#include "money.h"
#include "db.h"
#include "db_base.h"
#include "csv.h"
#include "db_temp_csv.h"
#include "db_v_transaction.h"
#include "reconcile.h"

#define CSV_TEMP_TABLENAME                  "csv_temp_transaction"

static const char * createTempCSVTable = 
    "CREATE TABLE " \
    CSV_TEMP_TABLENAME \
    " (" \
    "id INTEGER PRIMARY KEY," \
    "account_code TEXT NOT NULL," \
    "date TEXT NOT NULL," \
    "reference TEXT," \
    "description TEXT," \
    "type TEXT NOT NULL," \
    "amount NUMERIC NOT NULL," \
    "created TEXT NOT NULL," \
    "updated TEXT NOT NULL" \
    ");";

static const char * dropTempCSVTable =
    "DROP TABLE " \
    CSV_TEMP_TABLENAME \
    ";";

void TransactionReconciler::dropCSVTempTable() {
    PFM_DB & db = PFM_DB::getInstance();
    db.executeWrite(dropTempCSVTable);
}

void TransactionReconciler::populateCSVTempTable(const std::string & accountCode, CSV & csv) {
    while (csv.hasMoreRows()) {
        CSV::Row row = csv.readRow();

        if (!row.contains("date") || !row.contains("amount")) {
            throw pfm_validation_error(
                        pfm_error::buildMsg(
                            "CSV file '%s' does not conatin a date or amount column", 
                            csv.csvSourceFile.c_str()));
        }

        DBTempCSV temp;

        temp.accountCode = accountCode;
        temp.date = row["date"].value;
        temp.amount = row["amount"].value;
        temp.type = temp.amount.doubleValue() > 0 ? "CR" : "DB";

        if (temp.amount < 0.00) {
            temp.amount = temp.amount * -1;
        }

        if (row.contains("description")) {
            temp.description = row["description"].value;
        }
        
        if (row.contains("reference")) {
            temp.reference = row["reference"].value;
        }

        temp.save();
    }
}

void TransactionReconciler::reconcileTransactions(const std::string & accountCode, const std::string & bankCSVName, const std::string & csvMappingName) {
    PFM_DB & db = PFM_DB::getInstance();
    Logger & log = Logger::getInstance();

    log.entry("TransactionReconciler::reconcileTransactions()");

    db.begin();

    try {
        db.createTable(createTempCSVTable);

        CSV csv = CSV(bankCSVName, csvMappingName);
        populateCSVTempTable(accountCode, csv);
    }
    catch (pfm_error & e) {
        db.rollback();
        log.error("Failed reconciling '%s' csv with account '%s': %s", bankCSVName.c_str(), accountCode.c_str(), e.what());
        throw pfm_error(pfm_error::buildMsg("Failed reconciling transactions: ", e.what()));
    }

    db.commit();

    /*
    ** Build reconciliation report with the following details:
    **
    ** 1) Transactions that are within +- 2 days and with the same amount that exist in
    ** the source CSV file but not in the PFM transation table.
    **
    ** 2) Transactions that are within +- 2 days and with the same amount that exist in
    ** the PFM transation table but not in the source CSV file.
    **
    ** 3) Transactions that are within +- 2 days and with the +- 10% amount that exist in
    ** both the PFM transation table and in the source CSV file.
    */

    DBResult<DBTempCSV> csvRecords;
    csvRecords.retrieveAll();

    DBTransactionView transaction;

    DBResult<DBTempCSV> reportPart1Records;

    for (size_t i = 0;i < csvRecords.size();i++) {
        DBTempCSV temp = csvRecords[i];

        DBResult<DBTransactionView> transactions = 
            transaction.retrieveByDateRangeAndAmount(
                temp.date.addDays(-2), 
                temp.date.addDays(2), 
                temp.amount);

        if (transactions.size() == 0) {
            reportPart1Records.addRow(temp);
        }
    }

    dropCSVTempTable();

    log.exit("TransactionReconciler::reconcileTransactions()");
}

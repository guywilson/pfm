#include <string>
#include <exception>
#include <vector>
#include <utility>

#include "strdate.h"
#include "money.h"
#include "db.h"
#include "db_base.h"
#include "csv.h"
#include "db_temp_csv.h"
#include "db_v_transaction.h"
#include "debug_views.h"
#include "reconcile.h"

using Rows = std::vector<DBRow>;
using Columns = std::vector<DBColumn>;

#define CSV_TEMP_TABLENAME                  "csv_temp_transaction"
#define DATE_SEARCH_WINDOW                  5

static const char * tempCSVTableCreate = 
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

static const char * reconciliationViewCreate = 
    "CREATE VIEW v_reconciliation AS SELECT " \
    "t.id AS transaction_id," \
    "c.id AS csv_transaction_id," \
    "a.code AS account_code," \
    "t.date," \
    "p.name AS payee_name," \
    "t.description AS pfm_description," \
    "c.description AS csv_description," \
    "t.credit_debit AS type," \
    "t.amount AS pfm_amount," \
    "c.amount AS csv_amount," \
    "ABS(t.amount - c.amount) AS amount_difference " \
    "FROM account_transaction AS t " \
    "JOIN account AS a " \
    "ON a.id = t.account_id " \
    "JOIN payee AS p " \
    "ON p.id = t.payee_id " \
    "JOIN csv_temp_transaction AS c " \
    "ON c.account_code = a.code " \
    "AND c.date = t.date " \
    "AND c.type = t.credit_debit " \
    "AND p.name IS NOT NULL " \
    "AND p.name <> '' " \
    "AND c.description IS NOT NULL " \
    "AND INSTR(LOWER(c.description), LOWER(p.name)) > 0 " \
    "AND ABS(t.amount - c.amount) <= ABS(c.amount) * 0.10 " \
    "WHERE t.amount <> c.amount;";

static const char * tempCSVTableDrop =
    "DROP TABLE " \
    CSV_TEMP_TABLENAME \
    ";";

static const char * reconciliationViewDrop =
    "DROP VIEW v_reconciliation;";

void TransactionReconciler::dropCSVTempTable() {
    PFM_DB & db = PFM_DB::getInstance();

    try {
        db.executeWrite(tempCSVTableDrop);
    }
    catch (pfm_error & e) {

    }
}

void TransactionReconciler::dropReconciliationView() {
    PFM_DB & db = PFM_DB::getInstance();

    try {
        db.executeWrite(reconciliationViewDrop);
    }
    catch (pfm_error & e) {

    }
}

Rows TransactionReconciler::reportPart1(const std::string & accountCode) {
    Logger & log = Logger::getInstance();
    PFM_DB & db = PFM_DB::getInstance();

    log.entry("TransactionReconciler::reportPart1()");

    DBTempCSV t;
    std::string statement = t.getSelectAllStatement();

    Rows csvRecords;
    db.executeRead(statement, &csvRecords);

    DBTransactionView transaction;

    Rows reportPart1Records;

    StrDate date;
    StrDate lower;
    StrDate upper;
    std::string description;
    Money amount;

    for (size_t i = 0;i < csvRecords.size();i++) {
        DBRow temp = csvRecords[i];

        for (size_t j = 0;j < temp.getNumColumns();j++) {
            DBColumn c = temp.getColumnAt((int)j);

            if (c.getName() == "date") {
                date = c.getValue();

                lower = date;
                lower.addDays(-DATE_SEARCH_WINDOW);

                upper = date;
                upper.addDays(DATE_SEARCH_WINDOW);
            }
            else if (c.getName() == "description") {
                description = c.getValue();
            }
            else if (c.getName() == "amount") {
                amount = c.doubleValue();
            }
        }

        DBResult<DBTransactionView> transactions = 
            transaction.retrieveByDateRangeAndAmountForAccount(
                accountCode,
                lower, 
                upper, 
                amount);

        if (transactions.size() == 0) {
            log.info(
                "No matching transaction found in PFM transaction table for record %s | %s | %s", 
                date.shortDate().c_str(),
                description.c_str(),
                amount.localeFormattedStringValue().c_str());

            DBColumn dateColumn("date", date.shortDate().c_str());
            DBColumn descColumn("description", description.c_str());
            DBColumn amountColumn("amount", amount.rawStringValue().c_str());

            DBRow r({dateColumn, descColumn, amountColumn});
            reportPart1Records.push_back(r);
        }
        else {
            log.info(
                "Found matching transaction(s) in PFM transaction table for record %s | %s | %s",
                date.shortDate().c_str(),
                description.c_str(),
                amount.localeFormattedStringValue().c_str());
        }
    }

    log.exit("TransactionReconciler::reportPart1()");

    return reportPart1Records;
}

Rows TransactionReconciler::reportPart2(const std::string & accountCode, const StrDate & startDate, const StrDate & endDate) {
    Logger & log = Logger::getInstance();

    log.entry("TransactionReconciler::reportPart2");

    PFM_DB & db = PFM_DB::getInstance();

    DBTransactionView t;
    std::string statement = t.getSelectStatement() + " WHERE account = '" + accountCode + "' AND date >= '" + startDate.shortDate() + "' AND date <= '" + endDate.shortDate() + "' ORDER BY date DESC;";

    Rows pfmRecords;
    db.executeRead(statement, &pfmRecords);

    DBTempCSV temp;

    Rows reportPart2Records;

    StrDate date;
    StrDate lower;
    StrDate upper;
    std::string description;
    Money amount;

    for (size_t i = 0;i < pfmRecords.size();i++) {
        DBRow transaction = pfmRecords[i];

        for (size_t j = 0;j < transaction.getNumColumns();j++) {
            DBColumn c = transaction.getColumnAt((int)j);

            if (c.getName() == "date") {
                date = c.getValue();

                lower = date;
                lower.addDays(-DATE_SEARCH_WINDOW);

                upper = date;
                upper.addDays(DATE_SEARCH_WINDOW);
            }
            else if (c.getName() == "description") {
                description = c.getValue();
            }
            else if (c.getName() == "amount") {
                amount = c.doubleValue();
            }
        }

        DBResult<DBTempCSV> transactions = 
            temp.retrieveByDateRangeAndAmountForAccount(
                accountCode,
                lower, 
                upper, 
                amount);

        if (transactions.size() == 0) {
            log.info(
                "No matching transaction found in temp CSV table for record %s | %s | %s", 
                date.shortDate().c_str(),
                description.c_str(),
                amount.localeFormattedStringValue().c_str());

            DBColumn dateColumn("date", date.shortDate().c_str());
            DBColumn descColumn("description", description.c_str());
            DBColumn amountColumn("amount", amount.rawStringValue().c_str());

            DBRow r({dateColumn, descColumn, amountColumn});
            reportPart2Records.push_back(r);
        }
        else {
            log.info(
                "Found matching transaction(s) in temp CSV table for record %s | %s | %s",
                date.shortDate().c_str(),
                description.c_str(),
                amount.localeFormattedStringValue().c_str());
        }
    }

    log.exit("TransactionReconciler::reportPart2");

    return reportPart2Records;
}

Rows TransactionReconciler::reportPart3(const std::string & accountCode, const StrDate & startDate, const StrDate & endDate) {
    Logger & log = Logger::getInstance();

    log.entry("TransactionReconciler::reportPart3");

    PFM_DB & db = PFM_DB::getInstance();

    std::string statement = "SELECT date, pfm_description, csv_description, type, pfm_amount, csv_amount FROM v_reconciliation;";

    Rows viewRecords;
    db.executeRead(statement, &viewRecords);

    Rows reportPart3Records;

    for (size_t i = 0;i < viewRecords.size();i++) {
        DBRow transaction = viewRecords[i];
        reportPart3Records.push_back(transaction);
    }

    log.exit("TransactionReconciler::reportPart3");

    return reportPart3Records;
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

    dropCSVTempTable();
    dropReconciliationView();

    db.begin();

    try {
        db.createTable(tempCSVTableCreate);

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
    ** 1) Transactions that exist in the source CSV file but not in the PFM transation table.
    ** Look for transactions in account_transaction that are within +- 5 days and with the same 
    ** amount as a transaction in csv_temp_transaction. 
    **
    ** 2) Transactions that exist in the PFM transation table but not in the source CSV file.
    ** Look for transactions in csv_temp_transaction that are within +- 5 days and with the same 
    ** amount as a transaction in account_transaction. 
    **
    ** 3) Transactions that match on date and with the +- 10% amount that exist in
    ** both the PFM transation table and in the source CSV file. We're looking for transactions
    ** where we have the amount wrong in PFM.
    */

    Rows reportPart1Records = reportPart1(accountCode);

    DBTempCSV temp;
    std::pair<StrDate, StrDate> dateRange = temp.getDateRangeForAccount(accountCode);

    Rows reportPart2Records = reportPart2(accountCode, dateRange.first, dateRange.second);

    db.createView(reconciliationViewCreate);

    Rows reportPart3Records = reportPart3(accountCode, dateRange.first, dateRange.second);
    
    if (reportPart1Records.size() > 0) {
        std::cout << std::endl << "********************************************************************************" << std::endl;
        std::cout <<"Transactions in " << bankCSVName << " but not in the PFM database :" << std::endl;
        std::cout << "********************************************************************************" << std::endl;

        GenericListView view;
        view.addRows(reportPart1Records);

        view.show();
    }
    if (reportPart2Records.size() > 0) {
        std::cout << std::endl << "********************************************************************************" << std::endl;
        std::cout << "Transactions in the PFM database but not in " << bankCSVName << " :" << std::endl;
        std::cout << "********************************************************************************" << std::endl;
        
        GenericListView view;
        view.addRows(reportPart2Records);

        view.show();
    }
    if (reportPart3Records.size() > 0) {
        std::cout << std::endl << "********************************************************************************" << std::endl;
        std::cout << "Transactions within 10% of the amount in " << bankCSVName << " :" << std::endl;
        std::cout << "********************************************************************************" << std::endl;
        
        GenericListView view;
        view.addRows(reportPart3Records);

        view.show();
    }

    log.exit("TransactionReconciler::reconcileTransactions()");
}

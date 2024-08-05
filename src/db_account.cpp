#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db_account.h"
#include "db_new.h"
#include "strdate.h"

using namespace std;

sqlite3_int64 DBAccount::insert() {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];

    string now = StrDate::now();

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN,
        sqlInsert,
        name.c_str(),
        code.c_str(),
        openingBalance,
        currentBalance,
        now.c_str(),
        now.c_str());

    string statement = szStatement;

    return db.executeInsert(statement);
}

void DBAccount::update() {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];

    string now = StrDate::now();

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN,
        sqlUpdate,
        code.c_str(),
        name.c_str(),
        openingBalance,
        currentBalance,
        now.c_str(),
        id);

    string statement = szStatement;

    db.executeUpdate(statement);
}

DBAccount::DBAccount() : DBBase() {
    clear();

    string dbName = "test.db";
    db.open(dbName);
}

void DBAccount::clear(void) {
    DBBase::clear();

    this->name = "";
    this->code = "";
    this->openingBalance = 0.0;
    this->currentBalance = 0.0;
}

void DBAccount::set(const DBAccount & src) {
    DBBase::set(src);

    this->name =            src.name;
    this->code =            src.code;
    this->openingBalance =  src.openingBalance;
    this->currentBalance =  src.currentBalance;
}

void DBAccount::print(void) {
    DBBase::print();

    cout << "Name: '" << name << "'" << endl;
    cout << "Code: '" << code << "'" << endl;

    cout << fixed << setprecision(2);
    cout << "Opening balance: " << openingBalance << endl;
    cout << "Current balance: " << currentBalance << endl;
}

void DBAccount::save() {
    if (id == 0) {
        id = insert();
    }
    else {
        update();
    }
}

DBAccount DBAccount::retrieveByID(sqlite3_int64 id) {
    DBAccount       account;
    DBAccountResult result;
    char            szStatement[SQL_STATEMENT_BUFFER_LEN];

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByID, 
        id);

    string statement = szStatement;

    db.executeSelect(statement, &result);

    return result.getAccountAt(0);
}

DBAccount DBAccount::retrieveByCode(string & code) {
    char            szStatement[SQL_STATEMENT_BUFFER_LEN];
    int             rowsRetrievedCount;
    DBAccountResult result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByCode, 
        code.c_str());

    string statement = szStatement;

    rowsRetrievedCount = db.executeSelect(statement, &result);

    if (rowsRetrievedCount != 1) {
        throw pfm_error(
                pfm_error::buildMsg("Expected exactly 1 row, got %d", rowsRetrievedCount), 
                __FILE__, 
                __LINE__);
    }

    return result.getAccountAt(0);
}

DBAccountResult DBAccount::retrieveAll() {
    DBAccountResult result;

    string statement = sqlSelectAll;

    db.executeSelect(statement, &result);

    return result;
}

void DBAccount::remove() {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN,
        sqlDelete,
        id);

    string statement = szStatement;

    db.executeDelete(statement);
}

void DBAccountResult::clear() {
    DBResult::clear();
    results.clear();
}

DBAccount DBAccountResult::getAccountAt(int i) {
    if (getNumRows() > i) {
        return results[i];
    }
    else {
        throw pfm_error(
                pfm_error::buildMsg(
                    "getAccountAt(): Index out of range: numRows: %d, requested row: %d", getNumRows(), i), 
                __FILE__, 
                __LINE__);
    }
}

void DBAccountResult::processRow(DBRow & row) {
    DBAccount account;

    for (size_t i = 0;i < row.getNumColumns();i++) {
        DBColumn column = row.getColumnAt(i);

        if (column.getName() == "id") {
            account.id = column.getIDValue();
        }
        else if (column.getName() == "name") {
            account.name = column.getValue();
        }
        else if (column.getName() == "code") {
            account.code = column.getValue();
        }
        else if (column.getName() == "opening_balance") {
            account.openingBalance = column.getDoubleValue();
        }
        else if (column.getName() == "current_balance") {
            account.currentBalance = column.getDoubleValue();
        }
        else if (column.getName() == "created") {
            account.createdDate = column.getValue();
        }
        else if (column.getName() == "updated") {
            account.updatedDate = column.getValue();
        }
    }
    
    results.push_back(account);
    incrementNumRows();
}

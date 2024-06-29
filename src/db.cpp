#include <iostream>
#include <string>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <sqlite3.h>

#include "account.h"
#include "db.h"
#include "pfm_error.h"

#define SQLITE_ERROR_BUFFER_LEN                     512
#define SQL_STATEMENT_BUFFER_LEN                    256

using namespace std;

static const char * pszCreateAccountTable = 
    "CREATE TABLE account (" \
    "id INTEGER PRIMARY KEY," \
    "name TEXT NOT NULL," \
    "code TEXT NOT NULL," \
    "opening_balance NUMERIC NOT NULL," \
    "current_balance NUMERIC NOT NULL," \
    "UNIQUE(code) ON CONFLICT ROLLBACK" \
    ");";

static const char * pszCreateCategoryTable = 
    "CREATE TABLE category (" \
    "id INTEGER PRIMARY KEY," \
    "code TEXT NOT NULL," \
    "description TEXT NOT NULL" \
    ");";

static const char * pszCreatePayeeTable = 
    "CREATE TABLE payee (" \
    "id INTEGER PRIMARY KEY," \
    "code TEXT NOT NULL," \
    "name TEXT NOT NULL" \
    ");";

static const char * pszCreateRCTable = 
    "CREATE TABLE recurring_charge (" \
    "id INTEGER PRIMARY KEY," \
    "account_id INTEGER," \
    "payee_id INTEGER," \
    "date TEXT," \
    "description TEXT NOT NULL," \
    "amount NUMERIC NOT NULL," \
    "frequency TEXT NOT NULL," \
    "FOREIGN KEY(account_id) REFERENCES account(id)," \
    "FOREIGN KEY(payee_id) REFERENCES payee(id)" \
    ");";

static const char * pszCreateTransationTable = 
    "CREATE TABLE account_transaction (" \
    "id INTEGER PRIMARY KEY," \
    "account_id INTEGER," \
    "category_id INTEGER," \
    "payee_id INTEGER," \
    "date TEXT NOT NULL," \
    "description TEXT NOT NULL," \
    "credit_debit TEXT NOT NULL," \
    "amount NUMERIC NOT NULL," \
    "FOREIGN KEY(account_id) REFERENCES account(id)," \
    "FOREIGN KEY(category_id) REFERENCES category(id)," \
    "FOREIGN KEY(payee_id) REFERENCES payee(id)" \
    ");";

bool AccountDB::open(string dbName) {
    int error = sqlite3_open_v2(
                    dbName.c_str(), 
                    &this->dbHandle, 
                    SQLITE_OPEN_READWRITE, 
                    NULL);

    if (error != SQLITE_OK) {
        if ((error & 0x000000FF) ==  SQLITE_CANTOPEN) {
            error = sqlite3_open_v2(
                            dbName.c_str(),
                            &this->dbHandle,
                            SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                            NULL);

            if (error == SQLITE_OK) {
                cout << "Data file '" << dbName << "' does not exist, creating..." << endl;
                createSchema();
            }
            else {
                throw pfm_fatal(
                        pfm_fatal::buildMsg(
                            "Cannot open database file %s, aborting: %d:%s", 
                            dbName.c_str(),
                            error,
                            sqlite3_errmsg(this->dbHandle)));
            }
        }
    }

    return true;
}

AccountDB::~AccountDB() {
    sqlite3_close_v2(this->dbHandle);
}

void AccountDB::createSchema() {
    int     error = SQLITE_OK;
    char *  pszErrorMsg;

    pszErrorMsg = (char *)sqlite3_malloc(SQLITE_ERROR_BUFFER_LEN);

    try {
        error = sqlite3_exec(
                        dbHandle, 
                        pszCreateAccountTable,
                        NULL,
                        NULL,
                        &pszErrorMsg);

        if (error) {
            throw pfm_error(
                pfm_error::buildMsg(
                    "Execute failed in createSchema(): %s", 
                    pszErrorMsg), 
                __FILE__, 
                __LINE__);
        }

        error = sqlite3_exec(
                        dbHandle, 
                        pszCreateCategoryTable,
                        NULL,
                        NULL,
                        &pszErrorMsg);

        if (error) {
            throw pfm_error(
                pfm_error::buildMsg(
                    "Execute failed in createSchema(): %s", 
                    pszErrorMsg), 
                __FILE__, 
                __LINE__);
        }        

        error = sqlite3_exec(
                        dbHandle, 
                        pszCreatePayeeTable,
                        NULL,
                        NULL,
                        &pszErrorMsg);

        if (error) {
            throw pfm_error(
                pfm_error::buildMsg(
                    "Execute failed in createSchema(): %s", 
                    pszErrorMsg), 
                __FILE__, 
                __LINE__);
        }        

        error = sqlite3_exec(
                        dbHandle, 
                        pszCreateRCTable,
                        NULL,
                        NULL,
                        &pszErrorMsg);

        if (error) {
            throw pfm_error(
                pfm_error::buildMsg(
                    "Execute failed in createSchema(): %s", 
                    pszErrorMsg), 
                __FILE__, 
                __LINE__);
        }        

        error = sqlite3_exec(
                        dbHandle, 
                        pszCreateTransationTable,
                        NULL,
                        NULL,
                        &pszErrorMsg);

        if (error) {
            throw pfm_error(
                pfm_error::buildMsg(
                    "Execute failed in createSchema(): %s", 
                    pszErrorMsg), 
                __FILE__, 
                __LINE__);
        }        
    }
    catch (pfm_error & e) {
        sqlite3_free(pszErrorMsg);
        throw e;
    }
    
    sqlite3_free(pszErrorMsg);
}

sqlite3_int64 AccountDB::createAccount(string name, string code, double openingBalance) {
    char *          pszErrorMsg;
    char *          pszInsertStatement;
    int             error;

    pszErrorMsg = (char *)sqlite3_malloc(SQLITE_ERROR_BUFFER_LEN);
    pszInsertStatement = (char *)sqlite3_malloc(SQL_STATEMENT_BUFFER_LEN);

    snprintf(
        pszInsertStatement, 
        SQL_STATEMENT_BUFFER_LEN,
        "INSERT INTO account (name, code, opening_balance, current_balance) VALUES ('%s', '%s', %.2f, %.2f);",
        name.c_str(),
        code.c_str(),
        openingBalance,
        openingBalance);

    error = sqlite3_exec(dbHandle, pszInsertStatement, NULL, NULL, &pszErrorMsg);

    if (error) {
        throw pfm_error(
            pfm_error::buildMsg(
                "Failed to create account %s: %s", 
                name.c_str(),
                sqlite3_errmsg(dbHandle)), 
            __FILE__, 
            __LINE__);
    }

    sqlite3_free(pszInsertStatement);
    sqlite3_free(pszErrorMsg);

    return sqlite3_last_insert_rowid(dbHandle);
}

static int accountCallback(void * p, int numColumns, char ** columns, char ** columnNames) {
    int                     columnIndex = 0;
    Account                 account;
    AccountResult *         result = (AccountResult *)p;

    while (columnIndex < numColumns) {
        if (strncmp(&columnNames[columnIndex][0], "id", 2) == 0) {
            account.id= strtoll(&columns[columnIndex][0], NULL, 10);
        }
        else if (strncmp(&columnNames[columnIndex][0], "name", 4) == 0) {
            account.name.assign(&columns[columnIndex][0]);
        }
        else if (strncmp(&columnNames[columnIndex][0], "code", 4) == 0) {
            account.code.assign(&columns[columnIndex][0]);
        }
        else if (strncmp(&columnNames[columnIndex][0], "opening_balance", 16) == 0) {
            account.openingBalance = strtod(&columns[columnIndex][0], NULL);
        }
        else if (strncmp(&columnNames[columnIndex][0], "current_balance", 16) == 0) {
            account.currentBalance = strtod(&columns[columnIndex][0], NULL);
        }

        columnIndex++;
    }

    result->results.push_back(account);
    result->numRows++;

    return SQLITE_OK;
}

int AccountDB::getAccounts(AccountResult * result) {
    char *          pszErrorMsg;
    int             error;

    const char * pszStatement = 
                "SELECT id, name, code, opening_balance, current_balance FROM account;";

    error = sqlite3_exec(dbHandle, pszStatement, accountCallback, result, &pszErrorMsg);

    if (error) {
        throw pfm_error(
                pfm_error::buildMsg(
                    "Failed to get accounts list: %s", 
                    pszErrorMsg), 
                __FILE__, 
                __LINE__);
    }

    return result->numRows;
}

int AccountDB::getAccount(string code, AccountResult * result) {
    char *          pszErrorMsg;
    char            szStatement[SQL_STATEMENT_BUFFER_LEN];
    int             error;

    const char * pszTemplate = 
                "SELECT id, name, code, opening_balance, current_balance FROM account where code = '%s';";

    snprintf(szStatement, SQL_STATEMENT_BUFFER_LEN - 1, pszTemplate, code.c_str());

    error = sqlite3_exec(dbHandle, szStatement, accountCallback, result, &pszErrorMsg);

    if (error) {
        throw pfm_error(
                pfm_error::buildMsg(
                    "Failed to get account: %s", 
                    pszErrorMsg), 
                __FILE__, 
                __LINE__);
    }
    else if (result->numRows != 1) {
        throw pfm_error(
            pfm_error::buildMsg(
                "Expected 1 result, got %d", 
                result->numRows),
            __FILE__,
            __LINE__);
    }

    result->results[0].print();

    return result->numRows;
}

int AccountDB::updateAccount(Account & account) {
    char *          pszErrorMsg;
    char *          pszUpdateStatement;
    int             error;

    pszErrorMsg = (char *)sqlite3_malloc(SQLITE_ERROR_BUFFER_LEN);
    pszUpdateStatement = (char *)sqlite3_malloc(SQL_STATEMENT_BUFFER_LEN);

    snprintf(
        pszUpdateStatement, 
        SQL_STATEMENT_BUFFER_LEN - 1,
        "UPDATE account SET code = '%s', name = '%s', opening_balance = %.2f, current_balance = %.2f WHERE id = %lld;",
        account.code.c_str(),
        account.name.c_str(),
        account.openingBalance,
        account.currentBalance,
        account.id);

    error = sqlite3_exec(dbHandle, pszUpdateStatement, NULL, NULL, &pszErrorMsg);

    if (error) {
        throw pfm_error(
            pfm_error::buildMsg(
                "Failed to update account with id %lld: %s", 
                account.id,
                sqlite3_errmsg(dbHandle)), 
            __FILE__, 
            __LINE__);
    }

    sqlite3_free(pszUpdateStatement);
    sqlite3_free(pszErrorMsg);

    return 0;
}

int AccountDB::deleteAccount(Account & account) {
    char *          pszErrorMsg;
    char *          pszDeleteStatement;
    int             error;

    pszErrorMsg = (char *)sqlite3_malloc(SQLITE_ERROR_BUFFER_LEN);
    pszDeleteStatement = (char *)sqlite3_malloc(SQL_STATEMENT_BUFFER_LEN);

    snprintf(
        pszDeleteStatement, 
        SQL_STATEMENT_BUFFER_LEN - 1,
        "DELETE FROM account WHERE id = %lld;",
        account.id);

    error = sqlite3_exec(dbHandle, pszDeleteStatement, NULL, NULL, &pszErrorMsg);

    if (error) {
        throw pfm_error(
            pfm_error::buildMsg(
                "Failed to update account with id %lld: %s", 
                account.id,
                sqlite3_errmsg(dbHandle)), 
            __FILE__, 
            __LINE__);
    }

    sqlite3_free(pszDeleteStatement);
    sqlite3_free(pszErrorMsg);

    return 0;
}

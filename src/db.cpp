#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <sqlite3.h>

#include "db.h"
#include "pfm_error.h"

#define SQLITE_ERROR_BUFFER_LEN                     512

using namespace std;

static const char * pszCreateAccountTable = 
    "CREATE TABLE account (" \
    "id INTEGER PRIMARY KEY," \
    "name TEXT NOT NULL," \
    "code TEXT NOT NULL," \
    "opening_balance NUMERIC NOT NULL," \
    "current_balance NUMERIC NOT NULL" \
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
    pszInsertStatement = (char *)sqlite3_malloc(512);

    snprintf(
        pszInsertStatement, 
        512,
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
                pszErrorMsg), 
            __FILE__, 
            __LINE__);
    }

    sqlite3_free(pszInsertStatement);
    sqlite3_free(pszErrorMsg);

    return sqlite3_last_insert_rowid(dbHandle);
}

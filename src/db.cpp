/*
** Copyright (c) 2025, Guy Wilson
** 
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
** 
** 1. Redistributions of source code must retain the above copyright notice, this
**    list of conditions and the following disclaimer.
** 
** 2. Redistributions in binary form must reproduce the above copyright notice,
**    this list of conditions and the following disclaimer in the documentation
**    and/or other materials provided with the distribution.
** 
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
** CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
** OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
** 
** Copyright (c) 2025, ZETETIC LLC
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the ZETETIC LLC nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
** 
** THIS SOFTWARE IS PROVIDED BY ZETETIC LLC ''AS IS'' AND ANY
** EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL ZETETIC LLC BE LIABLE FOR ANY
** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <iostream>
#include <string>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#endif

#include <pthread.h>
#include <sqlcipher/sqlite3.h>
#include <gcrypt.h>

#include "logger.h"
#include "db.h"
#include "strdate.h"
#include "pfm_error.h"
#include "schema.h"

using namespace std;

#ifndef DEBUG_PASSWORD
#define DEBUG_PASSWORD          ""
#endif

static int __getch(void) {
	int		ch;

#ifndef _WIN32
	struct termios current;
	struct termios original;

	tcgetattr(fileno(stdin), &original); /* grab old terminal i/o settings */
	current = original; /* make new settings same as old settings */
	current.c_lflag &= ~ICANON; /* disable buffered i/o */
	current.c_lflag &= ~ECHO; /* set echo mode */
	tcsetattr(fileno(stdin), TCSANOW, &current); /* use these new terminal i/o settings now */
#endif

#ifdef _WIN32
    ch = _getch();
#else
    ch = getchar();
#endif

#ifndef _WIN32
	tcsetattr(0, TCSANOW, &original);
#endif

    return ch;
}

static string getPassword(const string & prompt) {
    cout << prompt;

    char password[256];
	int	ch = 0;
	int i = 0;
	
    while (ch != '\n') {
        ch = __getch();

        if (ch != '\n' && ch != '\r') {
            putchar('*');
            fflush(stdout);

            password[i++] = (char)ch;
        }
    }

	password[i] = 0;
	
    cout << endl;
    fflush(stdout);

    return string(password);
}

static string getKeyFromPassword(const string & password) {
	uint32_t keySize = gcry_md_get_algo_dlen(GCRY_MD_SHA3_256);

	uint8_t * keyBuffer = (uint8_t *)malloc(keySize);
	char * k = (char *)malloc((keySize * 2) + 1);

	gcry_md_hash_buffer(GCRY_MD_SHA3_256, keyBuffer, password.c_str(), password.length());

	char hexBuffer[3];
    int j = 0;
    for (int i = 0;i < (int)keySize;i++) {
        snprintf(hexBuffer, 3, "%02X", keyBuffer[i]);

        k[j++] = hexBuffer[0];
        k[j++] = hexBuffer[1];
    }

    k[j] = 0;

    string key(k);

	free(keyBuffer);
	free(k);
	
	return key;
}

string PFM_DB::getKey(const string & prompt) {
    string password = getPassword(prompt);
    string key = getKeyFromPassword(password);
	
	return key;
}

static inline int _retrieveCallback(void * p, int numColumns, char ** columns, char ** columnNames) {
    vector<DBRow> * rows = (vector<DBRow> *)p;
    vector<DBColumn> columnVector;

    for (int i = 0;i < numColumns;i++) {
        DBColumn column(columnNames[i], columns[i]);
        columnVector.push_back(column);
    }

    DBRow row(numColumns, columnVector);

    rows->push_back(row);

    return SQLITE_OK;
}

int PFM_DB::openReadWrite(const string & dbName) {
    int error = sqlite3_open_v2(
                    dbName.c_str(), 
                    &this->dbHandle, 
                    SQLITE_OPEN_READWRITE, 
                    NULL);

    if (!isNewFileRequired(error) && error != SQLITE_OK) {
        const char * errorMsg = sqlite3_errmsg(this->dbHandle);

        log.logFatal(
                "Cannot open database file %s for read-write, aborting: %d:%s", 
                databaseName.c_str(),
                error,
                errorMsg);

        throw pfm_fatal(
                pfm_fatal::buildMsg(
                    "Cannot open database file %s for read-write, aborting: %d:%s", 
                    dbName.c_str(),
                    error,
                    errorMsg));
    }

    return error;
}

bool PFM_DB::isNewFileRequired(int errorCode) {
    if ((errorCode & 0x000000FF) == SQLITE_CANTOPEN) {
        return true;
    }

    return false;
}

void PFM_DB::createDB(const string & dbName) {
    int error = sqlite3_open_v2(
                    dbName.c_str(),
                    &this->dbHandle,
                    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                    NULL);

    if (error != SQLITE_OK) {
        const char * errorMsg = sqlite3_errmsg(this->dbHandle);

        log.logFatal(
                "Cannot open new database file %s, aborting: %d:%s", 
                dbName.c_str(),
                error,
                errorMsg);

        throw pfm_fatal(
                pfm_fatal::buildMsg(
                    "Cannot open new database file %s, aborting: %d:%s", 
                    dbName.c_str(),
                    error,
                    errorMsg));
    }
}

void PFM_DB::applyDatabaseKey(const string & dbName) {
#ifndef RUN_IN_DEBUGGER
    string key = getKey("Enter database password: ");
#else
    string key = getKeyFromPassword(DEBUG_PASSWORD);
#endif
    int keyError = sqlite3_key(this->dbHandle, key.c_str(), key.length());

    if (keyError != SQLITE_OK) {
        const char * errorMsg = sqlite3_errmsg(this->dbHandle);

        log.logFatal(
                "Cannot apply key to database file %s, aborting: %d:%s", 
                dbName.c_str(),
                keyError,
                errorMsg);

        throw pfm_fatal(
                pfm_fatal::buildMsg(
                    "Cannot apply key to database file %s, aborting: %d:%s", 
                    dbName.c_str(),
                    keyError,
                    errorMsg));
    }
}

void PFM_DB::open(const string & dbName) {
    log.logEntry("PFM_DB::open()");

    try {
        int error = openReadWrite(dbName);

        if (isNewFileRequired(error)) {
            createDB(dbName);

            applyDatabaseKey(dbName);

            cout << "Data file '" << dbName << "' does not exist, creating..." << endl;
            createSchema();
        }
        else {
            applyDatabaseKey(dbName);
        }
    }
    catch (pfm_fatal & f) {
        throw f;
    }

    databaseName = dbName;

    log.logExit("PFM_DB::open()");
}

void PFM_DB::open() {
    open(this->databaseName);
}

void PFM_DB::close() {
    log.logEntry("PFM_DB::close()");
    
    int error = sqlite3_close_v2(this->dbHandle);

    if (error != SQLITE_OK) {
        const char * errorMsg = sqlite3_errmsg(this->dbHandle);

        close();

        log.logFatal(
                "Cannot close database file %s, aborting: %d:%s", 
                databaseName.c_str(),
                error,
                errorMsg);

        throw pfm_fatal(
                pfm_fatal::buildMsg(
                    "Cannot close database file %s, aborting: %d:%s", 
                    databaseName.c_str(),
                    error,
                    errorMsg));
    }

    log.logExit("PFM_DB::close()");
}

void PFM_DB::changePassword() {
    log.logEntry("PFM_DB::changePassword()");

    string newPassword = getKey("Enter new database password: ");

    int error = sqlite3_rekey(dbHandle, newPassword.c_str(), newPassword.length());

    if (error != SQLITE_OK) {
        const char * errorMsg = sqlite3_errmsg(this->dbHandle);

        close();

        log.logFatal(
                "Failed to change the password on database file %s, aborting: %d:%s", 
                databaseName.c_str(),
                error,
                errorMsg);

        throw pfm_fatal(
                pfm_fatal::buildMsg(
                    "Failed to change the password on database file %s, aborting: %d:%s", 
                    databaseName.c_str(),
                    error,
                    errorMsg));
    }

    close();
    open();

    log.logExit("PFM_DB::changePassword()");
}

bool PFM_DB::getIsTransactionActive() {
    log.logEntry("PFM_DB::getIsTransactionActive()");

    pthread_mutex_lock(&mutex);
    bool active = isTransactionActive;
    pthread_mutex_unlock(&mutex);

    log.logExit("PFM_DB::getIsTransactionActive()");

    return active;
}

void PFM_DB::setIsTransactionActive() {
    log.logEntry("PFM_DB::setIsTransactionActive()");

    pthread_mutex_lock(&mutex);
    isTransactionActive = true;
    pthread_mutex_unlock(&mutex);

    log.logExit("PFM_DB::setIsTransactionActive()");
}

void PFM_DB::clearIsTransactionActive() {
    log.logEntry("PFM_DB::clearIsTransactionActive()");

    pthread_mutex_lock(&mutex);
    isTransactionActive = false;
    pthread_mutex_unlock(&mutex);

    log.logExit("PFM_DB::clearIsTransactionActive()");
}

void PFM_DB::_executeSQLNoCallback(const char * sql) {
    log.logEntry("PFM_DB::_executeSQLNoCallback()");

    log.logDebug("Executing SQL '%s'", sql);
    
    char * pszErrorMsg;
    int error = sqlite3_exec(dbHandle, sql, NULL, NULL, &pszErrorMsg);

    if (error) {
        log.logError("Failed to execute statement '%s' with error '%s'", sql, pszErrorMsg);

        throw pfm_error(
            pfm_error::buildMsg(
                "Failed to execute statement '%s' with error %s",
                sql,
                pszErrorMsg), 
            __FILE__, 
            __LINE__);
    }

    log.logExit("PFM_DB::_executeSQLNoCallback()");
}

void PFM_DB::_executeSQLCallback(const char * sql, vector<DBRow> * rows) {
    log.logEntry("PFM_DB::_executeSQLCallback()");

    log.logDebug("Executing SQL '%s'", sql);

    char * pszErrorMsg;
    int error = sqlite3_exec(dbHandle, sql, _retrieveCallback, rows, &pszErrorMsg);

    if (error) {
        log.logError("Failed to execute statement '%s' with error '%s'", sql, pszErrorMsg);

        throw pfm_error(
            pfm_error::buildMsg(
                "Failed to execute statement '%s' with error %s",
                sql,
                pszErrorMsg), 
            __FILE__, 
            __LINE__);
    }

    log.logExit("PFM_DB::_executeSQLCallback()");
}

void PFM_DB::createTable(const char * sql) {
    log.logEntry("PFM_DB::createTable()");
    
    log.logDebug("Creating table with sql %s", sql);

    try {
        _executeSQLNoCallback(sql);
    }
    catch (pfm_error & e) {
        log.logError("Failed to create table with error '%s'", e.what());

        throw pfm_error(
            pfm_error::buildMsg(
                "Failed to create table with statement '%s' with error %s",
                sql,
                e.what()), 
            __FILE__, 
            __LINE__);
    }

    log.logExit("PFM_DB::createTable()");
}

void PFM_DB::createView(const char * sql) {
    log.logEntry("PFM_DB::createView()");
    
    log.logDebug("Creating view with sql %s", sql);

    try {
        _executeSQLNoCallback(sql);
    }
    catch (pfm_error & e) {
        log.logError("Failed to create view with error '%s'", e.what());

        throw pfm_error(
            pfm_error::buildMsg(
                "Failed to create view with statement '%s' with error %s",
                sql,
                e.what()), 
            __FILE__, 
            __LINE__);
    }

    log.logExit("PFM_DB::createView()");
}

void PFM_DB::createSchema() {
    log.logEntry("PFM_DB::createSchema()");

    try {
        createTable(pszCreateConfigTable);
        createTable(pszCreateCurrencyTable);
        createTable(pszCreateAccountTable);
        createTable(pszCreatePrimaryAccountTable);
        createTable(pszCreateCategoryTable);
        createTable(pszCreatePayeeTable);
        createTable(pszCreateRCTable);
        createTable(pszCreateTransationTable);
        createTable(pszCreateCarriedOverTable);
        createTable(pszCreateBudgetTable);
        createTable(pszCreateBudgetTrackTable);

        createView(pszCreateCOView);
        createView(pszCreateListRCView);
        createView(pszCreateListTransationView);
        createView(pszCreateBudgetTrackView);

        createCurrencies();
        createDefaultCategories();
        createDefaultConfig();
    }
    catch (pfm_error & e) {
        log.logError("Failed to create schema: %s", e.what());
        throw e;
    }

    log.logExit("PFM_DB::createSchema()");
}

void PFM_DB::createDefaultCategories() {
    log.logEntry("PFM_DB::createDefaultCategories()");

    const char * insertStatement = 
        "INSERT INTO category (code, description, created, updated) VALUES ('%s', '%s', '%s', '%s')";

    char statement[SQL_STATEMENT_BUFFER_LEN];

    string now = StrDate::getTimestamp();

    try {
        begin();

        for (int i = 0;i < NUM_DEFAULT_CATEGORIES;i++) {
            snprintf(
                statement, 
                SQL_STATEMENT_BUFFER_LEN, 
                insertStatement, 
                defaultCategories[i][0], 
                defaultCategories[i][1], 
                now.c_str(), 
                now.c_str());

            executeInsert(statement);
        }

        commit();
    }
    catch (pfm_error & e) {
        rollback();

        log.logError("Failed to insert default categories");
        throw pfm_error("Failed to insert default categories", __FILE__, __LINE__);
    }

    log.logExit("PFM_DB::createDefaultCategories()");
}

void PFM_DB::createDefaultConfig() {
    log.logEntry("PFM_DB::createDefaultConfig()");

    const char * insertStatement = 
        "INSERT INTO config (key, value, description, created, updated) VALUES ('%s', '%s', '%s', '%s', '%s')";

    char statement[SQL_STATEMENT_BUFFER_LEN];

    string now = StrDate::getTimestamp();

    try {
        begin();

        for (int i = 0;i < NUM_CONFIG_ITEMS;i++) {
            snprintf(
                statement, 
                SQL_STATEMENT_BUFFER_LEN, 
                insertStatement, 
                defaultConfig[i][0], 
                defaultConfig[i][1], 
                defaultConfig[i][2], 
                now.c_str(), 
                now.c_str());

            executeInsert(statement);
        }

        commit();
    }
    catch (pfm_error & e) {
        rollback();

        log.logError("Failed to insert default config");
        throw pfm_error("Failed to insert default config", __FILE__, __LINE__);
    }

    log.logExit("PFM_DB::createDefaultConfig()");
}

void PFM_DB::createCurrencies() {
    log.logEntry("PFM_DB::createCurrencies()");

    const char * insertStatement = 
        "INSERT INTO currency (code, name, symbol, created, updated) VALUES ('%s', '%s', '%s', '%s', '%s')";

    char statement[SQL_STATEMENT_BUFFER_LEN];

    string now = StrDate::getTimestamp();

    try {
        begin();

        for (int i = 0;i < NUM_CURRENCIES;i++) {
            snprintf(
                statement, 
                SQL_STATEMENT_BUFFER_LEN, 
                insertStatement, 
                currencies[i][0], 
                currencies[i][1], 
                currencies[i][2], 
                now.c_str(), 
                now.c_str());

            executeInsert(statement);
        }

        commit();
    }
    catch (pfm_error & e) {
        rollback();

        log.logError("Failed to insert currencies");
        throw pfm_error("Failed to insert currencies", __FILE__, __LINE__);
    }

    log.logExit("PFM_DB::createCurrencies()");
}

int PFM_DB::executeSelect(const char * statement, vector<DBRow> * rows) {
    log.logEntry("PFM_DB::executeSelect()");

    _executeSQLCallback(statement, rows);

    log.logDebug("Execute SELECT returned %d rows", rows->size());

    log.logExit("PFM_DB::executeSelect()");

    return rows->size();
}

pfm_id_t PFM_DB::executeInsert(const char * statement) {
    log.logEntry("PFM_DB::executeInsert()");

    _executeSQLNoCallback(statement);

    log.logExit("PFM_DB::executeInsert()");

    return sqlite3_last_insert_rowid(dbHandle);
}

void PFM_DB::executeUpdate(const char * statement) {
    log.logEntry("PFM_DB::executeUpdate()");

    _executeSQLNoCallback(statement);

    log.logExit("PFM_DB::executeUpdate()");
}

void PFM_DB::executeDelete(const char * statement) {
    log.logEntry("PFM_DB::executeDelete()");

    _executeSQLNoCallback(statement);

    log.logExit("PFM_DB::executeDelete()");
}

void PFM_DB::begin() {
    log.logEntry("PFM_DB::begin()");

    if (getIsTransactionActive()) {
        log.logInfo("Begin transaction - transaction already active, skipping");
        return;
    }

    log.logDebug("BEGIN TRANSACTION");

    char * pszErrorMsg;
    int error = sqlite3_exec(dbHandle, "BEGIN DEFERRED TRANSACTION;", NULL, NULL, &pszErrorMsg);

    if (error) {
        log.logError("Failed to begin transaction with error '%s'", pszErrorMsg);

        throw pfm_error(
            pfm_error::buildMsg(
                "BEGIN TRANSACTION failed: %s", 
                pszErrorMsg), 
            __FILE__, 
            __LINE__);
    }

    setIsTransactionActive();

    log.logExit("PFM_DB::begin()");
}

void PFM_DB::commit() {
    log.logEntry("PFM_DB::commit()");

    if (!getIsTransactionActive()) {
        log.logInfo("Commit transaction - no transaction active, skipping");
        return;
    }
    
    log.logDebug("COMMIT TRANSACTION");

    char * pszErrorMsg;
    int error = sqlite3_exec(dbHandle, "COMMIT TRANSACTION;", NULL, NULL, &pszErrorMsg);

    if (error) {
        log.logError("Failed to commit transaction with error '%s'", pszErrorMsg);

        throw pfm_error(
            pfm_error::buildMsg(
                "COMMIT TRANSACTION failed: %s", 
                pszErrorMsg), 
            __FILE__, 
            __LINE__);
    }

    clearIsTransactionActive();

    log.logExit("PFM_DB::commit()");
}

void PFM_DB::rollback() {
    log.logEntry("PFM_DB::rollback()");

    if (!getIsTransactionActive()) {
        log.logInfo("Rollback transaction - no transaction active, skipping");
        return;
    }

    log.logDebug("ROLLBACK TRANSACTION");

    char * pszErrorMsg;
    int error = sqlite3_exec(dbHandle, "ROLLBACK TRANSACTION;", NULL, NULL, &pszErrorMsg);

    if (error) {
        log.logError("Failed to rollback transaction with error '%s'", pszErrorMsg);

        throw pfm_error(
            pfm_error::buildMsg(
                "ROLLBACK TRANSACTION failed: %s", 
                pszErrorMsg), 
            __FILE__, 
            __LINE__);
    }

    clearIsTransactionActive();

    log.logExit("PFM_DB::rollback()");
}

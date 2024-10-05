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

#include "logger.h"
#include "db.h"
#include "strdate.h"
#include "pfm_error.h"
#include "schema.h"

using namespace std;


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

static string getPassword() {
    printf("Enter password: ");

    string password;
	int	ch = 0;

    while (ch != '\n') {
        ch = __getch();

        if (ch != '\n' && ch != '\r') {
            putchar('*');
            fflush(stdout);

            password += (char)ch;
        }
    }

    putchar('\n');
    fflush(stdout);

    return password;
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

bool PFM_DB::open(string dbName) {
    log.logEntry("PFM_DB::open()");

    int error = sqlite3_open_v2(
                    dbName.c_str(), 
                    &this->dbHandle, 
                    SQLITE_OPEN_READWRITE, 
                    NULL);

    if (error == SQLITE_OK) {
        // string password = getPassword();
        string password = "Cal14rnia";
        sqlite3_key(this->dbHandle, password.c_str(), password.length());
    }
    else {
        if ((error & 0x000000FF) ==  SQLITE_CANTOPEN) {
            error = sqlite3_open_v2(
                            dbName.c_str(),
                            &this->dbHandle,
                            SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                            NULL);

            if (error == SQLITE_OK) {
                string password = getPassword();
                sqlite3_key(this->dbHandle, password.c_str(), password.length());

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

    log.logExit("PFM_DB::open()");

    return true;
}

PFM_DB::~PFM_DB() {
    log.logEntry("PFM_DB::~PFM_DB()");
    sqlite3_close_v2(this->dbHandle);
    log.logExit("PFM_DB::~PFM_DB()");
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
        createTable(pszCreateCategoryTable);
        createTable(pszCreatePayeeTable);
        createTable(pszCreateRCTable);
        createTable(pszCreateTransationTable);
        createTable(pszCreateCarriedOverTable);
        createTable(pszCreateBudgetTable);
        createTable(pszCreateBudgetTrackTable);

        createView(pszCreateListRCView);
        createView(pszCreateListTransationView);

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

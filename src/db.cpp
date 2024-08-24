#include <iostream>
#include <string>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <sqlite3.h>

#include "logger.h"
#include "db_base.h"
#include "db_config.h"
#include "db_currency.h"
#include "db_category.h"
#include "utils.h"
#include "strdate.h"
#include "pfm_error.h"
#include "schema.h"

using namespace std;

bool PFM_DB::open(string dbName) {
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

PFM_DB::~PFM_DB() {
    sqlite3_close_v2(this->dbHandle);
}

sqlite3 * PFM_DB::getHandle() {
    return this->dbHandle;
}

void PFM_DB::createSchema() {
    int     error = SQLITE_OK;
    char *  pszErrorMsg;

    pszErrorMsg = (char *)sqlite3_malloc(SQLITE_ERROR_BUFFER_LEN);

    try {
        error = sqlite3_exec(
                        dbHandle, 
                        pszCreateConfigTable,
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
                        pszCreateCurrencyTable,
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

        error = sqlite3_exec(
                        dbHandle, 
                        pszCreateCarriedOverTable,
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

        DBCategory category;

        for (int i = 0;i < NUM_DEFAULT_CATEGORIES;i++) {
            category.clear();

            category.code = defaultCategories[i][0];
            category.description = defaultCategories[i][1];

            category.save();
        }

        DBCurrency currency;

        for (int i = 0;i < NUM_CURRENCIES;i++) {
            currency.clear();

            currency.code = currencies[i][0];
            currency.name = currencies[i][1];
            currency.symbol = currencies[i][2];

            currency.save();
        }

        DBConfig config;

        for (int i = 0;i < NUM_CONFIG_ITEMS;i++) {
            config.clear();

            config.key = defaultConfig[i][0];
            config.value = defaultConfig[i][1];
            config.description = defaultConfig[i][2];

            config.save();
        }
    }
    catch (pfm_error & e) {
        sqlite3_free(pszErrorMsg);
        throw e;
    }
    
    sqlite3_free(pszErrorMsg);
}

void PFM_DB::begin() {
    char * pszErrorMsg;

    int error = sqlite3_exec(dbHandle, "BEGIN DEFERRED TRANSACTION;", NULL, NULL, &pszErrorMsg);

    if (error) {
        throw pfm_error(
            pfm_error::buildMsg(
                "BEGIN TRANSACTION failed: %s", 
                pszErrorMsg), 
            __FILE__, 
            __LINE__);
    }
}

void PFM_DB::commit() {
    char * pszErrorMsg;

    int error = sqlite3_exec(dbHandle, "COMMIT TRANSACTION;", NULL, NULL, &pszErrorMsg);

    if (error) {
        throw pfm_error(
            pfm_error::buildMsg(
                "COMMIT TRANSACTION failed: %s", 
                pszErrorMsg), 
            __FILE__, 
            __LINE__);
    }
}

void PFM_DB::rollback() {
    char * pszErrorMsg;

    int error = sqlite3_exec(dbHandle, "ROLLBACK TRANSACTION;", NULL, NULL, &pszErrorMsg);

    if (error) {
        throw pfm_error(
            pfm_error::buildMsg(
                "ROLLBACK TRANSACTION failed: %s", 
                pszErrorMsg), 
            __FILE__, 
            __LINE__);
    }
}

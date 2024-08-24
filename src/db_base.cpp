#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdint.h>

#include <sqlite3.h>

#include "strdate.h"
#include "db.h"
#include "db_base.h"
#include "pfm_error.h"
#include "logger.h"

using namespace std;

pfm_id_t DBEntity::insert() {
    const char * statement = getInsertStatement();

    Logger & log = Logger::getInstance();
    log.logDebug("Executing INSERT statement '%s'", statement);

    PFM_DB & db = PFM_DB::getInstance();

    char * pszErrorMsg;

    int error = sqlite3_exec(db.getHandle(), statement, NULL, NULL, &pszErrorMsg);

    if (error) {
        throw pfm_error(
                pfm_error::buildMsg(
                    "Failed to execute statement '%s': %s",
                    statement, 
                    pszErrorMsg), 
                __FILE__, 
                __LINE__);
    }

    return sqlite3_last_insert_rowid(db.getHandle());
}

void DBEntity::update() {
    const char * statement = getUpdateStatement();

    Logger & log = Logger::getInstance();

    log.logDebug("Executing UPDATE statement '%s'", statement);

    PFM_DB & db = PFM_DB::getInstance();

    char * pszErrorMsg;

    int error = sqlite3_exec(db.getHandle(), statement, NULL, NULL, &pszErrorMsg);

    if (error) {
        throw pfm_error(
                pfm_error::buildMsg(
                    "Failed to execute statement '%s': %s",
                    statement, 
                    pszErrorMsg), 
                __FILE__, 
                __LINE__);
    }
}

void DBEntity::remove() {
    const char * statement = getDeleteStatement();

    PFM_DB & db = PFM_DB::getInstance();

    db.begin();

    beforeRemove();

    Logger & log = Logger::getInstance();

    log.logDebug("Executing DELETE statement '%s'", statement);

    char * pszErrorMsg;

    int error = sqlite3_exec(db.getHandle(), statement, NULL, NULL, &pszErrorMsg);

    if (error) {
        db.rollback();

        throw pfm_error(
                pfm_error::buildMsg(
                    "Failed to execute statement '%s': %s",
                    statement, 
                    pszErrorMsg), 
                __FILE__, 
                __LINE__);
    }

    afterRemove();

    db.commit();
}

void DBEntity::save() {
    PFM_DB & db = PFM_DB::getInstance();
    Logger & log = Logger::getInstance();

    try {
        db.begin();

        if (id == 0) {
            beforeInsert();
            id = insert();
            afterInsert();
        }
        else {
            beforeUpdate();
            update();
            afterUpdate();
        }

        db.commit();
    }
    catch (pfm_error & e) {
        db.rollback();
        log.logError("Failed to save entity with id: %lld", id);

        throw e;
    }
}

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

    return db.executeInsert(statement);
}

void DBEntity::update() {
    const char * statement = getUpdateStatement();

    Logger & log = Logger::getInstance();

    log.logDebug("Executing UPDATE statement '%s'", statement);

    PFM_DB & db = PFM_DB::getInstance();

    db.executeUpdate(statement);
}

void DBEntity::retrieve() {
    retrieve(this->id);
}

void DBEntity::retrieve(pfm_id_t id) {
    const char * statement = getSelectByIDStatement(id);

    Logger & log = Logger::getInstance();

    PFM_DB & db = PFM_DB::getInstance();

    vector<DBRow> rows;

    log.logDebug("Executing SELECT BY id statement '%s'", statement);
    int rowsRetrievedCount = db.executeSelect(statement, &rows);

    if (rowsRetrievedCount != 1) {
        throw pfm_error(
                pfm_error::buildMsg("Expected exactly 1 row, got %d", rowsRetrievedCount), 
                __FILE__, 
                __LINE__);
    }

    DBRow row = rows[0];

    for (size_t i = 0;i < row.getNumColumns();i++) {
        DBColumn column = row.getColumnAt(i);

        assignColumn(column);
    }
    
    /*
    ** We're only returning a single row,
    ** so sequence will always be 1...
    */
    onRowComplete(1);
}

void DBEntity::remove() {
    const char * statement = getDeleteStatement();

    Logger & log = Logger::getInstance();

    PFM_DB & db = PFM_DB::getInstance();

    db.begin();

    beforeRemove();

    log.logDebug("Executing DELETE statement '%s'", statement);
    db.executeDelete(statement);

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

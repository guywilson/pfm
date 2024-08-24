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
    PFM_DB & db = PFM_DB::getInstance();
    return db.executeInsert(getInsertStatement());
}

void DBEntity::update() {
    PFM_DB & db = PFM_DB::getInstance();
    db.executeUpdate(getUpdateStatement());
}

template <class T>
void DBEntity::retrieveByID(T * result) {
    PFM_DB & db = PFM_DB::getInstance();
    int rowsRetrievedCount = db.executeSelect(getSelectByIDStatement(), result);

    if (rowsRetrievedCount != 1) {
        throw pfm_error(
                pfm_error::buildMsg("Expected exactly 1 row, got %d", rowsRetrievedCount), 
                __FILE__, 
                __LINE__);
    }

    set(result->getResultAt(0));
}

void DBEntity::remove() {
    PFM_DB & db = PFM_DB::getInstance();
    Logger & log = Logger::getInstance();

    try {
        db.begin();

        beforeRemove();
        db.executeDelete(getDeleteStatement());
        afterRemove();

        db.commit();
    }
    catch (pfm_error & e) {
        db.rollback();
        log.logError("Failed to save entity with id: %lld", id);

        throw e;
    }
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

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdint.h>

#include <sqlcipher/sqlite3.h>

#include "strdate.h"
#include "db.h"
#include "db_base.h"
#include "pfm_error.h"
#include "logger.h"

using namespace std;

pfm_id_t DBEntity::insert() {
    Logger & log = Logger::getInstance();
    
    log.entry("DBEntity::insert()");

    const char * statement = getInsertStatement();

    log.sql("Executing INSERT statement '%s'", statement);

    PFM_DB & db = PFM_DB::getInstance();

    log.exit("DBEntity::insert()");

    return db.executeInsert(statement);
}

void DBEntity::update() {
    Logger & log = Logger::getInstance();
    
    log.entry("DBEntity::update()");

    const char * statement = getUpdateStatement();

    log.sql("Executing UPDATE statement '%s'", statement);

    PFM_DB & db = PFM_DB::getInstance();

    db.executeUpdate(statement);

    log.exit("DBEntity::update()");
}

uint64_t DBEntity::findSingleQuotePos(string & s, int startingPos = 0) {
    uint64_t pos = s.find(SINGLE_QUOTE_CHAR, startingPos);

    if (pos != string::npos) {
        /*
        ** If the single quote is before the end of the string, check that 
        ** we haven't already got a double single quote. Otherwise, if the 
        ** single quote is at the end of the string, return the position. 
        ** This avoids out-of-range run-time exceptions...
        */
        if (pos < s.length() - 1) {
            if (s.at(pos + 1) != SINGLE_QUOTE_CHAR) {
                return pos;
            }
        }
        else {
            return pos;
        }
    }

    return string::npos;
}

string DBEntity::delimitSingleQuotes(string & s) {
    string delimited = s;

    uint64_t searchPos = findSingleQuotePos(delimited);

    while (searchPos != string::npos) {
        delimited.insert(searchPos, 1, SINGLE_QUOTE_CHAR);
        searchPos = findSingleQuotePos(delimited, searchPos + 2);
    }

    return delimited;
}

void DBEntity::retrieve() {
    retrieve(this->id);
}

void DBEntity::retrieve(pfm_id_t id) {
    Logger & log = Logger::getInstance();
    
    log.entry("DBEntity::retrieve()");

    const char * statement = getSelectByIDStatement(id);

    PFM_DB & db = PFM_DB::getInstance();

    vector<DBRow> rows;

    log.sql("Executing SELECT BY id statement '%s'", statement);
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

    log.exit("DBEntity::retrieve()");
}

void DBEntity::remove() {
    Logger & log = Logger::getInstance();
    
    log.entry("DBEntity::remove()");

    const char * statement = getDeleteByIDStatement(id);

    PFM_DB & db = PFM_DB::getInstance();

    try {
        db.begin();

        beforeRemove();

        log.sql("Executing DELETE statement '%s'", statement);
        db.executeDelete(statement);

        afterRemove();

        db.commit();
    }
    catch (pfm_error & e) {
        db.rollback();
        log.error("Failed to remove entity with id: %s", id.c_str());

        throw e;
    }

    log.exit("DBEntity::remove()");
}

void DBEntity::remove(const char * statement) {
    Logger & log = Logger::getInstance();
    
    log.entry("DBEntity::remove()");

    PFM_DB & db = PFM_DB::getInstance();

    try {
        db.begin();

        log.sql("Executing DELETE statement '%s'", statement);
        db.executeDelete(statement);

        db.commit();
    }
    catch (pfm_error & e) {
        db.rollback();
        log.error("Failed to remove entities with sql statement '%s'", statement);

        throw e;
    }

    log.exit("DBEntity::remove()");
}

void DBEntity::removeAll() {
    Logger & log = Logger::getInstance();
    
    log.entry("DBEntity::remove()");

    const char * statement = getDeleteAllStatement();

    PFM_DB & db = PFM_DB::getInstance();

    try {
        db.begin();

        log.sql("Executing DELETE statement '%s'", statement);
        db.executeDelete(statement);

        db.commit();
    }
    catch (pfm_error & e) {
        db.rollback();
        log.error("Failed to remove entity with id: %s", id.c_str());

        throw e;
    }

    log.exit("DBEntity::remove()");
}

void DBEntity::save() {
    Logger & log = Logger::getInstance();
    
    log.entry("DBEntity::save()");

    PFM_DB & db = PFM_DB::getInstance();

    try {
        db.begin();

        beforeSave();

        if (id.isNull()) {
            beforeInsert();
            id = insert();
            afterInsert();
        }
        else {
            beforeUpdate();
            update();
            afterUpdate();
        }

        afterSave();

        db.commit();
    }
    catch (pfm_error & e) {
        db.rollback();
        log.error("Failed to save entity with id: %s", id.c_str());

        throw e;
    }

    log.exit("DBEntity::save()");
}

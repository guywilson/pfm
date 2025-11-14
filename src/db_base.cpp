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

    const string statement = getInsertStatement();

    log.sql("Executing INSERT statement '%s'", statement.c_str());

    PFM_DB & db = PFM_DB::getInstance();

    log.exit("DBEntity::insert()");

    return db.executeInsert(statement);
}

void DBEntity::update() {
    Logger & log = Logger::getInstance();
    
    log.entry("DBEntity::update()");

    const string statement = getUpdateStatement();

    log.sql("Executing UPDATE statement '%s'", statement.c_str());

    PFM_DB & db = PFM_DB::getInstance();

    db.executeUpdate(statement);

    log.exit("DBEntity::update()");
}

uint64_t DBEntity::findSingleQuotePos(string & s, int startingPos = 0) const {
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

const string DBEntity::delimitSingleQuotes(string & s) const {
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

void DBEntity::retrieve(pfm_id_t & id) {
    Logger & log = Logger::getInstance();
    
    log.entry("DBEntity::retrieve()");

    const string statement = getSelectByIDStatement(id);

    PFM_DB & db = PFM_DB::getInstance();

    vector<DBRow> rows;

    log.debug("Executing SELECT BY id statement '%s'", statement.c_str());
    int rowsRetrievedCount = db.executeSelect(statement, &rows);

    if (rowsRetrievedCount != 1) {
        throw pfm_error(
                pfm_error::buildMsg("Expected exactly 1 row, got %d", rowsRetrievedCount), 
                __FILE__, 
                __LINE__);
    }

    /*
    ** As we're retrieving by ID, there will be only 1 row...
    */
    DBRow row = rows[0];

    /*
    ** Call the assignColumn() method to populate the object...
    */
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

    const string statement = getDeleteByIDStatement(id);

    PFM_DB & db = PFM_DB::getInstance();

    try {
        db.begin();

        beforeRemove();

        log.debug("Executing DELETE statement '%s'", statement.c_str());
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

void DBEntity::remove(const string & statement) {
    Logger & log = Logger::getInstance();
    
    log.entry("DBEntity::remove()");

    PFM_DB & db = PFM_DB::getInstance();

    try {
        db.begin();

        log.debug("Executing DELETE statement '%s'", statement.c_str());
        db.executeDelete(statement);

        db.commit();
    }
    catch (pfm_error & e) {
        db.rollback();
        log.error("Failed to remove entities with sql statement '%s'", statement.c_str());

        throw e;
    }

    log.exit("DBEntity::remove()");
}

void DBEntity::removeAll() {
    Logger & log = Logger::getInstance();
    
    log.entry("DBEntity::remove()");

    const string statement = getDeleteAllStatement();

    PFM_DB & db = PFM_DB::getInstance();

    try {
        db.begin();

        log.debug("Executing DELETE statement '%s'", statement.c_str());
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

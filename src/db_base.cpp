#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdint.h>

#include <sqlite3.h>

#include "db_new.h"
#include "db_base.h"

using namespace std;

sqlite3_int64 DBEntity::insert() {
    PFM_DB_NEW & db = PFM_DB_NEW::getInstance();
    return db.executeInsert(getInsertStatement());
}

void DBEntity::update() {
    PFM_DB_NEW & db = PFM_DB_NEW::getInstance();
    return db.executeUpdate(getUpdateStatement());
}

void DBEntity::remove() {
    PFM_DB_NEW & db = PFM_DB_NEW::getInstance();
    return db.executeDelete(getDeleteStatement());
}

void DBEntity::save() {
    if (id == 0) {
        id = insert();
    }
    else {
        update();
    }
}

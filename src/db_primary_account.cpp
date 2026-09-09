/*
 * Copyright (C) 2025-2026 Guy Wilson
 *
 * This file is part of PFM.
 *
 * PFM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PFM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PFM. If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <iomanip>
#include <string>
#include <string.h>
#include <stdio.h>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db_primary_account.h"
#include "db.h"
#include "strdate.h"


std::string DBPrimaryAccount::getPrimaryAccountCode() {
    Logger & log = Logger::getInstance();
    log.entry("DBPrimaryAccount::getPrimaryAccountCode()");

    DBResult<DBPrimaryAccount> primaryAccounts;

    int numPrimaryAccounts = primaryAccounts.retrieveAll();

    if (numPrimaryAccounts == 1) {
        DBPrimaryAccount account = primaryAccounts.at(0);

        return account.code;
    }
    else {
        throw pfm_error(
                pfm_error::buildMsg(
                    "DBPrimaryAccount::getPrimaryAccountCode(): There should be one and only one primary account, found %d records", 
                    numPrimaryAccounts),
                __FILE__,
                __LINE__);
    }

    log.exit("DBPrimaryAccount::getPrimaryAccountCode()");
}

void DBPrimaryAccount::setPrimaryAccount(std::string & accountCode) {
    Logger & log = Logger::getInstance();
    log.entry("DBPrimaryAccount::setPrimaryAccount()");

    DBResult<DBPrimaryAccount> primaryAccounts;

    int numPrimaryAccounts = primaryAccounts.retrieveAll();

    if (numPrimaryAccounts == 1) {
        DBPrimaryAccount account = primaryAccounts.at(0);

        account.code = accountCode;
        account.save();
    }
    else {
        throw pfm_error(
                pfm_error::buildMsg(
                    "DBPrimaryAccount::setPrimaryAccount(): There should be one and only one primary account, found %d records", 
                    numPrimaryAccounts),
                __FILE__,
                __LINE__);
    }

    log.exit("DBPrimaryAccount::setPrimaryAccount()");
}

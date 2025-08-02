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

using namespace std;

string DBPrimaryAccount::getPrimaryAccountCode() {
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
}

void DBPrimaryAccount::setPrimaryAccount(string & accountCode) {
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
}

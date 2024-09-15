#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db_user.h"
#include "db.h"
#include "strdate.h"

using namespace std;

void DBUser::retrieveByUser(string & user) {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBUser> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByUser, 
        user.c_str());

    int rowsRetrievedCount = result.retrieve(szStatement);

    if (rowsRetrievedCount == 1) {
        set(result.getResultAt(0));
    }
    else {
        throw pfm_error(getValidationErrorMsg());
    }
}

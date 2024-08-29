#include <string>

#include "logger.h"
#include "pfm_error.h"
#include "db_budget.h"
#include "db_base.h"
#include "db_category.h"
#include "db_payee.h"
#include "db.h"
#include "strdate.h"

using namespace std;

DBResult<DBBudget> DBBudget::retrieveByPayeeCode(string & code) {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBBudget> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByPayeeCode, 
        code.c_str());

    result.retrieve(szStatement);

    return result;
}

DBResult<DBBudget> DBBudget::retrieveByCategoryCode(string & code) {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBBudget> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByCategoryCode, 
        code.c_str());

    result.retrieve(szStatement);

    return result;
}

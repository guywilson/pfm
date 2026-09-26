#include <string>

#include "db_base.h"
#include "db.h"
#include "logger.h"
#include "db_v_carried_over.h"

DBResult<DBCarriedOverView> DBCarriedOverView::retrieveByAccountCode(const std::string & accountCode) {
    DBCriteria criteria;
    criteria.add(Columns::accountCode, DBCriteria::sql_operator::equal_to, accountCode);

    std::string statement = getSelectStatement() + criteria.getStatementCriteria();

    DBResult<DBCarriedOverView> results;
    results.retrieve(statement);

    return results;
}

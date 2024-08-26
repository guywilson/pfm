#include <string>

#include "db_criteria.h"

using namespace std;

#ifndef __INCL_UTILS
#define __INCL_UTILS

int         readCriteria(
                const char * pszField, 
                const db_column_type & type, 
                bool isFirst, 
                const db_operator acceptedOperators[], 
                int numCriteria, 
                DBCriteria * pCriteria);

#endif

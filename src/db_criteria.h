#include <string>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdint.h>

#include <sqlite3.h>

#include "pfm_error.h"

using namespace std;

#ifndef __INCL_DB_QUERY
#define __INCL_DB_QUERY

typedef enum {
    less_than,
    less_than_or_equal_to,
    greater_than,
    greater_than_or_equal_to,
    equals,
    not_equals,
    like,
    unknown = -1
}
db_operator;

typedef enum {
    date,
    text,
    numeric
}
db_column_type;

class DBCriteria {
    public:
        string              columnName;
        db_column_type      columnType;
        db_operator         operation;
        string              value;

        static db_operator  getOperator(char * pszCriteria) {
            db_operator op;

            switch (pszCriteria[0]) {
                case '<':
                    if (pszCriteria[1] == '=') {
                        op = db_operator::less_than_or_equal_to;
                    }
                    else {
                        op = db_operator::less_than;
                    }
                    break;

                case '>':
                    if (pszCriteria[1] == '=') {
                        op = db_operator::greater_than_or_equal_to;
                    }
                    else {
                        op = db_operator::greater_than;
                    }
                    break;

                case '=':
                    op = db_operator::equals;
                    break;

                case '!':
                    if (pszCriteria[1] == '=') {
                        op = db_operator::not_equals;
                    }
                    else {
                        throw pfm_validation_error("Invalid operator found");
                    }
                    break;

                case '~':
                    op = db_operator::like;
                    break;

                default:
                    op = db_operator::unknown;
                    break;
            }

            return op;
        }

        static string  getValue(char * pszCriteria) {
            int         i = 0;
            char        ch;

            for (i = 0; i < strlen(pszCriteria);i++) {
                ch = pszCriteria[i];

                if (isalnum(ch) || ch == '.' || ch == '-') {
                    return string(&pszCriteria[i]);
                }
            }

            return string("");
        }
};

#endif

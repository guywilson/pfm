#include <iostream>
#include <string>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <sqlite3.h>

#include "account.h"
#include "category.h"
#include "payee.h"
#include "db.h"
#include "pfm_error.h"
#include "schema.h"

#define SQLITE_ERROR_BUFFER_LEN                     512
#define SQL_STATEMENT_BUFFER_LEN                    256

using namespace std;

static int accountCallback(void * p, int numColumns, char ** columns, char ** columnNames) {
    int                     columnIndex = 0;
    Account                 account;
    AccountResult *         result = (AccountResult *)p;

    while (columnIndex < numColumns) {
        if (strncmp(&columnNames[columnIndex][0], "id", 2) == 0) {
            account.id= strtoll(&columns[columnIndex][0], NULL, 10);
        }
        else if (strncmp(&columnNames[columnIndex][0], "name", 4) == 0) {
            account.name.assign(&columns[columnIndex][0]);
        }
        else if (strncmp(&columnNames[columnIndex][0], "code", 4) == 0) {
            account.code.assign(&columns[columnIndex][0]);
        }
        else if (strncmp(&columnNames[columnIndex][0], "opening_balance", 16) == 0) {
            account.openingBalance = strtod(&columns[columnIndex][0], NULL);
        }
        else if (strncmp(&columnNames[columnIndex][0], "current_balance", 16) == 0) {
            account.currentBalance = strtod(&columns[columnIndex][0], NULL);
        }

        columnIndex++;
    }

    result->results.push_back(account);
    result->numRows++;

    return SQLITE_OK;
}

static int categoryCallback(void * p, int numColumns, char ** columns, char ** columnNames) {
    int                     columnIndex = 0;
    Category                category;
    CategoryResult *        result = (CategoryResult *)p;

    while (columnIndex < numColumns) {
        if (strncmp(&columnNames[columnIndex][0], "id", 2) == 0) {
            category.id= strtoll(&columns[columnIndex][0], NULL, 10);
        }
        else if (strncmp(&columnNames[columnIndex][0], "code", 4) == 0) {
            category.code.assign(&columns[columnIndex][0]);
        }
        else if (strncmp(&columnNames[columnIndex][0], "description", 12) == 0) {
            category.description.assign(&columns[columnIndex][0]);
        }

        columnIndex++;
    }

    result->results.push_back(category);
    result->numRows++;

    return SQLITE_OK;
}

static int payeeCallback(void * p, int numColumns, char ** columns, char ** columnNames) {
    int                     columnIndex = 0;
    Payee                   payee;
    PayeeResult *           result = (PayeeResult *)p;

    while (columnIndex < numColumns) {
        if (strncmp(&columnNames[columnIndex][0], "id", 2) == 0) {
            payee.id= strtoll(&columns[columnIndex][0], NULL, 10);
        }
        else if (strncmp(&columnNames[columnIndex][0], "code", 4) == 0) {
            payee.code.assign(&columns[columnIndex][0]);
        }
        else if (strncmp(&columnNames[columnIndex][0], "name", 4) == 0) {
            payee.name.assign(&columns[columnIndex][0]);
        }

        columnIndex++;
    }

    result->results.push_back(payee);
    result->numRows++;

    return SQLITE_OK;
}

bool PFM_DB::open(string dbName) {
    int error = sqlite3_open_v2(
                    dbName.c_str(), 
                    &this->dbHandle, 
                    SQLITE_OPEN_READWRITE, 
                    NULL);

    if (error != SQLITE_OK) {
        if ((error & 0x000000FF) ==  SQLITE_CANTOPEN) {
            error = sqlite3_open_v2(
                            dbName.c_str(),
                            &this->dbHandle,
                            SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                            NULL);

            if (error == SQLITE_OK) {
                cout << "Data file '" << dbName << "' does not exist, creating..." << endl;
                createSchema();
            }
            else {
                throw pfm_fatal(
                        pfm_fatal::buildMsg(
                            "Cannot open database file %s, aborting: %d:%s", 
                            dbName.c_str(),
                            error,
                            sqlite3_errmsg(this->dbHandle)));
            }
        }
    }

    return true;
}

PFM_DB::~PFM_DB() {
    sqlite3_close_v2(this->dbHandle);
}

void PFM_DB::createSchema() {
    int     error = SQLITE_OK;
    char *  pszErrorMsg;

    pszErrorMsg = (char *)sqlite3_malloc(SQLITE_ERROR_BUFFER_LEN);

    try {
        error = sqlite3_exec(
                        dbHandle, 
                        pszCreateAccountTable,
                        NULL,
                        NULL,
                        &pszErrorMsg);

        if (error) {
            throw pfm_error(
                pfm_error::buildMsg(
                    "Execute failed in createSchema(): %s", 
                    pszErrorMsg), 
                __FILE__, 
                __LINE__);
        }

        error = sqlite3_exec(
                        dbHandle, 
                        pszCreateCategoryTable,
                        NULL,
                        NULL,
                        &pszErrorMsg);

        if (error) {
            throw pfm_error(
                pfm_error::buildMsg(
                    "Execute failed in createSchema(): %s", 
                    pszErrorMsg), 
                __FILE__, 
                __LINE__);
        }        

        error = sqlite3_exec(
                        dbHandle, 
                        pszCreatePayeeTable,
                        NULL,
                        NULL,
                        &pszErrorMsg);

        if (error) {
            throw pfm_error(
                pfm_error::buildMsg(
                    "Execute failed in createSchema(): %s", 
                    pszErrorMsg), 
                __FILE__, 
                __LINE__);
        }        

        error = sqlite3_exec(
                        dbHandle, 
                        pszCreateRCTable,
                        NULL,
                        NULL,
                        &pszErrorMsg);

        if (error) {
            throw pfm_error(
                pfm_error::buildMsg(
                    "Execute failed in createSchema(): %s", 
                    pszErrorMsg), 
                __FILE__, 
                __LINE__);
        }        

        error = sqlite3_exec(
                        dbHandle, 
                        pszCreateTransationTable,
                        NULL,
                        NULL,
                        &pszErrorMsg);

        if (error) {
            throw pfm_error(
                pfm_error::buildMsg(
                    "Execute failed in createSchema(): %s", 
                    pszErrorMsg), 
                __FILE__, 
                __LINE__);
        }

        Category category;

        for (int i = 0;i < NUM_DEFAULT_CATEGORIES;i++) {
            category.clear();

            category.code = defaultCategories[i][0];
            category.description = defaultCategories[i][1];

            createCategory(category);
        }
    }
    catch (pfm_error & e) {
        sqlite3_free(pszErrorMsg);
        throw e;
    }
    
    sqlite3_free(pszErrorMsg);
}

sqlite3_int64 PFM_DB::createAccount(Account & account) {
    char *          pszErrorMsg;
    char *          pszInsertStatement;
    int             error;

    pszErrorMsg = (char *)sqlite3_malloc(SQLITE_ERROR_BUFFER_LEN);
    pszInsertStatement = (char *)sqlite3_malloc(SQL_STATEMENT_BUFFER_LEN);

    snprintf(
        pszInsertStatement, 
        SQL_STATEMENT_BUFFER_LEN,
        "INSERT INTO account (name, code, opening_balance, current_balance) VALUES ('%s', '%s', %.2f, %.2f);",
        account.name.c_str(),
        account.code.c_str(),
        account.openingBalance,
        account.currentBalance);

    error = sqlite3_exec(dbHandle, pszInsertStatement, NULL, NULL, &pszErrorMsg);

    if (error) {
        throw pfm_error(
            pfm_error::buildMsg(
                "Failed to create account %s: %s", 
                account.name.c_str(),
                sqlite3_errmsg(dbHandle)), 
            __FILE__, 
            __LINE__);
    }

    sqlite3_free(pszInsertStatement);
    sqlite3_free(pszErrorMsg);

    return sqlite3_last_insert_rowid(dbHandle);
}

int PFM_DB::getAccounts(AccountResult * result) {
    char *          pszErrorMsg;
    int             error;

    const char * pszStatement = 
                "SELECT id, name, code, opening_balance, current_balance FROM account;";

    error = sqlite3_exec(dbHandle, pszStatement, accountCallback, result, &pszErrorMsg);

    if (error) {
        throw pfm_error(
                pfm_error::buildMsg(
                    "Failed to get accounts list: %s", 
                    pszErrorMsg), 
                __FILE__, 
                __LINE__);
    }

    return result->numRows;
}

int PFM_DB::getAccount(string code, AccountResult * result) {
    char *          pszErrorMsg;
    char            szStatement[SQL_STATEMENT_BUFFER_LEN];
    int             error;

    const char * pszTemplate = 
                "SELECT id, name, code, opening_balance, current_balance FROM account where code = '%s';";

    snprintf(szStatement, SQL_STATEMENT_BUFFER_LEN - 1, pszTemplate, code.c_str());

    error = sqlite3_exec(dbHandle, szStatement, accountCallback, result, &pszErrorMsg);

    if (error) {
        throw pfm_error(
                pfm_error::buildMsg(
                    "Failed to get account: %s", 
                    pszErrorMsg), 
                __FILE__, 
                __LINE__);
    }
    else if (result->numRows != 1) {
        throw pfm_error(
            pfm_error::buildMsg(
                "Expected 1 result, got %d", 
                result->numRows),
            __FILE__,
            __LINE__);
    }

    result->results[0].print();

    return result->numRows;
}

int PFM_DB::updateAccount(Account & account) {
    char *          pszErrorMsg;
    char *          pszUpdateStatement;
    int             error;

    pszErrorMsg = (char *)sqlite3_malloc(SQLITE_ERROR_BUFFER_LEN);
    pszUpdateStatement = (char *)sqlite3_malloc(SQL_STATEMENT_BUFFER_LEN);

    snprintf(
        pszUpdateStatement, 
        SQL_STATEMENT_BUFFER_LEN - 1,
        "UPDATE account SET code = '%s', name = '%s', opening_balance = %.2f, current_balance = %.2f WHERE id = %lld;",
        account.code.c_str(),
        account.name.c_str(),
        account.openingBalance,
        account.currentBalance,
        account.id);

    error = sqlite3_exec(dbHandle, pszUpdateStatement, NULL, NULL, &pszErrorMsg);

    if (error) {
        throw pfm_error(
            pfm_error::buildMsg(
                "Failed to update account with id %lld: %s", 
                account.id,
                sqlite3_errmsg(dbHandle)), 
            __FILE__, 
            __LINE__);
    }

    sqlite3_free(pszUpdateStatement);
    sqlite3_free(pszErrorMsg);

    return 0;
}

int PFM_DB::deleteAccount(Account & account) {
    char *          pszErrorMsg;
    char *          pszDeleteStatement;
    int             error;

    pszErrorMsg = (char *)sqlite3_malloc(SQLITE_ERROR_BUFFER_LEN);
    pszDeleteStatement = (char *)sqlite3_malloc(SQL_STATEMENT_BUFFER_LEN);

    snprintf(
        pszDeleteStatement, 
        SQL_STATEMENT_BUFFER_LEN - 1,
        "DELETE FROM account WHERE id = %lld;",
        account.id);

    error = sqlite3_exec(dbHandle, pszDeleteStatement, NULL, NULL, &pszErrorMsg);

    if (error) {
        throw pfm_error(
            pfm_error::buildMsg(
                "Failed to delete account with id %lld: %s", 
                account.id,
                sqlite3_errmsg(dbHandle)), 
            __FILE__, 
            __LINE__);
    }

    sqlite3_free(pszDeleteStatement);
    sqlite3_free(pszErrorMsg);

    return 0;
}

sqlite3_int64 PFM_DB::createCategory(Category & category) {
    char *          pszErrorMsg;
    char *          pszInsertStatement;
    int             error;

    pszErrorMsg = (char *)sqlite3_malloc(SQLITE_ERROR_BUFFER_LEN);
    pszInsertStatement = (char *)sqlite3_malloc(SQL_STATEMENT_BUFFER_LEN);

    snprintf(
        pszInsertStatement, 
        SQL_STATEMENT_BUFFER_LEN,
        "INSERT INTO category (description, code) VALUES ('%s', '%s');",
        category.description.c_str(),
        category.code.c_str());

    error = sqlite3_exec(dbHandle, pszInsertStatement, NULL, NULL, &pszErrorMsg);

    if (error) {
        throw pfm_error(
            pfm_error::buildMsg(
                "Failed to create account %s: %s", 
                category.description.c_str(),
                sqlite3_errmsg(dbHandle)), 
            __FILE__, 
            __LINE__);
    }

    sqlite3_free(pszInsertStatement);
    sqlite3_free(pszErrorMsg);

    return sqlite3_last_insert_rowid(dbHandle);
}

int PFM_DB::getCategories(CategoryResult * result) {
    char *          pszErrorMsg;
    int             error;

    const char * pszStatement = 
                "SELECT id, code, description FROM category;";

    error = sqlite3_exec(dbHandle, pszStatement, categoryCallback, result, &pszErrorMsg);

    if (error) {
        throw pfm_error(
                pfm_error::buildMsg(
                    "Failed to get categories list: %s", 
                    pszErrorMsg), 
                __FILE__, 
                __LINE__);
    }

    return result->numRows;
}

int PFM_DB::getCategory(string code, CategoryResult * result) {
    char *          pszErrorMsg;
    char            szStatement[SQL_STATEMENT_BUFFER_LEN];
    int             error;

    const char * pszTemplate = 
                "SELECT id, code, description FROM category where code = '%s';";

    snprintf(szStatement, SQL_STATEMENT_BUFFER_LEN - 1, pszTemplate, code.c_str());

    error = sqlite3_exec(dbHandle, szStatement, categoryCallback, result, &pszErrorMsg);

    if (error) {
        throw pfm_error(
                pfm_error::buildMsg(
                    "Failed to get category: %s", 
                    pszErrorMsg), 
                __FILE__, 
                __LINE__);
    }
    else if (result->numRows != 1) {
        throw pfm_error(
            pfm_error::buildMsg(
                "Expected 1 result, got %d", 
                result->numRows),
            __FILE__,
            __LINE__);
    }

    result->results[0].print();

    return result->numRows;
}

int PFM_DB::updateCategory(Category & category) {
    char *          pszErrorMsg;
    char *          pszUpdateStatement;
    int             error;

    pszErrorMsg = (char *)sqlite3_malloc(SQLITE_ERROR_BUFFER_LEN);
    pszUpdateStatement = (char *)sqlite3_malloc(SQL_STATEMENT_BUFFER_LEN);

    snprintf(
        pszUpdateStatement, 
        SQL_STATEMENT_BUFFER_LEN - 1,
        "UPDATE category SET code = '%s', description = '%s' WHERE id = %lld;",
        category.code.c_str(),
        category.description.c_str(),
        category.id);

    error = sqlite3_exec(dbHandle, pszUpdateStatement, NULL, NULL, &pszErrorMsg);

    if (error) {
        throw pfm_error(
            pfm_error::buildMsg(
                "Failed to update category with id %lld: %s", 
                category.id,
                sqlite3_errmsg(dbHandle)), 
            __FILE__, 
            __LINE__);
    }

    sqlite3_free(pszUpdateStatement);
    sqlite3_free(pszErrorMsg);

    return 0;
}

int PFM_DB::deleteCategory(Category & category) {
    char *          pszErrorMsg;
    char *          pszDeleteStatement;
    int             error;

    pszErrorMsg = (char *)sqlite3_malloc(SQLITE_ERROR_BUFFER_LEN);
    pszDeleteStatement = (char *)sqlite3_malloc(SQL_STATEMENT_BUFFER_LEN);

    snprintf(
        pszDeleteStatement, 
        SQL_STATEMENT_BUFFER_LEN - 1,
        "DELETE FROM category WHERE id = %lld;",
        category.id);

    error = sqlite3_exec(dbHandle, pszDeleteStatement, NULL, NULL, &pszErrorMsg);

    if (error) {
        throw pfm_error(
            pfm_error::buildMsg(
                "Failed to delete category with id %lld: %s", 
                category.id,
                sqlite3_errmsg(dbHandle)), 
            __FILE__, 
            __LINE__);
    }

    sqlite3_free(pszDeleteStatement);
    sqlite3_free(pszErrorMsg);

    return 0;
}

sqlite3_int64 PFM_DB::createPayee(Payee & payee) {
    char *          pszErrorMsg;
    char *          pszInsertStatement;
    int             error;

    pszErrorMsg = (char *)sqlite3_malloc(SQLITE_ERROR_BUFFER_LEN);
    pszInsertStatement = (char *)sqlite3_malloc(SQL_STATEMENT_BUFFER_LEN);

    snprintf(
        pszInsertStatement, 
        SQL_STATEMENT_BUFFER_LEN,
        "INSERT INTO payee (name, code) VALUES ('%s', '%s');",
        payee.name.c_str(),
        payee.code.c_str());

    error = sqlite3_exec(dbHandle, pszInsertStatement, NULL, NULL, &pszErrorMsg);

    if (error) {
        throw pfm_error(
            pfm_error::buildMsg(
                "Failed to create payee %s: %s", 
                payee.name.c_str(),
                sqlite3_errmsg(dbHandle)), 
            __FILE__, 
            __LINE__);
    }

    sqlite3_free(pszInsertStatement);
    sqlite3_free(pszErrorMsg);

    return sqlite3_last_insert_rowid(dbHandle);
}

int PFM_DB::getPayees(PayeeResult * result) {
    char *          pszErrorMsg;
    int             error;

    const char * pszStatement = 
                "SELECT id, code, name FROM payee;";

    error = sqlite3_exec(dbHandle, pszStatement, payeeCallback, result, &pszErrorMsg);

    if (error) {
        throw pfm_error(
                pfm_error::buildMsg(
                    "Failed to get payees list: %s", 
                    pszErrorMsg), 
                __FILE__, 
                __LINE__);
    }

    return result->numRows;
}

int PFM_DB::getPayee(string code, PayeeResult * result) {
    char *          pszErrorMsg;
    char            szStatement[SQL_STATEMENT_BUFFER_LEN];
    int             error;

    const char * pszTemplate = 
                "SELECT id, code, name FROM payee where code = '%s';";

    snprintf(szStatement, SQL_STATEMENT_BUFFER_LEN - 1, pszTemplate, code.c_str());

    error = sqlite3_exec(dbHandle, szStatement, payeeCallback, result, &pszErrorMsg);

    if (error) {
        throw pfm_error(
                pfm_error::buildMsg(
                    "Failed to get payee: %s", 
                    pszErrorMsg), 
                __FILE__, 
                __LINE__);
    }
    else if (result->numRows != 1) {
        throw pfm_error(
            pfm_error::buildMsg(
                "Expected 1 result, got %d", 
                result->numRows),
            __FILE__,
            __LINE__);
    }

    result->results[0].print();

    return result->numRows;
}

int PFM_DB::updatePayee(Payee & payee) {
    char *          pszErrorMsg;
    char *          pszUpdateStatement;
    int             error;

    pszErrorMsg = (char *)sqlite3_malloc(SQLITE_ERROR_BUFFER_LEN);
    pszUpdateStatement = (char *)sqlite3_malloc(SQL_STATEMENT_BUFFER_LEN);

    snprintf(
        pszUpdateStatement, 
        SQL_STATEMENT_BUFFER_LEN - 1,
        "UPDATE payee SET code = '%s', name = '%s' WHERE id = %lld;",
        payee.code.c_str(),
        payee.name.c_str(),
        payee.id);

    error = sqlite3_exec(dbHandle, pszUpdateStatement, NULL, NULL, &pszErrorMsg);

    if (error) {
        throw pfm_error(
            pfm_error::buildMsg(
                "Failed to update payee with id %lld: %s", 
                payee.id,
                sqlite3_errmsg(dbHandle)), 
            __FILE__, 
            __LINE__);
    }

    sqlite3_free(pszUpdateStatement);
    sqlite3_free(pszErrorMsg);

    return 0;
}

int PFM_DB::deletePayee(Payee & payee) {
    char *          pszErrorMsg;
    char *          pszDeleteStatement;
    int             error;

    pszErrorMsg = (char *)sqlite3_malloc(SQLITE_ERROR_BUFFER_LEN);
    pszDeleteStatement = (char *)sqlite3_malloc(SQL_STATEMENT_BUFFER_LEN);

    snprintf(
        pszDeleteStatement, 
        SQL_STATEMENT_BUFFER_LEN - 1,
        "DELETE FROM payee WHERE id = %lld;",
        payee.id);

    error = sqlite3_exec(dbHandle, pszDeleteStatement, NULL, NULL, &pszErrorMsg);

    if (error) {
        throw pfm_error(
            pfm_error::buildMsg(
                "Failed to delete payee with id %lld: %s", 
                payee.id,
                sqlite3_errmsg(dbHandle)), 
            __FILE__, 
            __LINE__);
    }

    sqlite3_free(pszDeleteStatement);
    sqlite3_free(pszErrorMsg);

    return 0;
}

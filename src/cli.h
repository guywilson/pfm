#include <string>

#include <sqlite3.h>

#include "db_account.h"
#include "db_category.h"
#include "db_payee.h"
#include "db_transaction.h"
#include "db_budget.h"
#include "db_user.h"
#include "db_carried_over.h"
#include "db_budget_track.h"

using namespace std;

#ifndef __INCL_CLI
#define __INCL_CLI

#define FIELD_STRING_LEN                        64
#define MAX_PROMPT_LENGTH                      128
#define AMOUNT_FIELD_STRING_LEN                 16

DBUser              addUser();
DBUser              login();

void                addAccount(DBUser & user);
void                listAccounts();
DBAccount           chooseAccount(const char * szAccountCode);
void                updateAccount(DBAccount & account);
void                showAccountBalances(DBAccount & account);

void                addCategory();
void                listCategories();
DBCategory          getCategory(const char * pszCategoryCode);
void                updateCategory(DBCategory & category);
void                deleteCategory(DBCategory & category);

void                addPayee();
void                listPayees();
DBPayee             getPayee(const char * pszPayeeCode);
void                updatePayee(DBPayee & payee);
void                deletePayee(DBPayee & payee);

void                addRecurringCharge(DBAccount & account);
void                listRecurringCharges(DBAccount & account);
DBRecurringCharge   getRecurringCharge(int sequence);
void                updateRecurringCharge(DBRecurringCharge & charge);
void                deleteRecurringCharge(DBRecurringCharge & charge);

void                addTransaction(DBAccount & account);
void                listTransactions(DBAccount & account);
void                findTransactions(DBAccount & account);
DBTransaction       getTransaction(int sequence);
void                updateTransaction(DBTransaction & transaction);
void                deleteTransaction(DBTransaction & transaction);

void                addBudget();
void                listBudgets();
DBBudget            getBudget(int sequence);
void                updateBudget(DBBudget & budget);
void                deleteBudget(DBBudget & budget);

void                listCarriedOverLogs(DBAccount & account);
void                listBudgetTracks();

void                testAccount(const char * accountCode);

#endif

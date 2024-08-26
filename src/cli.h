#include <string>

#include <sqlite3.h>

#include "db_account.h"
#include "db_category.h"
#include "db_payee.h"
#include "db_transaction.h"

using namespace std;

#ifndef __INCL_CLI
#define __INCL_CLI

#define FIELD_STRING_LEN                        64
#define MAX_PROMPT_LENGTH                      128
#define AMOUNT_FIELD_STRING_LEN                 16

void                add_account();
void                list_accounts();
DBAccount           choose_account(const char * szAccountCode);
void                update_account(DBAccount & account);

void                add_category();
void                list_categories();
DBCategory          get_category(const char * pszCategoryCode);
void                update_category(DBCategory & category);
void                delete_category(DBCategory & category);

void                add_payee();
void                list_payees();
DBPayee             get_payee(const char * pszPayeeCode);
void                update_payee(DBPayee & payee);
void                delete_payee(DBPayee & payee);

void                add_recurring_charge(DBAccount & account);
void                list_recurring_charges(DBAccount & account);
DBRecurringCharge   get_recurring_charge(int sequence);
void                update_recurring_charge(DBRecurringCharge & charge);
void                delete_recurring_charge(DBRecurringCharge & charge);

void                add_transaction(DBAccount & account);
void                list_transactions(DBAccount & account);
void                find_transactions(DBAccount & account);
DBTransaction       get_transaction(int sequence);
void                update_transaction(DBTransaction & transaction);
void                delete_transaction(DBTransaction & transaction);

#endif

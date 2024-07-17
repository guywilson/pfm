#include <string>

#include <sqlite3.h>

#include "account.h"
#include "category.h"
#include "payee.h"
#include "transaction.h"

using namespace std;

#ifndef __INCL_CLI
#define __INCL_CLI

#define FIELD_STRING_LEN                        64
#define MAX_PROMPT_LENGTH                      128
#define AMOUNT_FIELD_STRING_LEN                 16

void            add_account(void);
void            list_accounts(void);
Account         choose_account(const char * szAccountCode);
void            update_account(Account & account);
void            delete_account(Account & account);

void            add_category(void);
void            list_categories(void);
Category        get_category(const char * pszCategoryCode);
void            update_category(Category & category);
void            delete_category(Category & category);

void            add_payee(void);
void            list_payees(void);
Payee           get_payee(const char * pszPayeeCode);
void            update_payee(Payee & payee);
void            delete_payee(Payee & payee);

void            add_recurring_charge(Account & account);
void            list_recurring_charges(Account & account);
RecurringCharge get_recurring_charge(int sequence);
void            update_recurring_charge(RecurringCharge & charge);
void            delete_recurring_charge(RecurringCharge & charge);

void            add_transaction(Account & account);
void            list_transactions(Account & account);
Transaction     get_transaction(int sequence);
void            update_transaction(Transaction & transaction);
void            delete_transaction(Transaction & transaction);

#endif

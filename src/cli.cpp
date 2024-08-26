#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sqlite3.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "db.h"
#include "cache.h"
#include "cli.h"
#include "strdate.h"
#include "views.h"
#include "db_account.h"
#include "db_category.h"
#include "db_recurring_charge.h"
#include "db_transaction.h"

using namespace std;

bool validateCreditDebit(const char * pszCD) {
    if (strlen(pszCD) != 2) {
        return false;
    }
    else if (strncmp(pszCD, "CR", 2) != 0 && strncmp(pszCD, "DB", 2) != 0) {
        return false;
    }
    else {
        return true;
    }
}

void add_account() {
    AddAccountView view;
    view.show();

    DBAccount account = view.getAccount();
    account.save();

    cout << "Created account with ID " << account.id << endl;
}

void list_accounts() {
    DBResult<DBAccount> result;
    result.retrieveAll();

    AccountListView view;

    view.addResults(result);
    view.show();
}

DBAccount choose_account(const char * szAccountCode) {
    string accountCode;

    if (szAccountCode == NULL || strlen(szAccountCode) == 0) {
        ChooseAccountView view;
        view.show();

        accountCode = view.getCode();
    }
    else {
        accountCode = szAccountCode;
    }

    DBAccount account;
    account.retrieveByCode(accountCode);
    account.onUseAccountTrigger();
    
    Money balanceAfterBills = account.calculateBalanceAfterBills();

    cout << "Current balance:     " << right << setw(13) << account.currentBalance.getFormattedStringValue() << endl;
    cout << "Balance after bills: " << right << setw(13) << balanceAfterBills.getFormattedStringValue() << endl;

    return account;
}

void update_account(DBAccount & account) {
    UpdateAccountView view;
    view.setAccount(account);
    view.show();

    DBAccount updatedAccount = view.getAccount();

    updatedAccount.save();
}

void add_category() {
    AddCategoryView view;
    view.show();

    DBCategory category = view.getCategory();

    category.save();
}

void list_categories() {
    DBResult<DBCategory> result;
    result.retrieveAll();

    CategoryListView view;
    view.addResults(result);
    view.show();
}

DBCategory get_category(const char * pszCategoryCode) {
    string categoryCode;

    if (pszCategoryCode == NULL || strlen(pszCategoryCode) == 0) {
        ChooseCategoryView view;
        view.show();

        categoryCode = view.getCode();
    }
    else {
        categoryCode = pszCategoryCode;
    }

    DBCategory category;
    category.retrieveByCode(categoryCode);

    return category;
}

void update_category(DBCategory & category) {
    UpdateCategoryView view;
    view.setCategory(category);
    view.show();

    DBCategory updatedCategory = view.getCategory();
    updatedCategory.save();
}

void add_payee() {
    AddPayeeView view;
    view.show();

    DBPayee payee = view.getPayee();

    payee.save();
}

void list_payees() {
    DBResult<DBPayee> result;
    result.retrieveAll();

    PayeeListView view;
    view.addResults(result);
    view.show();
}

DBPayee get_payee(const char * pszPayeeCode) {
    string payeeCode;

    if (pszPayeeCode == NULL || strlen(pszPayeeCode) == 0) {
        ChoosePayeeView view;
        view.show();

        payeeCode = view.getCode();
    }
    else {
        payeeCode = pszPayeeCode;
    }

    DBPayee payee;
    payee.retrieveByCode(payeeCode);

    return payee;
}

void update_payee(DBPayee & payee) {
    UpdatePayeeView view;
    view.setPayee(payee);
    view.show();

    DBPayee updatedPayee = view.getPayee();
    updatedPayee.save();
}

void add_recurring_charge(DBAccount & account) {
    AddRecurringChargeView view;
    view.show();

    DBRecurringCharge charge = view.getRecurringCharge();
    charge.accountId = account.id;

    charge.save();
}

void list_recurring_charges(DBAccount & account) {
    DBRecurringCharge chargeInstance;
    DBResult<DBRecurringCharge> result = chargeInstance.retrieveByAccountID(account.id);

    RecurringChargeListView view;
    view.addResults(result, account.code);
    view.show();

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    for (int i = 0;i < result.getNumRows();i++) {
        DBRecurringCharge charge = result.getResultAt(i);
        cacheMgr.addRecurringCharge(charge.sequence, charge);
    }
}

DBRecurringCharge get_recurring_charge(int sequence) {
    int selectedSequence;

    if (sequence == 0) {
        ChooseRecurringChargeView view;
        view.show();

        selectedSequence = view.getSequence();
    }
    else {
        selectedSequence = sequence;
    }

    CacheMgr & cacheMgr = CacheMgr::getInstance();
    DBRecurringCharge charge = cacheMgr.getRecurringCharge(selectedSequence);

    charge.retrieve();

    return charge;
}

void update_recurring_charge(DBRecurringCharge & charge) {
    UpdateRecurringChargeView view;
    
    view.setRecurringCharge(charge);
    view.show();

    DBRecurringCharge updatedCharge = view.getRecurringCharge();

    updatedCharge.save();
}

void add_transaction(DBAccount & account) {
    AddTransactionView view;
    view.show();

    DBTransaction transaction = view.getTransaction();
    transaction.accountId = account.id;
    transaction.save();
}

void list_transactions(DBAccount & account) {
    DBTransaction transactionInstance;
    DBResult<DBTransaction> result = transactionInstance.retrieveByAccountID(account.id, sort_ascending, 0);

    TransactionListView view;
    view.addResults(result, account.code);
    view.show();

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    for (int i = 0;i < result.getNumRows();i++) {
        DBTransaction transaction = result.getResultAt(i);
        cacheMgr.addTransaction(transaction.sequence, transaction);
    }
}

void find_transactions(DBAccount & account) {
    FindTransactionView findView;
    findView.show();

    string criteria = findView.getCriteria();

    DBTransaction tr;
    DBResult<DBTransaction> result = tr.findTransactionsForAccountID(account.id, criteria);

    TransactionListView view;
    view.addResults(result, account.code);
    view.show();

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    for (int i = 0;i < result.getNumRows();i++) {
        DBTransaction transaction = result.getResultAt(i);
        cacheMgr.addTransaction(transaction.sequence, transaction);
    }
}

DBTransaction get_transaction(int sequence) {
    int selectedSequence;

    if (sequence == 0) {
        ChooseTransactionView view;
        view.show();

        selectedSequence = view.getSequence();
    }
    else {
        selectedSequence = sequence;
    }

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    DBTransaction transaction = cacheMgr.getTransaction(selectedSequence);

    return transaction;
}

void update_transaction(DBTransaction & transaction) {
    UpdateTransactionView view;
    view.setTransaction(transaction);
    view.show();

    DBTransaction updatedTransaction = view.getTransaction();
    updatedTransaction.save();
}

void delete_transaction(DBTransaction & transaction) {
    transaction.remove();
}

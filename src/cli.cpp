#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sqlite3.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "db.h"
#include "utils.h"
#include "cache.h"
#include "cli.h"
#include "strdate.h"
#include "views.h"
#include "db_account.h"
#include "db_category.h"
#include "db_recurring_charge.h"
#include "db_transaction.h"

using namespace std;

void add_account(void) {
    AddAccountView view;
    view.show();

    DBAccount account = view.getAccount();
    account.save();

    cout << "Created account with ID " << account.id << endl;
}

void list_accounts(void) {
    DBAccount accountInstance;
    DBAccountResult result = accountInstance.retrieveAll();

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

    //db.createDueRecurringTransactionsForAccount(account.id);

    return account;
}

void update_account(DBAccount & account) {
    UpdateAccountView view;
    view.setAccount(account);
    view.show();

    DBAccount updatedAccount = view.getAccount();

    updatedAccount.save();
}

void add_category(void) {
    AddCategoryView view;
    view.show();

    DBCategory category = view.getCategory();

    category.save();
}

void list_categories(void) {
    DBCategory category;
    DBCategoryResult result =  category.retrieveAll();

    cout << "*** Categories (" << result.getNumRows() << ") ***" << endl << endl;
    cout << "| Code  | Description               |" << endl;
    cout << "-------------------------------------" << endl;

    for (int i = 0;i < result.getNumRows();i++) {
        DBCategory category = result.getResultAt(i);

        cout << 
            "| " << 
            left << setw(5) << category.code << 
            " | " << 
            left << setw(25) << category.description << 
            " |" <<
            endl;
    }

    cout << endl;
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

void add_payee(void) {
    AddPayeeView view;
    view.show();

    DBPayee payee = view.getPayee();

    payee.save();
}

void list_payees(void) {
    DBPayee payee;
    DBPayeeResult result =  payee.retrieveAll();

    cout << "*** Payees (" << result.getNumRows() << ") ***" << endl << endl;
    cout << "| Code  | Name                      |" << endl;
    cout << "-------------------------------------" << endl;

    for (int i = 0;i < result.getNumRows();i++) {
        DBPayee payee = result.getResultAt(i);

        cout << 
            "| " << 
            left << setw(5) << payee.code << 
            " | " << 
            left << setw(25) << payee.name << 
            " |" <<
            endl;
    }

    cout << endl;
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
    DBRecurringChargeResult result = chargeInstance.retrieveByAccountID(account.id);

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
    charge.retrieveByID(charge.id);

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
    DBTransactionResult result = transactionInstance.retrieveByAccountID(account.id);

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
    int                     numCriteria = 0;
    bool                    isDateValid = false;
    bool                    isCreditDebitValid = false;
    bool                    isReconciledValid = false;
    DBCriteria              criterion[8];
    char                    seq[4];

    cout << "*** Find transactions ***" << endl;

    using_history();
    clear_history();

    DBCategory category;
    DBCategoryResult catResult = category.retrieveAll();

    for (int i = 0;i < catResult.getNumRows();i++) {
        add_history(catResult.getResultAt(i).code.c_str());
    }

    const db_operator ops1[] = {db_operator::equals};
    int hasCategory = readCriteria("category_code", db_column_type::text, true, ops1, 1, &criterion[numCriteria]);

    if (hasCategory) {
        DBCriteria * criteria = &criterion[numCriteria++];

        DBCategory category;
        category.retrieveByCode(criteria->value);

        criteria->columnName = "category_id";
        criteria->columnType = db_column_type::numeric;
        criteria->value = category.id;
    }

    using_history();
    clear_history();

    DBPayee payee;
    DBPayeeResult payResult = payee.retrieveAll();

    for (int i = 0;i < payResult.getNumRows();i++) {
        add_history(payResult.getResultAt(i).code.c_str());
    }

    const db_operator ops2[] = {db_operator::equals};
    int hasPayee = readCriteria("payee_code", db_column_type::text, false, ops2, 1, &criterion[numCriteria]);

    if (hasPayee) {
        DBCriteria * criteria = &criterion[numCriteria++];

        DBPayee payee;
        payee.retrieveByCode(criteria->value);

        criteria->columnName = "payee_id";
        criteria->columnType = db_column_type::numeric;
        criteria->value = payee.id;
    }

    while (!isDateValid) {
        const db_operator ops3[] = {
                    db_operator::equals, 
                    db_operator::less_than, 
                    db_operator::less_than_or_equal_to, 
                    db_operator::greater_than, 
                    db_operator::greater_than_or_equal_to};
        int hasDate = readCriteria("date", db_column_type::text, false, ops3, 5, &criterion[numCriteria]);

        if (hasDate) {
            isDateValid = StrDate::validateDate(criterion[numCriteria].value);

            if (isDateValid) {
                numCriteria++;
            }
        }
        else {
            isDateValid = true;
        }
    }

    const db_operator ops4[] = {db_operator::equals, db_operator::like};
    numCriteria += readCriteria("description", db_column_type::text, false, ops4, 2, &criterion[numCriteria]);

    while (!isCreditDebitValid) {
        const db_operator ops5[] = {db_operator::equals};
        int hasCreditDebit = readCriteria("credit_debit", db_column_type::text, false, ops5, 1, &criterion[numCriteria]);

        if (hasCreditDebit) {
            isCreditDebitValid = validateCreditDebit(criterion[numCriteria].value.c_str());

            if (hasCreditDebit) {
                numCriteria++;
            }
        }
        else {
            isCreditDebitValid = true;
        }
    }

    const db_operator ops6[] = {
                    db_operator::equals, 
                    db_operator::greater_than,
                    db_operator::greater_than_or_equal_to,
                    db_operator::less_than,
                    db_operator::less_than_or_equal_to};
    numCriteria += readCriteria("amount", db_column_type::numeric, false, ops6, 5, &criterion[numCriteria]);

    while (!isReconciledValid) {
        const db_operator ops7[] = {db_operator::equals};
        int hasReconciled = readCriteria("is_reconciled", db_column_type::text, false, ops7, 1, &criterion[numCriteria]);

        if (hasReconciled) {
            isReconciledValid = (criterion[numCriteria].value.at(0) == 'Y' || criterion[numCriteria].value.at(0) == 'N');

            if (isReconciledValid) {
                numCriteria++;
            }
        }
        else {
            isReconciledValid = true;
        }
    }

    DBTransactionResult result;

    if (numCriteria) {
        DBTransaction tr;
        result = tr.findTransactionsForAccountID(account.id, criterion, numCriteria);
    }

    cout << "*** Transactions for account: '" << account.code << "' (" << result.getNumRows() << ") ***" << endl << endl;

    cout << "| Seq | Date       | Description               | Cat.  | Payee | CR/DB | Amount       | Rec |" << endl;
    cout << "---------------------------------------------------------------------------------------------" << endl;

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    for (int i = 0;i < result.getNumRows();i++) {
        DBTransaction transaction = result.getResultAt(i);

        cacheMgr.addTransaction(transaction.sequence, transaction);

        snprintf(seq, 4, "%03d", transaction.sequence);

        cout << 
            "| " << 
            seq <<
            " | " <<
            transaction.date <<
            " | " <<
            left << setw(25) << transaction.description << 
            " | " << 
            left << setw(5) << transaction.category.code << 
            " | " <<
            left << setw(5) << transaction.payee.code <<
            " | " <<
            left << setw(5) << (transaction.isCredit ? "CR" : "DB") <<
            " | " <<
            right << setw(13) << formatCurrency(transaction.amount) <<
            " | " <<
            left << setw(3) << (transaction.isReconciled ? " Y " : " N ") <<
            " |" <<
            endl;
    }

    cout << endl;
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

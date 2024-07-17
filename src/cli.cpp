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
#include "db_account.h"
#include "db_category.h"
#include "db_recurring_charge.h"
#include "db_transaction.h"

using namespace std;

void add_account(void) {
    char *          accountName;
    char *          accountCode;
    char *          openingBalance;
    double          balance;
    sqlite3_int64   accountId;

    cout << "*** Add account ***" << endl;

    accountName = readString("DBAccount name: ", NULL, 32);
    accountCode = readString("DBAccount code (max. 5 chars): ", NULL, 4);
    openingBalance = readString("Opening balance [0.00]: ", "0.00", 32);

    if (strlen(openingBalance) > 0) {
        balance = strtod(openingBalance, NULL);
    }
    else {
        balance = 0.0;
    }

    if (strlen(accountCode) == 0) {
        fprintf(stderr, "\nAccount code must have a value.\n");
        return;
    }

    DBAccount account;
    account.name = accountName;
    account.code = accountCode;
    account.openingBalance = balance;
    account.currentBalance = balance;

    PFM_DB & db = PFM_DB::getInstance();

    accountId = db.createAccount(account);

    cout << "Created account with ID " << accountId << endl;

    free(openingBalance);
    free(accountCode);
    free(accountName);
}

void list_accounts(void) {
    DBAccountResult           result;
    int                     numAccounts;
    int                     i;

    PFM_DB & db = PFM_DB::getInstance();

    numAccounts = db.getAccounts(&result);

    cout << "*** Accounts (" << numAccounts << ") ***" << endl << endl;
    cout << "| Code  | Name                      | Balance      |" << endl;
    cout << "----------------------------------------------------" << endl;

    for (i = 0;i < numAccounts;i++) {
        DBAccount account = result.results[i];

        cout << 
            "| " << 
            left << setw(5) << account.code << 
            " | " << 
            left << setw(25) << account.name << 
            " | " << 
            right << setw(13) << formatCurrency(account.currentBalance) << 
            " |" <<
            endl;
    }

    cout << endl;
}

DBAccount choose_account(const char * szAccountCode) {
    char *          accountCode;
    DBAccountResult   result;

    if (szAccountCode == NULL || strlen(szAccountCode) == 0) {
        cout << "*** Use account ***" << endl;
        accountCode = readString("DBAccount code (max. 4 chars): ", NULL, 4);
    }
    else {
        accountCode = strdup(szAccountCode);
    }

    PFM_DB & db = PFM_DB::getInstance();

    db.getAccount(accountCode, &result);

    free(accountCode);

    return result.results[0];
}

void update_account(DBAccount & account) {
    char            szPrompt[MAX_PROMPT_LENGTH];
    char            szBalance[AMOUNT_FIELD_STRING_LEN];
    char *          pszBalance;

    cout << "*** Update account ***" << endl;

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "DBAccount name ['%s']: ", account.name.c_str());
    account.name = readString(szPrompt, account.name.c_str(), FIELD_STRING_LEN);

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "DBAccount code ['%s']: ", account.code.c_str());
    account.code = readString(szPrompt, account.code.c_str(), FIELD_STRING_LEN);

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Opening balance [%.2f]: ", account.openingBalance);
    snprintf(szBalance, AMOUNT_FIELD_STRING_LEN, "%.2f", account.openingBalance);
    pszBalance = readString(szPrompt, szBalance, AMOUNT_FIELD_STRING_LEN);
    account.openingBalance = strtod(pszBalance, NULL);

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Current balance [%.2f]: ", account.currentBalance);
    snprintf(szBalance, AMOUNT_FIELD_STRING_LEN, "%.2f", account.currentBalance);
    pszBalance = readString(szPrompt, szBalance, AMOUNT_FIELD_STRING_LEN);
    account.currentBalance = strtod(pszBalance, NULL);

    if (account.code.length() == 0) {
        fprintf(stderr, "\nAccount code must have a value.\n");
        return;
    }

    PFM_DB & db = PFM_DB::getInstance();

    db.updateAccount(account);

    free(pszBalance);
}

void delete_account(DBAccount & account) {
    PFM_DB & db = PFM_DB::getInstance();

    db.deleteAccount(account);
}

void add_category(void) {
    char *          categoryDescription;
    char *          categoryCode;

    cout << "*** Add category ***" << endl;

    categoryDescription = readString("DBCategory description: ", NULL, 32);
    categoryCode = readString("DBCategory code (max. 5 chars): ", NULL, 5);

    if (strlen(categoryCode) == 0) {
        fprintf(stderr, "\nCategory code must have a value.\n");
        return;
    }

    DBCategory category;
    category.code = categoryCode;
    category.description = categoryDescription;

    PFM_DB & db = PFM_DB::getInstance();

    db.createCategory(category);

    free(categoryCode);
    free(categoryDescription);
}

void list_categories(void) {
    DBCategoryResult          result;
    int                     numCategories;
    int                     i;

    PFM_DB & db = PFM_DB::getInstance();

    numCategories = db.getCategories(&result);

    cout << "*** Categories (" << numCategories << ") ***" << endl << endl;
    cout << "| Code  | Description               |" << endl;
    cout << "-------------------------------------" << endl;

    for (i = 0;i < numCategories;i++) {
        DBCategory category = result.results[i];

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
    char *          categoryCode;
    DBCategoryResult  result;

    if (pszCategoryCode == NULL || strlen(pszCategoryCode) == 0) {
        cout << "*** Get category ***" << endl;
        categoryCode = readString("DBCategory code (max. 5 chars): ", NULL, 5);
    }
    else {
        categoryCode = strdup(pszCategoryCode);
    }

    PFM_DB & db = PFM_DB::getInstance();

    db.getCategory(categoryCode, &result);

    free(categoryCode);

    return result.results[0];
}

void update_category(DBCategory & category) {
    char            szPrompt[MAX_PROMPT_LENGTH];

    cout << "*** Update category ***" << endl;

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "DBCategory description ['%s']: ", category.description.c_str());
    category.description = readString(szPrompt, category.description.c_str(), FIELD_STRING_LEN);

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "DBCategory code ['%s']: ", category.code.c_str());
    category.code = readString(szPrompt, category.code.c_str(), FIELD_STRING_LEN);

    if (category.code.length() == 0) {
        fprintf(stderr, "\nCategory code must have a value.\n");
        return;
    }

    PFM_DB & db = PFM_DB::getInstance();

    db.updateCategory(category);
}

void delete_category(DBCategory & category) {
    PFM_DB & db = PFM_DB::getInstance();

    db.deleteCategory(category);
}

void add_payee(void) {
    char *          payeeName;
    char *          payeeCode;

    cout << "*** Add payee ***" << endl;

    payeeName = readString("DBPayee name: ", NULL, 32);
    payeeCode = readString("DBPayee code (max. 5 chars): ", NULL, 5);

    if (strlen(payeeCode) == 0) {
        fprintf(stderr, "\nPayee code must have a value.\n");
        return;
    }

    DBPayee payee;
    payee.code = payeeCode;
    payee.name = payeeName;

    PFM_DB & db = PFM_DB::getInstance();

    db.createPayee(payee);

    free(payeeCode);
    free(payeeName);
}

void list_payees(void) {
    DBPayeeResult             result;
    int                     numPayees;
    int                     i;

    PFM_DB & db = PFM_DB::getInstance();

    numPayees = db.getPayees(&result);

    cout << "*** Payees (" << numPayees << ") ***" << endl << endl;
    cout << "| Code  | Name                      |" << endl;
    cout << "-------------------------------------" << endl;

    for (i = 0;i < numPayees;i++) {
        DBPayee payee = result.results[i];

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
    char *          payeeCode;
    DBPayeeResult     result;

    if (pszPayeeCode == NULL || strlen(pszPayeeCode) == 0) {
        cout << "*** Get payee ***" << endl;
        payeeCode = readString("DBPayee code (max. 5 chars): ", NULL, 5);
    }
    else {
        payeeCode = strdup(pszPayeeCode);
    }

    PFM_DB & db = PFM_DB::getInstance();

    db.getPayee(payeeCode, &result);

    free(payeeCode);

    return result.results[0];
}

void update_payee(DBPayee & payee) {
    char            szPrompt[MAX_PROMPT_LENGTH];

    cout << "*** Update payee ***" << endl;

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "DBPayee name ['%s']: ", payee.name.c_str());
    payee.name = readString(szPrompt, payee.name.c_str(), FIELD_STRING_LEN);

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "DBPayee code ['%s']: ", payee.code.c_str());
    payee.code = readString(szPrompt, payee.code.c_str(), FIELD_STRING_LEN);

    if (payee.code.length() == 0) {
        fprintf(stderr, "\nPayee code must have a value.\n");
        return;
    }

    PFM_DB & db = PFM_DB::getInstance();

    db.updatePayee(payee);
}

void delete_payee(DBPayee & payee) {
    PFM_DB & db = PFM_DB::getInstance();

    db.deletePayee(payee);
}

void add_recurring_charge(DBAccount & account) {
    DBRecurringCharge charge;
    const char *    today;
    char *          categoryCode;
    char *          payeeCode;
    char *          date;
    char *          description;
    char *          frequency;
    char *          amount;
    bool            isDateValid = false;
    bool            isFrequencyValid = false;

    today = StrDate::today().c_str();

    PFM_DB & db = PFM_DB::getInstance();

    cout << "*** Add recurring charge ***" << endl;

    using_history();
    clear_history();

    DBCategoryResult catResult;
    db.getCategories(&catResult);

    for (int i = 0;i < catResult.numRows;i++) {
        add_history(catResult.results[i].code.c_str());
    }

    categoryCode = readString("DBCategory code (max. 5 chars)^ ", NULL, 4);

    using_history();
    clear_history();

    DBPayeeResult payResult;
    db.getPayees(&payResult);

    for (int i = 0;i < payResult.numRows;i++) {
        add_history(payResult.results[i].code.c_str());
    }

    payeeCode = readString("DBPayee code (max. 5 chars)^ ", NULL, 5);

    while (!isDateValid) {
        date = readString("Start date (yyyy-mm-dd)[today]: ", today, 10);

        isDateValid = StrDate::validateDate(date);
    }

    description = readString("Charge description: ", description, 32);

    while (!isFrequencyValid) {
        frequency = readString("Frequency (N[wmy]): ", "1m", 3);

        isFrequencyValid = validatePaymentFrequency(frequency);
    }

    amount = readString("Amount: ", NULL, 32);

    if (strlen(categoryCode) == 0) {
        fprintf(stderr, "\nCategory code must have a value.\n");
        return;
    }

    if (strlen(payeeCode) == 0) {
        fprintf(stderr, "\nPayee code must have a value.\n");
        return;
    }

    DBCategoryResult cr;

    db.getCategory(categoryCode, &cr);

    DBPayeeResult pr;

    db.getPayee(payeeCode, &pr);

    charge.accountId = account.id;
    charge.categoryId = cr.results[0].id;
    charge.payeeId = pr.results[0].id;

    charge.date = date;
    charge.description = description;
    charge.frequency = frequency;
    charge.amount = strtod(amount, NULL);

    db.createRecurringCharge(charge);

    free(payeeCode);
    free(categoryCode);
    free(date);
    free(description);
    free(frequency);
    free(amount);
}

void list_recurring_charges(DBAccount & account) {
    DBRecurringChargeResult   result;
    int                     numCharges;
    int                     i;
    char                    seq[4];

    PFM_DB & db = PFM_DB::getInstance();

    numCharges = db.getRecurringChargesForAccount(account.id, &result);

    cout << "*** Recurring charges for account: '" << account.code << "' (" << numCharges << ") ***" << endl << endl;

    cout << "| Seq | Start Date | Nxt Pmnt   | Description               | Cat.  | DBPayee | Frq. | Amount       |" << endl;
    cout << "---------------------------------------------------------------------------------------------------" << endl;

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    for (i = 0;i < numCharges;i++) {
        DBRecurringCharge charge = result.results[i];

        cacheMgr.addRecurringCharge(charge.sequence, charge);

        snprintf(seq, 4, "%03d", charge.sequence);

        cout << 
            "| " << 
            seq <<
            " | " <<
            charge.date <<
            " | " <<
            charge.nextPaymentDate <<
            " | " <<
            left << setw(25) << charge.description << 
            " | " << 
            left << setw(5) << charge.category.code << 
            " | " <<
            left << setw(5) << charge.payee.code <<
            " | " <<
            right << setw(4) << charge.frequency <<
            " | " <<
            right << setw(13) << formatCurrency(charge.amount) <<
            " |" <<
            endl;
    }

    cout << endl;
}

DBRecurringCharge get_recurring_charge(int sequence) {
    DBRecurringCharge         charge;
    char *                  pszSequence;

    if (sequence == 0) {
        cout << "*** Get recurring charge ***" << endl;
        pszSequence = readString("Sequence no.: ", NULL, 3);

        sequence = atoi(pszSequence);

        free(pszSequence);
    }

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    charge = cacheMgr.getRecurringCharge(sequence);

    return charge;
}

void update_recurring_charge(DBRecurringCharge & charge) {
    char            szPrompt[MAX_PROMPT_LENGTH];
    char            amountStr[AMOUNT_FIELD_STRING_LEN];
    char *          categoryCode;
    char *          payeeCode;
    char *          date;
    char *          description;
    char *          frequency;
    char *          amount;
    bool            isDateValid = false;
    bool            isFrequencyValid = false;

    cout << "*** Update recurring charge ***" << endl;

    PFM_DB & db = PFM_DB::getInstance();

    using_history();
    clear_history();

    DBCategoryResult catResult;
    db.getCategories(&catResult);

    for (int i = 0;i < catResult.numRows;i++) {
        add_history(catResult.results[i].code.c_str());
    }

    catResult.clear();
    db.getCategory(charge.categoryId, &catResult);
    charge.category.setCategory(catResult.results[0]);

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "DBCategory code ['%s']^ ", charge.category.code.c_str());
    categoryCode = readString(szPrompt, charge.category.code.c_str(), FIELD_STRING_LEN);

    using_history();
    clear_history();

    DBPayeeResult payResult;
    db.getPayees(&payResult);

    for (int i = 0;i < payResult.numRows;i++) {
        add_history(payResult.results[i].code.c_str());
    }

    payResult.clear();
    db.getPayee(charge.payeeId, &payResult);
    charge.payee.setPayee(payResult.results[0]);

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "DBPayee code ['%s']^ ", charge.payee.code.c_str());
    payeeCode = readString(szPrompt, charge.payee.code.c_str(), FIELD_STRING_LEN);

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Start date ['%s']: ", charge.date.c_str());

    while (!isDateValid) {
        date = readString(szPrompt, charge.date.c_str(), FIELD_STRING_LEN);

        isDateValid = StrDate::validateDate(date);
    }

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Description ['%s']: ", charge.description.c_str());
    description = readString(szPrompt, charge.description.c_str(), FIELD_STRING_LEN);

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Frequency (N[wmy])['%s']: ", charge.frequency.c_str());

    while (!isFrequencyValid) {
        frequency = readString(szPrompt, charge.frequency.c_str(), FIELD_STRING_LEN);

        isFrequencyValid = validatePaymentFrequency(frequency);
    }

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Amount [%.2f]: ", charge.amount);
    snprintf(amountStr, AMOUNT_FIELD_STRING_LEN, "%.2f", charge.amount);
    amount = readString(szPrompt, amountStr, AMOUNT_FIELD_STRING_LEN);

    if (strlen(categoryCode) == 0) {
        fprintf(stderr, "\nCategory code must have a value.\n");
        return;
    }

    if (strlen(payeeCode) == 0) {
        fprintf(stderr, "\nPayee code must have a value.\n");
        return;
    }

    DBCategoryResult cr;

    db.getCategory(categoryCode, &cr);

    DBPayeeResult pr;

    db.getPayee(payeeCode, &pr);

    charge.categoryId = cr.results[0].id;
    charge.payeeId = pr.results[0].id;

    charge.date = date;
    charge.description = description;
    charge.frequency = frequency;
    charge.amount = strtod(amount, NULL);

    db.updateRecurringCharge(charge);
}

void delete_recurring_charge(DBRecurringCharge & charge) {
    PFM_DB & db = PFM_DB::getInstance();

    db.deleteRecurringCharge(charge);
}

void add_transaction(DBAccount & account) {
    DBTransaction     transaction;
    const char *    today;
    char *          categoryCode;
    char *          payeeCode;
    char *          date;
    char *          description;
    char *          credit_debit;
    char *          amount;
    char *          is_reconciled = NULL;
    bool            isDateValid = false;
    bool            isCreditDebitValid = false;
    bool            isReconciledValid = false;

    today = StrDate::today().c_str();

    PFM_DB & db = PFM_DB::getInstance();

    cout << "*** Add transaction ***" << endl;

    using_history();
    clear_history();

    DBCategoryResult catResult;
    db.getCategories(&catResult);

    for (int i = 0;i < catResult.numRows;i++) {
        add_history(catResult.results[i].code.c_str());
    }

    categoryCode = readString("DBCategory code (max. 5 chars)^ ", NULL, 4);

    using_history();
    clear_history();

    DBPayeeResult payResult;
    db.getPayees(&payResult);

    for (int i = 0;i < payResult.numRows;i++) {
        add_history(payResult.results[i].code.c_str());
    }

    payeeCode = readString("DBPayee code (max. 5 chars)^ ", NULL, 5);

    while (!isDateValid) {
        date = readString("DBTransaction date (yyyy-mm-dd)[today]: ", today, 10);

        isDateValid = StrDate::validateDate(date);
    }

    description = readString("Description: ", description, 32);

    while (!isCreditDebitValid) {
        credit_debit = readString("Credit/Debit [DB]: ", "DB", 2);

        isCreditDebitValid = validateCreditDebit(credit_debit);
    }

    amount = readString("Amount: ", NULL, 32);

    while (!isReconciledValid) {
        is_reconciled = readString("Is reconciled [N]: ", "N", 1);

        isReconciledValid = (is_reconciled[0] == 'Y' || is_reconciled[0] == 'N');
    }

    if (strlen(categoryCode) == 0) {
        fprintf(stderr, "\nCategory code must have a value.\n");
        return;
    }

    if (strlen(payeeCode) == 0) {
        fprintf(stderr, "\nPayee code must have a value.\n");
        return;
    }

    DBCategoryResult cr;

    db.getCategory(categoryCode, &cr);

    DBPayeeResult pr;

    db.getPayee(payeeCode, &pr);

    transaction.accountId = account.id;
    transaction.categoryId = cr.results[0].id;
    transaction.payeeId = pr.results[0].id;

    transaction.date = date;
    transaction.description = description;
    transaction.isCredit = decodeCreditDebit(credit_debit);
    transaction.amount = strtod(amount, NULL);
    transaction.isReconciled = strtobool(is_reconciled);

    db.createTransaction(transaction);

    free(payeeCode);
    free(categoryCode);
    free(date);
    free(description);
    free(credit_debit);
    free(amount);
    free(is_reconciled);
}

void list_transactions(DBAccount & account) {
    DBTransactionResult       result;
    int                     numTransactions;
    int                     i;
    char                    seq[4];

    PFM_DB & db = PFM_DB::getInstance();

    numTransactions = db.getTransactionsForAccount(account.id, &result);

    cout << "*** Transactions for account: '" << account.code << "' (" << numTransactions << ") ***" << endl << endl;

    cout << "| Seq | Date       | Description               | Cat.  | DBPayee | CR/DB | Amount       | Rec |" << endl;
    cout << "---------------------------------------------------------------------------------------------" << endl;

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    for (i = 0;i < numTransactions;i++) {
        DBTransaction transaction = result.results[i];

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
    DBTransaction             transaction;
    char *                  pszSequence;

    if (sequence == 0) {
        cout << "*** Get transaction ***" << endl;
        pszSequence = readString("Sequence no.: ", NULL, 3);

        sequence = atoi(pszSequence);

        free(pszSequence);
    }

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    transaction = cacheMgr.getTransaction(sequence);

    return transaction;
}

void update_transaction(DBTransaction & transaction) {
    char            szPrompt[MAX_PROMPT_LENGTH];
    char            amountStr[AMOUNT_FIELD_STRING_LEN];
    char *          categoryCode;
    char *          payeeCode;
    char *          date;
    char *          description;
    char *          credit_debit;
    char *          amount;
    char *          is_reconciled = NULL;
    bool            isDateValid = false;
    bool            isCreditDebitValid = false;
    bool            isReconciledValid = false;

    cout << "*** Update transaction ***" << endl;

    PFM_DB & db = PFM_DB::getInstance();

    using_history();
    clear_history();

    DBCategoryResult catResult;
    db.getCategories(&catResult);

    for (int i = 0;i < catResult.numRows;i++) {
        add_history(catResult.results[i].code.c_str());
    }

    catResult.clear();
    db.getCategory(transaction.categoryId, &catResult);
    transaction.category.setCategory(catResult.results[0]);

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "DBCategory code ['%s']^ ", transaction.category.code.c_str());
    categoryCode = readString(szPrompt, transaction.category.code.c_str(), FIELD_STRING_LEN);

    using_history();
    clear_history();

    DBPayeeResult payResult;
    db.getPayees(&payResult);

    for (int i = 0;i < payResult.numRows;i++) {
        add_history(payResult.results[i].code.c_str());
    }

    payResult.clear();
    db.getPayee(transaction.payeeId, &payResult);
    transaction.payee.setPayee(payResult.results[0]);

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "DBPayee code ['%s']^ ", transaction.payee.code.c_str());
    payeeCode = readString(szPrompt, transaction.payee.code.c_str(), FIELD_STRING_LEN);

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "DBTransaction date ['%s']: ", transaction.date.c_str());

    while (!isDateValid) {
        date = readString(szPrompt, transaction.date.c_str(), FIELD_STRING_LEN);

        isDateValid = StrDate::validateDate(date);
    }

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Description ['%s']: ", transaction.description.c_str());
    description = readString(szPrompt, transaction.description.c_str(), FIELD_STRING_LEN);

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Credit/Debit ['%s']: ", (transaction.isCredit ? "CR" : "DB"));

    while (!isCreditDebitValid) {
        credit_debit = readString(szPrompt, (transaction.isCredit ? "CR" : "DB"), 2);

        isCreditDebitValid = validateCreditDebit(credit_debit);
    }

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Amount [%.2f]: ", transaction.amount);
    snprintf(amountStr, AMOUNT_FIELD_STRING_LEN, "%.2f", transaction.amount);
    amount = readString(szPrompt, amountStr, AMOUNT_FIELD_STRING_LEN);

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Is reconciled ['%s']: ", (transaction.isReconciled ? "Y" : "N"));

    while (!isReconciledValid) {
        is_reconciled = readString(szPrompt, (transaction.isReconciled ? "Y" : "N"), 1);

        isReconciledValid = (is_reconciled[0] == 'Y' || is_reconciled[0] == 'N');
    }

    if (strlen(categoryCode) == 0) {
        fprintf(stderr, "\nCategory code must have a value.\n");
        return;
    }

    if (strlen(payeeCode) == 0) {
        fprintf(stderr, "\nPayee code must have a value.\n");
        return;
    }

    DBCategoryResult cr;

    db.getCategory(categoryCode, &cr);

    DBPayeeResult pr;

    db.getPayee(payeeCode, &pr);

    transaction.categoryId = cr.results[0].id;
    transaction.payeeId = pr.results[0].id;

    transaction.date = date;
    transaction.description = description;
    transaction.isCredit = decodeCreditDebit(credit_debit);
    transaction.amount = strtod(amount, NULL);
    transaction.isReconciled = strtobool(is_reconciled);

    db.updateTransaction(transaction);
}

void delete_transaction(DBTransaction & transaction) {
    PFM_DB & db = PFM_DB::getInstance();

    db.deleteTransaction(transaction);
}

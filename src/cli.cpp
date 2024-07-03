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
#include "account.h"
#include "category.h"
#include "recurring_charge.h"

using namespace std;

void add_account(void) {
    char *          accountName;
    char *          accountCode;
    char *          openingBalance;
    double          balance;
    sqlite3_int64   accountId;

    cout << "*** Add account ***" << endl;

    accountName = readString("Account name: ", NULL, 32);
    accountCode = readString("Account code (max. 5 chars): ", NULL, 4);
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

    Account account;
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
    AccountResult           result;
    int                     numAccounts;
    int                     i;

    PFM_DB & db = PFM_DB::getInstance();

    numAccounts = db.getAccounts(&result);

    cout << "*** Accounts (" << numAccounts << ") ***" << endl << endl;
    cout << "| Code  | Name                      | Balance      |" << endl;
    cout << "----------------------------------------------------" << endl;

    for (i = 0;i < numAccounts;i++) {
        Account account = result.results[i];

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

Account choose_account(const char * szAccountCode) {
    char *          accountCode;
    AccountResult   result;

    if (szAccountCode == NULL || strlen(szAccountCode) == 0) {
        cout << "*** Use account ***" << endl;
        accountCode = readString("Account code (max. 4 chars): ", NULL, 4);
    }
    else {
        accountCode = strdup(szAccountCode);
    }

    PFM_DB & db = PFM_DB::getInstance();

    db.getAccount(accountCode, &result);

    free(accountCode);

    return result.results[0];
}

void update_account(Account & account) {
    char            szPrompt[MAX_PROMPT_LENGTH];
    char            szBalance[AMOUNT_FIELD_STRING_LEN];
    char *          pszBalance;

    cout << "*** Update account ***" << endl;

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Account name ['%s']: ", account.name.c_str());
    account.name = readString(szPrompt, account.name.c_str(), FIELD_STRING_LEN);

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Account code ['%s']: ", account.code.c_str());
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

void delete_account(Account & account) {
    PFM_DB & db = PFM_DB::getInstance();

    db.deleteAccount(account);
}

void add_category(void) {
    char *          categoryDescription;
    char *          categoryCode;

    cout << "*** Add category ***" << endl;

    categoryDescription = readString("Category description: ", NULL, 32);
    categoryCode = readString("Category code (max. 5 chars): ", NULL, 5);

    if (strlen(categoryCode) == 0) {
        fprintf(stderr, "\nCategory code must have a value.\n");
        return;
    }

    Category category;
    category.code = categoryCode;
    category.description = categoryDescription;

    PFM_DB & db = PFM_DB::getInstance();

    db.createCategory(category);

    free(categoryCode);
    free(categoryDescription);
}

void list_categories(void) {
    CategoryResult          result;
    int                     numCategories;
    int                     i;

    PFM_DB & db = PFM_DB::getInstance();

    numCategories = db.getCategories(&result);

    cout << "*** Categories (" << numCategories << ") ***" << endl << endl;
    cout << "| Code  | Description               |" << endl;
    cout << "-------------------------------------" << endl;

    for (i = 0;i < numCategories;i++) {
        Category category = result.results[i];

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

Category get_category(const char * pszCategoryCode) {
    char *          categoryCode;
    CategoryResult  result;

    if (pszCategoryCode == NULL || strlen(pszCategoryCode) == 0) {
        cout << "*** Get category ***" << endl;
        categoryCode = readString("Category code (max. 5 chars): ", NULL, 5);
    }
    else {
        categoryCode = strdup(pszCategoryCode);
    }

    PFM_DB & db = PFM_DB::getInstance();

    db.getCategory(categoryCode, &result);

    free(categoryCode);

    return result.results[0];
}

void update_category(Category & category) {
    char            szPrompt[MAX_PROMPT_LENGTH];

    cout << "*** Update category ***" << endl;

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Category description ['%s']: ", category.description.c_str());
    category.description = readString(szPrompt, category.description.c_str(), FIELD_STRING_LEN);

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Category code ['%s']: ", category.code.c_str());
    category.code = readString(szPrompt, category.code.c_str(), FIELD_STRING_LEN);

    if (category.code.length() == 0) {
        fprintf(stderr, "\nCategory code must have a value.\n");
        return;
    }

    PFM_DB & db = PFM_DB::getInstance();

    db.updateCategory(category);
}

void delete_category(Category & category) {
    PFM_DB & db = PFM_DB::getInstance();

    db.deleteCategory(category);
}

void add_payee(void) {
    char *          payeeName;
    char *          payeeCode;

    cout << "*** Add payee ***" << endl;

    payeeName = readString("Payee name: ", NULL, 32);
    payeeCode = readString("Payee code (max. 5 chars): ", NULL, 5);

    if (strlen(payeeCode) == 0) {
        fprintf(stderr, "\nPayee code must have a value.\n");
        return;
    }

    Payee payee;
    payee.code = payeeCode;
    payee.name = payeeName;

    PFM_DB & db = PFM_DB::getInstance();

    db.createPayee(payee);

    free(payeeCode);
    free(payeeName);
}

void list_payees(void) {
    PayeeResult             result;
    int                     numPayees;
    int                     i;

    PFM_DB & db = PFM_DB::getInstance();

    numPayees = db.getPayees(&result);

    cout << "*** Payees (" << numPayees << ") ***" << endl << endl;
    cout << "| Code  | Name                      |" << endl;
    cout << "-------------------------------------" << endl;

    for (i = 0;i < numPayees;i++) {
        Payee payee = result.results[i];

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

Payee get_payee(const char * pszPayeeCode) {
    char *          payeeCode;
    PayeeResult     result;

    if (pszPayeeCode == NULL || strlen(pszPayeeCode) == 0) {
        cout << "*** Get payee ***" << endl;
        payeeCode = readString("Payee code (max. 5 chars): ", NULL, 5);
    }
    else {
        payeeCode = strdup(pszPayeeCode);
    }

    PFM_DB & db = PFM_DB::getInstance();

    db.getPayee(payeeCode, &result);

    free(payeeCode);

    return result.results[0];
}

void update_payee(Payee & payee) {
    char            szPrompt[MAX_PROMPT_LENGTH];

    cout << "*** Update payee ***" << endl;

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Payee name ['%s']: ", payee.name.c_str());
    payee.name = readString(szPrompt, payee.name.c_str(), FIELD_STRING_LEN);

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Payee code ['%s']: ", payee.code.c_str());
    payee.code = readString(szPrompt, payee.code.c_str(), FIELD_STRING_LEN);

    if (payee.code.length() == 0) {
        fprintf(stderr, "\nPayee code must have a value.\n");
        return;
    }

    PFM_DB & db = PFM_DB::getInstance();

    db.updatePayee(payee);
}

void delete_payee(Payee & payee) {
    PFM_DB & db = PFM_DB::getInstance();

    db.deletePayee(payee);
}

void add_recurring_charge(Account & account) {
    RecurringCharge charge;
    char *          today;
    char *          categoryCode;
    char *          payeeCode;
    char *          date;
    char *          description;
    char *          frequency;
    char *          amount;
    bool            isDateValid = false;
    bool            isFrequencyValid = false;

    today = getToday();

    PFM_DB & db = PFM_DB::getInstance();

    cout << "*** Add recurring charge ***" << endl;

    using_history();
    clear_history();

    CategoryResult catResult;
    db.getCategories(&catResult);

    for (int i = 0;i < catResult.numRows;i++) {
        add_history(catResult.results[i].code.c_str());
    }

    categoryCode = readString("Category code (max. 5 chars)^ ", NULL, 4);

    using_history();
    clear_history();

    PayeeResult payResult;
    db.getPayees(&payResult);

    for (int i = 0;i < payResult.numRows;i++) {
        add_history(payResult.results[i].code.c_str());
    }

    payeeCode = readString("Payee code (max. 5 chars)^ ", NULL, 5);

    while (!isDateValid) {
        date = readString("Start date (yyyy-mm-dd): ", today, 10);

        isDateValid = validateDate(date);
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

    CategoryResult cr;

    db.getCategory(categoryCode, &cr);

    PayeeResult pr;

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

void list_recurring_charges(Account & account) {
    RecurringChargeResult   result;
    int                     numCharges;
    int                     i;
    char                    seq[4];

    PFM_DB & db = PFM_DB::getInstance();

    numCharges = db.getRecurringChargesForAccount(account.id, &result);

    cout << "*** Recurring charges for account: '" << account.code << "' (" << numCharges << ") ***" << endl << endl;

    cout << "| Seq | Date       | Description               | Cat.  | Payee | Frq. | Amnt         |" << endl;
    cout << "--------------------------------------------------------------------------------------" << endl;

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    for (i = 0;i < numCharges;i++) {
        RecurringCharge charge = result.results[i];

        cacheMgr.addRecurringCharge(charge.sequence, charge);

        snprintf(seq, 4, "%03d", charge.sequence);

        cout << 
            "| " << 
            seq <<
            " | " <<
            charge.date <<
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

RecurringCharge get_recurring_charge(int sequence) {
    RecurringCharge         charge;
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

void update_recurring_charge(RecurringCharge & charge) {
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

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Category code ['%s']^ ", charge.category.code.c_str());
    categoryCode = readString(szPrompt, charge.category.code.c_str(), FIELD_STRING_LEN);

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Payee code ['%s']^ ", charge.payee.code.c_str());
    payeeCode = readString(szPrompt, charge.payee.code.c_str(), FIELD_STRING_LEN);

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Start date ['%s']: ", charge.date.c_str());

    while (!isDateValid) {
        date = readString(szPrompt, charge.date.c_str(), FIELD_STRING_LEN);

        isDateValid = validateDate(date);
    }

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Description ['%s']: ", charge.description.c_str());
    description = readString(szPrompt, charge.payee.code.c_str(), FIELD_STRING_LEN);

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

    CategoryResult cr;

    db.getCategory(categoryCode, &cr);

    PayeeResult pr;

    db.getPayee(payeeCode, &pr);

    charge.categoryId = cr.results[0].id;
    charge.payeeId = pr.results[0].id;

    charge.date = date;
    charge.description = description;
    charge.frequency = frequency;
    charge.amount = strtod(amount, NULL);

    db.updateRecurringCharge(charge);
}

void delete_recurring_charge(RecurringCharge & charge) {
    PFM_DB & db = PFM_DB::getInstance();

    db.deleteRecurringCharge(charge);
}

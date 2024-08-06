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

#if defined(__APPLE__) || defined(__unix__)
std::ostream& bold_on(std::ostream& os) {
    return os << "\e[1m";
}

std::ostream& bold_off(std::ostream& os) {
    return os << "\e[0m";
}
#else
std::ostream& bold_on(std::ostream& os) {
    return os << "";
}

std::ostream& bold_off(std::ostream& os) {
    return os << "";
}
#endif

void add_account(void) {
    char *          accountName;
    char *          accountCode;
    char *          openingBalance;
    double          balance;

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

    DBAccount account;
    account.name = accountName;
    account.code = accountCode;
    account.openingBalance = balance;
    account.currentBalance = balance;

    account.save();

    cout << "Created account with ID " << account.id << endl;

    free(openingBalance);
    free(accountCode);
    free(accountName);
}

void list_accounts(void) {
    DBAccountResult         result;
    int                     numAccounts;
    int                     i;

    DBAccount accountInstance;
    result = accountInstance.retrieveAll();

    numAccounts = result.getNumRows();

    cout << "*** Accounts (" << numAccounts << ") ***" << endl << endl;
    cout << "| Code  | Name                      | Balance      |" << endl;
    cout << "----------------------------------------------------" << endl;

    for (i = 0;i < numAccounts;i++) {
        DBAccount account = result.getResultAt(i);

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

    if (szAccountCode == NULL || strlen(szAccountCode) == 0) {
        cout << "*** Use account ***" << endl;
        accountCode = readString("Account code (max. 4 chars): ", NULL, 4);
    }
    else {
        accountCode = strdup(szAccountCode);
    }

    string code = szAccountCode;

    DBAccount account;

    account.retrieveByCode(code);

    free(accountCode);

    //db.createDueRecurringTransactionsForAccount(account.id);

    return account;
}

void update_account(DBAccount & account) {
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

    account.save();

    free(pszBalance);
}

void delete_account(DBAccount & account) {
    account.remove();
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

    DBCategory category;
    category.code = categoryCode;
    category.description = categoryDescription;

    category.save();

    free(categoryCode);
    free(categoryDescription);
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
        cout << "*** Get category ***" << endl;
        categoryCode = readString("Category code (max. 5 chars): ", NULL, 5);
    }
    else {
        categoryCode = strdup(pszCategoryCode);
    }

    DBCategory category;
    category.retrieveByCode(categoryCode);

    return category;
}

void update_category(DBCategory & category) {
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

    category.save();
}

void delete_category(DBCategory & category) {
    category.remove();
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

    DBPayee payee;
    payee.code = payeeCode;
    payee.name = payeeName;

    payee.save();

    free(payeeCode);
    free(payeeName);
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
        cout << "*** Get payee ***" << endl;
        payeeCode = readString("Payee code (max. 5 chars): ", NULL, 5);
    }
    else {
        payeeCode = strdup(pszPayeeCode);
    }

    DBPayee payee;
    payee.retrieveByCode(payeeCode);

    return payee;
}

void update_payee(DBPayee & payee) {
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

    payee.save();
}

void delete_payee(DBPayee & payee) {
    payee.remove();
}

void add_recurring_charge(DBAccount & account) {
    char *          payeeName;
    char *          date;
    char *          description;
    char *          frequency;
    char *          amount;
    bool            isDateValid = false;
    bool            isFrequencyValid = false;

    cout << "*** Add recurring charge ***" << endl;

    using_history();
    clear_history();

    DBCategory category;
    DBCategoryResult catResult = category.retrieveAll();

    for (int i = 0;i < catResult.getNumRows();i++) {
        add_history(catResult.getResultAt(i).code.c_str());
    }

    string categoryCode = readString("Category code (max. 5 chars)^ ", NULL, 4);

    using_history();
    clear_history();

    DBPayee payee;
    DBPayeeResult payResult = payee.retrieveAll();

    for (int i = 0;i < payResult.getNumRows();i++) {
        add_history(payResult.getResultAt(i).code.c_str());
    }

    string payeeCode = readString("Payee code (max. 5 chars)^ ", NULL, 5);

    /*
    ** If the payee does not exist, add it here for convenience...
    */
    try {
        payee.retrieveByCode(payeeCode);
    }
    catch (pfm_error & e) {
        payeeName = readString("Payee name: ", NULL, 32);

        DBPayee payee;
        payee.code = payeeCode;
        payee.name = payeeName;

        payee.save();

        free(payeeName);
    }

    string today = StrDate::today();

    while (!isDateValid) {
        date = readString("Start date (yyyy-mm-dd)[today]: ", today.c_str(), 10);
        isDateValid = StrDate::validateDate(date);
    }

    description = readString("Charge description: ", description, 32);

    while (!isFrequencyValid) {
        frequency = readString("Frequency (N[wmy]): ", "1m", 3);
        isFrequencyValid = validatePaymentFrequency(frequency);
    }

    amount = readString("Amount: ", NULL, 32);

    if (categoryCode.length() == 0) {
        fprintf(stderr, "\nCategory code must have a value.\n");
        return;
    }

    if (payeeCode.length() == 0) {
        fprintf(stderr, "\nPayee code must have a value.\n");
        return;
    }

    category.retrieveByCode(categoryCode);
    payee.retrieveByCode(payeeCode);

    DBRecurringCharge charge;

    charge.accountId = account.id;
    charge.categoryId = category.id;
    charge.payeeId = payee.id;

    charge.date = date;
    charge.description = description;
    charge.frequency = frequency;
    charge.amount = strtod(amount, NULL);
    charge.createdDate = StrDate::today();
    charge.updatedDate = StrDate::today();

    charge.save();

    free(date);
    free(description);
    free(frequency);
    free(amount);
}

void list_recurring_charges(DBAccount & account) {
    DBRecurringCharge chargeInstance;
    DBRecurringChargeResult result = chargeInstance.retrieveByAccountID(account.id);

    clear_history();

    cout << "*** Recurring charges for account: '" << account.code << "' (" << result.getNumRows() << ") ***" << endl << endl;

    cout << "| Seq | Start Date | Nxt Pmnt   | Description               | Cat.  | Payee | Frq. | Amount       |" << endl;
    cout << "---------------------------------------------------------------------------------------------------" << endl;

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    double total = 0.0;
    char seq[4];

    for (int i = 0;i < result.getNumRows();i++) {
        DBRecurringCharge charge = result.getResultAt(i);

        cacheMgr.addRecurringCharge(charge.sequence, charge);

        total += charge.amount;

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

    cout << "---------------------------------------------------------------------------------------------------" << endl;
    cout << "                                                                    Total charges: | " << bold_on << right << setw(13) << formatCurrency(total) << bold_off << " |" << endl;

    cout << endl;
}

DBRecurringCharge get_recurring_charge(int sequence) {
    char * pszSequence;

    if (sequence == 0) {
        cout << "*** Get recurring charge ***" << endl;
        pszSequence = readString("Sequence no.: ", NULL, 3);

        sequence = atoi(pszSequence);

        free(pszSequence);
    }

    CacheMgr & cacheMgr = CacheMgr::getInstance();
    DBRecurringCharge charge = cacheMgr.getRecurringCharge(sequence);

    return charge;
}

void update_recurring_charge(DBRecurringCharge & charge) {
    char            szPrompt[MAX_PROMPT_LENGTH];
    char            amountStr[AMOUNT_FIELD_STRING_LEN];
    char *          payeeName;
    char *          date;
    char *          description;
    char *          frequency;
    char *          amount;
    bool            isDateValid = false;
    bool            isFrequencyValid = false;

    cout << "*** Update recurring charge ***" << endl;

    using_history();
    clear_history();

    DBCategory category;
    DBCategoryResult catResult = category.retrieveAll();

    for (int i = 0;i < catResult.getNumRows();i++) {
        add_history(catResult.getResultAt(i).code.c_str());
    }

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Category code ['%s']^ ", charge.category.code.c_str());
    string categoryCode = readString(szPrompt, charge.category.code.c_str(), FIELD_STRING_LEN);

    using_history();
    clear_history();

    DBPayee payee;
    DBPayeeResult payResult = payee.retrieveAll();

    for (int i = 0;i < payResult.getNumRows();i++) {
        add_history(payResult.getResultAt(i).code.c_str());
    }

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Payee code ['%s']^ ", charge.payee.code.c_str());
    string payeeCode = readString(szPrompt, charge.payee.code.c_str(), FIELD_STRING_LEN);

    /*
    ** If the payee does not exist, add it here for convenience...
    */
    try {
        payee.retrieveByCode(payeeCode);
    }
    catch (pfm_error & e) {
        payeeName = readString("Payee name: ", NULL, 32);

        DBPayee payee;
        payee.code = payeeCode;
        payee.name = payeeName;

        payee.save();

        free(payeeName);
    }

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

    if (categoryCode.length() == 0) {
        fprintf(stderr, "\nCategory code must have a value.\n");
        return;
    }

    if (payeeCode.length() == 0) {
        fprintf(stderr, "\nPayee code must have a value.\n");
        return;
    }

    category.retrieveByCode(categoryCode);
    payee.retrieveByCode(payeeCode);

    charge.categoryId = category.id;
    charge.payeeId = payee.id;

    charge.date = date;
    charge.description = description;
    charge.frequency = frequency;
    charge.amount = strtod(amount, NULL);

    charge.save();
}

void delete_recurring_charge(DBRecurringCharge & charge) {
    charge.remove();
}

void add_transaction(DBAccount & account) {
    char *          payeeName;
    char *          date;
    char *          description;
    char *          credit_debit;
    char *          amount;
    bool            isDateValid = false;
    bool            isCreditDebitValid = false;

    cout << "*** Add transaction ***" << endl;

    using_history();
    clear_history();

    DBCategory category;
    DBCategoryResult catResult = category.retrieveAll();

    for (int i = 0;i < catResult.getNumRows();i++) {
        add_history(catResult.getResultAt(i).code.c_str());
    }

    string categoryCode = readString("Category code (max. 5 chars)^ ", NULL, 4);

    using_history();
    clear_history();

    DBPayee payee;
    DBPayeeResult payResult = payee.retrieveAll();

    for (int i = 0;i < payResult.getNumRows();i++) {
        add_history(payResult.getResultAt(i).code.c_str());
    }

    string payeeCode = readString("Payee code (max. 5 chars)^ ", NULL, 5);

    /*
    ** If the payee does not exist, add it here for convenience...
    */
    try {
        payee.retrieveByCode(payeeCode);
    }
    catch (pfm_error & e) {
        payeeName = readString("Payee name: ", NULL, 32);

        DBPayee payee;
        payee.code = payeeCode;
        payee.name = payeeName;

        payee.save();

        free(payeeName);
    }

    string today = StrDate::today();

    while (!isDateValid) {
        date = readString("Transaction date (yyyy-mm-dd)[today]: ", today.c_str(), 10);
        isDateValid = StrDate::validateDate(date);
    }

    description = readString("Description: ", NULL, 32);

    while (!isCreditDebitValid) {
        credit_debit = readString("Credit/Debit [DB]: ", "DB", 2);
        isCreditDebitValid = validateCreditDebit(credit_debit);
    }

    amount = readString("Amount: ", NULL, 32);

    bool isReconciledValid = false;
    string is_reconciled;

    while (!isReconciledValid) {
        is_reconciled = readString("Is reconciled [N]: ", "N", 1);
        isReconciledValid = DBEntity::isYesNoBooleanValid(is_reconciled);
    }

    if (categoryCode.length() == 0) {
        fprintf(stderr, "\nCategory code must have a value.\n");
        return;
    }

    if (payeeCode.length() == 0) {
        fprintf(stderr, "\nPayee code must have a value.\n");
        return;
    }

    category.retrieveByCode(categoryCode);
    payee.retrieveByCode(payeeCode);

    DBTransaction transaction;

    transaction.accountId = account.id;
    transaction.categoryId = category.id;
    transaction.payeeId = payee.id;

    transaction.date = date;
    transaction.description = description;
    transaction.isCredit = decodeCreditDebit(credit_debit);
    transaction.amount = strtod(amount, NULL);
    transaction.isReconciled = strtobool(is_reconciled.c_str());
    transaction.createdDate = StrDate::today();
    transaction.updatedDate = StrDate::today();

    transaction.save();

    free(date);
    free(description);
    free(credit_debit);
    free(amount);
}

void list_transactions(DBAccount & account) {
    DBTransaction transactionInstance;
    DBTransactionResult result = transactionInstance.retrieveByAccountID(account.id);

    cout << "*** Transactions for account: '" << account.code << "' (" << result.getNumRows() << ") ***" << endl << endl;

    cout << "| Seq | Date       | Description               | Cat.  | Payee | CR/DB | Amount       | Rec |" << endl;
    cout << "---------------------------------------------------------------------------------------------" << endl;

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    char seq[4];

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
    char * pszSequence;

    if (sequence == 0) {
        cout << "*** Get transaction ***" << endl;
        pszSequence = readString("Sequence no.: ", NULL, 3);

        sequence = atoi(pszSequence);

        free(pszSequence);
    }

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    DBTransaction transaction = cacheMgr.getTransaction(sequence);

    return transaction;
}

void update_transaction(DBTransaction & transaction) {
    char            szPrompt[MAX_PROMPT_LENGTH];
    char            amountStr[AMOUNT_FIELD_STRING_LEN];
    char *          payeeName;
    char *          date;
    char *          description;
    char *          credit_debit;
    char *          amount;
    char *          is_reconciled = NULL;
    bool            isDateValid = false;
    bool            isCreditDebitValid = false;
    bool            isReconciledValid = false;

    cout << "*** Update transaction ***" << endl;

    using_history();
    clear_history();

    DBCategory category;
    DBCategoryResult catResult = category.retrieveAll();

    for (int i = 0;i < catResult.getNumRows();i++) {
        add_history(catResult.getResultAt(i).code.c_str());
    }

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Category code ['%s']^ ", transaction.category.code.c_str());
    string categoryCode = readString(szPrompt, transaction.category.code.c_str(), FIELD_STRING_LEN);

    using_history();
    clear_history();

    DBPayee payee;
    DBPayeeResult payResult = payee.retrieveAll();

    for (int i = 0;i < payResult.getNumRows();i++) {
        add_history(payResult.getResultAt(i).code.c_str());
    }

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Payee code ['%s']^ ", transaction.payee.code.c_str());
    string payeeCode = readString(szPrompt, transaction.payee.code.c_str(), FIELD_STRING_LEN);

    /*
    ** If the payee does not exist, add it here for convenience...
    */
    try {
        payee.retrieveByCode(payeeCode);
    }
    catch (pfm_error & e) {
        payeeName = readString("Payee name: ", NULL, 32);

        DBPayee payee;
        payee.code = payeeCode;
        payee.name = payeeName;

        payee.save();

        free(payeeName);
    }

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Transaction date ['%s']: ", transaction.date.c_str());

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

    if (categoryCode.length() == 0) {
        fprintf(stderr, "\nCategory code must have a value.\n");
        return;
    }

    if (payeeCode.length() == 0) {
        fprintf(stderr, "\nPayee code must have a value.\n");
        return;
    }

    category.retrieveByCode(categoryCode);
    payee.retrieveByCode(payeeCode);

    transaction.categoryId = category.id;
    transaction.payeeId = payee.id;

    transaction.date = date;
    transaction.description = description;
    transaction.isCredit = decodeCreditDebit(credit_debit);
    transaction.amount = strtod(amount, NULL);
    transaction.isReconciled = strtobool(is_reconciled);

    transaction.save();
}

void delete_transaction(DBTransaction & transaction) {
    transaction.remove();
}

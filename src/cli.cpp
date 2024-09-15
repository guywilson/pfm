#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#endif

#include <sqlite3.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "keymgr.h"
#include "db.h"
#include "cache.h"
#include "cli.h"
#include "strdate.h"
#include "views.h"
#include "db_account.h"
#include "db_category.h"
#include "db_recurring_charge.h"
#include "db_transaction.h"
#include "db_budget.h"
#include "db_user.h"
#include "db_carried_over.h"
#include "db_v_budget_track.h"

using namespace std;

int __getch(void) {
	int		ch;

#ifndef _WIN32
	struct termios current;
	struct termios original;

	tcgetattr(fileno(stdin), &original); /* grab old terminal i/o settings */
	current = original; /* make new settings same as old settings */
	current.c_lflag &= ~ICANON; /* disable buffered i/o */
	current.c_lflag &= ~ECHO; /* set echo mode */
	tcsetattr(fileno(stdin), TCSANOW, &current); /* use these new terminal i/o settings now */
#endif

#ifdef _WIN32
    ch = _getch();
#else
    ch = getchar();
#endif

#ifndef _WIN32
	tcsetattr(0, TCSANOW, &original);
#endif

    return ch;
}

string getPassword() {
    printf("Enter password: ");

    string password;
	int	ch = 0;

    while (ch != '\n') {
        ch = __getch();

        if (ch != '\n' && ch != '\r') {
            putchar('*');
            fflush(stdout);

            password += (char)ch;
        }
    }

    putchar('\n');
    fflush(stdout);

    return password;
}

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

DBUser addUser() {
    AddUserView view;
    view.show();

    Key & key = Key::getInstance();
    string password = getPassword();

    DBUser user = view.getUser();

    if (password.length() > 0) {
        key.hasPassword = true;
        key.generate(password);
        user.password = key.getBase64Key();
        user.hasPassword = true;
    }
    else {
        key.hasPassword = false;
        user.hasPassword = false;
    }

    user.save();

    return user;
}

DBUser login() {
    DBResult<DBUser> userResult;
    int numUsers = userResult.retrieveAll();
    userResult.clear();

    Key & key = Key::getInstance();

    DBUser user;

    if (numUsers == 0) {
        user = addUser();
    }
    else {
        LoginView view;
        view.show();

        string userName = view.getUserName();

        user.retrieveByUser(userName);

        if (user.hasPassword) {
            key.hasPassword = true;
            key.generate(getPassword());
            string base64Key = key.getBase64Key();

            user.checkPassword(base64Key);
        }
    }

    return user;
}

void addAccount(DBUser & user) {
    AddAccountView view;
    view.show();

    DBAccount account = view.getAccount();
    account.userId = user.id;
    account.save();

    cout << "Created account with ID " << account.id << endl;
}

void listAccounts() {
    DBResult<DBAccount> result;
    result.retrieveAll();

    AccountListView view;

    view.addResults(result);
    view.show();
}

DBAccount chooseAccount(const char * szAccountCode) {
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
    
    showAccountBalances(account);
    
    return account;
}

void updateAccount(DBAccount & account) {
    UpdateAccountView view;
    view.setAccount(account);
    view.show();

    DBAccount updatedAccount = view.getAccount();

    updatedAccount.save();
}

void showAccountBalances(DBAccount & account) {
    Money balanceAfterBills = account.calculateBalanceAfterBills();

    cout << "Current balance:     " << right << setw(13) << account.currentBalance.getFormattedStringValue() << endl;
    cout << "Balance after bills: " << right << setw(13) << balanceAfterBills.getFormattedStringValue() << endl;
}

void addCategory() {
    AddCategoryView view;
    view.show();

    DBCategory category = view.getCategory();

    category.save();
}

void listCategories() {
    DBResult<DBCategory> result;
    result.retrieveAll();

    CategoryListView view;
    view.addResults(result);
    view.show();
}

DBCategory getCategory(const char * pszCategoryCode) {
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

void updateCategory(DBCategory & category) {
    UpdateCategoryView view;
    view.setCategory(category);
    view.show();

    DBCategory updatedCategory = view.getCategory();
    updatedCategory.save();
}

void addPayee() {
    AddPayeeView view;
    view.show();

    DBPayee payee = view.getPayee();

    payee.save();
}

void listPayees() {
    DBResult<DBPayee> result;
    result.retrieveAll();

    PayeeListView view;
    view.addResults(result);
    view.show();
}

DBPayee getPayee(const char * pszPayeeCode) {
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

void updatePayee(DBPayee & payee) {
    UpdatePayeeView view;
    view.setPayee(payee);
    view.show();

    DBPayee updatedPayee = view.getPayee();
    updatedPayee.save();
}

void addRecurringCharge(DBAccount & account) {
    AddRecurringChargeView view;
    view.show();

    DBRecurringCharge charge = view.getRecurringCharge();
    charge.accountId = account.id;

    charge.save();
}

void listRecurringCharges(DBAccount & account) {
    DBRecurringChargeView chargeInstance;
    DBResult<DBRecurringChargeView> result = chargeInstance.retrieveByAccountID(account.id);

    RecurringChargeListView view;
    view.addResults(result, account.code);
    view.show();

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    for (int i = 0;i < result.getNumRows();i++) {
        DBRecurringCharge charge = result.getResultAt(i);
        cacheMgr.addRecurringCharge(charge.sequence, charge);
    }
}

DBRecurringCharge getRecurringCharge(int sequence) {
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

void updateRecurringCharge(DBRecurringCharge & charge) {
    UpdateRecurringChargeView view;
    
    view.setRecurringCharge(charge);
    view.show();

    DBRecurringCharge updatedCharge = view.getRecurringCharge();

    updatedCharge.save();
}

void addTransaction(DBAccount & account) {
    AddTransactionView view;
    view.show();

    DBTransaction transaction = view.getTransaction();
    transaction.accountId = account.id;
    transaction.save();
}

void listTransactions(DBAccount & account) {
    DBTransactionView transactionInstance;
    DBResult<DBTransactionView> result = transactionInstance.retrieveByAccountID(account.id, sort_ascending, 0);

    TransactionListView view;
    view.addResults(result, account.code);
    view.show();

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    for (int i = 0;i < result.getNumRows();i++) {
        DBTransaction transaction = result.getResultAt(i);
        cacheMgr.addTransaction(transaction.sequence, transaction);
    }
}

void findTransactions(DBAccount & account) {
    cout << "How do you want to search for transactions:" << endl;
    cout << "1) By category" << endl;
    cout << "2) By payee" << endl;
    cout << "3) By description" << endl;
    cout << "4) Between dates" << endl;

    CLITextField optionField = CLITextField("Enter search option: ");
    optionField.show();

    int option = (int)optionField.getIntegerValue();

    cout << "Option = " << option << endl;

    CLIFindView * findView;

    switch (option) {
        case 1:
            findView = new FindTransactionByCategoryView();
            break;

        case 2:
            findView = new FindTransactionByPayeeView();
            break;

        case 3:
            findView = new FindTransactionByDescriptionView();
            break;

        case 4:
            findView = new FindTransactionByDateView();
            break;

        default:
            findView = new FindTransactionView();
            break;
    }

    findView->show();
    
    string criteria = findView->getCriteria();

    delete findView;
    
    DBTransactionView tr;
    DBResult<DBTransactionView> result = tr.findTransactionsForAccountID(account.id, criteria);

    TransactionListView view;
    view.addResults(result, account.code);
    view.show();

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    for (int i = 0;i < result.getNumRows();i++) {
        DBTransaction transaction = result.getResultAt(i);
        cacheMgr.addTransaction(transaction.sequence, transaction);
    }
}

DBTransaction getTransaction(int sequence) {
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

void updateTransaction(DBTransaction & transaction) {
    UpdateTransactionView view;
    view.setTransaction(transaction);
    view.show();

    DBTransaction updatedTransaction = view.getTransaction();
    updatedTransaction.save();
}

void deleteTransaction(DBTransaction & transaction) {
    transaction.remove();
}

void addBudget() {
    AddBudgetView view;
    view.show();

    DBBudget budget = view.getBudget();
    budget.save();
}

void listBudgets() {
    DBResult<DBBudget> result;
    result.retrieveAll();

    BudgetListView view;
    view.addResults(result);
    view.show();

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    for (int i = 0;i < result.getNumRows();i++) {
        DBBudget budget = result.getResultAt(i);
        cacheMgr.addBudget(budget.sequence, budget);
    }
}

DBBudget getBudget(int sequence) {
    int selectedSequence;

    if (sequence == 0) {
        ChooseBudgetView view;
        view.show();

        selectedSequence = view.getSequence();
    }
    else {
        selectedSequence = sequence;
    }

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    DBBudget budget = cacheMgr.getBudget(selectedSequence);

    return budget;
}

void updateBudget(DBBudget & budget) {
    UpdateBudgetView view;
    view.setBudget(budget);
    view.show();

    DBBudget updatedBudget = view.getBudget();
    updatedBudget.save();
}

void deleteBudget(DBBudget & budget) {
    budget.remove();
}

void listCarriedOverLogs(DBAccount & account) {
    DBResult<DBCarriedOver> result;
    result.retrieveAll();

    CarriedOverListView view;
    view.addResults(result);
    view.show();
}

void listBudgetTracks() {
    DBResult<DBBudgetTrackView> result;
    result.retrieveAll();

    BudgetTrackView view;
    view.addResults(result);
    view.show();
}

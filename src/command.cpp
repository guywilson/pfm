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

#include "pfm_error.h"
#include "keymgr.h"
#include "db.h"
#include "cache.h"
#include "strdate.h"
#include "views.h"
#include "version.h"
#include "db_account.h"
#include "db_category.h"
#include "db_recurring_charge.h"
#include "db_transaction.h"
#include "db_budget.h"
#include "db_user.h"
#include "db_carried_over.h"
#include "db_v_budget_track.h"
#include "command.h"

using namespace std;

static int __getch(void) {
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

static string getPassword() {
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

void Command::help() {

}

void Command::version() {
    cout << "PFM version '" << getVersion() << "' - built [" << getBuildDate() << "]" << endl << endl;
}

DBUser Command::addUser() {
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

DBUser Command::login() {
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

void Command::addAccount(DBUser & user) {
    AddAccountView view;
    view.show();

    DBAccount account = view.getAccount();
    account.userId = user.id;
    account.save();

    cout << "Created account with ID " << account.id << endl;
}

void Command::listAccounts() {
    DBResult<DBAccount> result;
    result.retrieveAll();

    AccountListView view;

    view.addResults(result);
    view.show();
}

void Command::chooseAccount(string & accountCode) {
    log.logEntry("Command::chooseAccount()");

    log.logDebug("Choose account with code '%s'", accountCode.c_str());

    if (accountCode.length() == 0) {
        ChooseAccountView view;
        view.show();

        accountCode = view.getCode();
    }

    DBAccount account;
    account.retrieveByCode(accountCode);
    account.onUseAccountTrigger();
    
    showAccountBalances(account);
    
    selectedAccount = account;

    log.logExit("Command::chooseAccount()");
}

void Command::updateAccount() {
    checkAccountSelected();

    UpdateAccountView view;
    view.setAccount(selectedAccount);
    view.show();

    DBAccount updatedAccount = view.getAccount();

    updatedAccount.save();
}

void Command::showAccountBalances(DBAccount & account) {
    Money balanceAfterBills = account.calculateBalanceAfterBills();

    cout << "Current balance:     " << right << setw(13) << account.currentBalance.getFormattedStringValue() << endl;
    cout << "Balance after bills: " << right << setw(13) << balanceAfterBills.getFormattedStringValue() << endl;
}

void Command::addCategory() {
    AddCategoryView view;
    view.show();

    DBCategory category = view.getCategory();

    category.save();
}

void Command::listCategories() {
    DBResult<DBCategory> result;
    result.retrieveAll();

    CategoryListView view;
    view.addResults(result);
    view.show();
}

DBCategory Command::getCategory(string & categoryCode) {
    if (categoryCode.length() == 0) {
        ChooseCategoryView view;
        view.show();

        categoryCode = view.getCode();
    }

    DBCategory category;
    category.retrieveByCode(categoryCode);

    return category;
}

void Command::updateCategory(DBCategory & category) {
    UpdateCategoryView view;
    view.setCategory(category);
    view.show();

    DBCategory updatedCategory = view.getCategory();
    updatedCategory.save();
}

void Command::deleteCategory(DBCategory & category) {
    category.remove();
    category.clear();
}

void Command::addPayee() {
    AddPayeeView view;
    view.show();

    DBPayee payee = view.getPayee();

    payee.save();
}

void Command::listPayees() {
    DBResult<DBPayee> result;
    result.retrieveAll();

    PayeeListView view;
    view.addResults(result);
    view.show();
}

DBPayee Command::getPayee(string & payeeCode) {
    if (payeeCode.length() == 0) {
        ChoosePayeeView view;
        view.show();

        payeeCode = view.getCode();
    }

    DBPayee payee;
    payee.retrieveByCode(payeeCode);

    return payee;
}

void Command::updatePayee(DBPayee & payee) {
    UpdatePayeeView view;
    view.setPayee(payee);
    view.show();

    DBPayee updatedPayee = view.getPayee();
    updatedPayee.save();
}

void Command::deletePayee(DBPayee & payee) {
    payee.remove();
    payee.clear();
}

void Command::addRecurringCharge() {
    checkAccountSelected();

    AddRecurringChargeView view;
    view.show();

    DBRecurringCharge charge = view.getRecurringCharge();
    charge.accountId = selectedAccount.id;

    charge.save();
}

void Command::listRecurringCharges() {
    checkAccountSelected();

    DBRecurringChargeView chargeInstance;
    DBResult<DBRecurringChargeView> result = chargeInstance.retrieveByAccountID(selectedAccount.id);

    RecurringChargeListView view;
    view.addResults(result, selectedAccount.code);
    view.show();

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    for (int i = 0;i < result.getNumRows();i++) {
        DBRecurringCharge charge = result.getResultAt(i);
        cacheMgr.addRecurringCharge(charge.sequence, charge);
    }
}

DBRecurringCharge Command::getRecurringCharge(int sequence) {
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

void Command::updateRecurringCharge(DBRecurringCharge & charge) {
    UpdateRecurringChargeView view;
    
    view.setRecurringCharge(charge);
    view.show();

    DBRecurringCharge updatedCharge = view.getRecurringCharge();

    updatedCharge.save();
}

void Command::deleteRecurringCharge(DBRecurringCharge & charge) {
    charge.remove();
    charge.clear();
}

void Command::addTransaction() {
    checkAccountSelected();

    AddTransactionView view;
    view.show();

    DBTransaction transaction = view.getTransaction();
    transaction.accountId = selectedAccount.id;
    transaction.save();
}

void Command::listTransactions() {
    checkAccountSelected();

    DBTransactionView transactionInstance;
    DBResult<DBTransactionView> result = transactionInstance.retrieveByAccountID(selectedAccount.id, sort_ascending, 0);

    TransactionListView view;
    view.addResults(result, selectedAccount.code);
    view.show();

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    for (int i = 0;i < result.getNumRows();i++) {
        DBTransaction transaction = result.getResultAt(i);
        cacheMgr.addTransaction(transaction.sequence, transaction);
    }
}

void Command::findTransactions() {
    checkAccountSelected();

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
    DBResult<DBTransactionView> result = tr.findTransactionsForAccountID(selectedAccount.id, criteria);

    TransactionListView view;
    view.addResults(result, selectedAccount.code);
    view.show();

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    for (int i = 0;i < result.getNumRows();i++) {
        DBTransaction transaction = result.getResultAt(i);
        cacheMgr.addTransaction(transaction.sequence, transaction);
    }
}

DBTransaction Command::getTransaction(int sequence) {
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

void Command::updateTransaction(DBTransaction & transaction) {
    UpdateTransactionView view;
    view.setTransaction(transaction);
    view.show();

    DBTransaction updatedTransaction = view.getTransaction();
    updatedTransaction.save();
}

void Command::deleteTransaction(DBTransaction & transaction) {
    transaction.remove();
}

void Command::addBudget() {
    AddBudgetView view;
    view.show();

    DBBudget budget = view.getBudget();
    budget.save();
}

void Command::listBudgets() {
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

DBBudget Command::getBudget(int sequence) {
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

void Command::updateBudget(DBBudget & budget) {
    UpdateBudgetView view;
    view.setBudget(budget);
    view.show();

    DBBudget updatedBudget = view.getBudget();
    updatedBudget.save();
}

void Command::deleteBudget(DBBudget & budget) {
    budget.remove();
    budget.clear();
}

void Command::listCarriedOverLogs() {
    DBResult<DBCarriedOver> result;
    result.retrieveAll();

    CarriedOverListView view;
    view.addResults(result);
    view.show();
}

void Command::listBudgetTracks() {
    DBResult<DBBudgetTrackView> result;
    result.retrieveAll();

    BudgetTrackView view;
    view.addResults(result);
    view.show();
}

Command::pfm_cmd_t Command::getCommandCode(string & command) {
    this->command = command;

    if (isCommand("exit") || isCommand("quit") || isCommand("q")) {
        return pfm_cmd_exit;
    }
    else if (isCommand("version")) {
        return pfm_cmd_version;
    }
    else if (isCommand("help")) {
        return pfm_cmd_help;
    }
    else if (isCommand("add account") || isCommand("aa")) {
        return pfm_cmd_account_add;
    }
    else if (isCommand("list accounts") || isCommand("la")) {
        return pfm_cmd_account_list;
    }
    else if (isCommand("use")) {
        return pfm_cmd_account_use;
    }
    else if (isCommand("update account") || isCommand("ua")) {
        return pfm_cmd_account_update;
    }
    else if (isCommand("delete account") || isCommand("da")) {
        return pfm_cmd_account_delete;
    }
    else if (isCommand("show balance") || isCommand("sb")) {
        return pfm_cmd_account_balance;
    }
    else if (isCommand("add category") || isCommand("ac")) {
        return pfm_cmd_category_add;
    }
    else if (isCommand("list categories") || isCommand("lc")) {
        return pfm_cmd_category_list;
    }
    else if (isCommand("update category") || isCommand("uc")) {
        return pfm_cmd_category_update;
    }
    else if (isCommand("delete category") || isCommand("dc")) {
        return pfm_cmd_category_delete;
    }
    else if (isCommand("add payee") || isCommand("ap")) {
        return pfm_cmd_category_add;
    }
    else if (isCommand("list payees") || isCommand("lp")) {
        return pfm_cmd_category_list;
    }
    else if (isCommand("update payee") || isCommand("up")) {
        return pfm_cmd_category_update;
    }
    else if (isCommand("delete payee") || isCommand("dp")) {
        return pfm_cmd_category_delete;
    }
    else if (isCommand("add recurring charge") || isCommand("arc")) {
        return pfm_cmd_charge_add;
    }
    else if (isCommand("list recurring charges") || isCommand("lrc")) {
        return pfm_cmd_charge_list;
    }
    else if (isCommand("update recurring charge") || isCommand("urc")) {
        return pfm_cmd_charge_update;
    }
    else if (isCommand("delete recurring charge") || isCommand("drc")) {
        return pfm_cmd_charge_delete;
    }
    else if (isCommand("add transaction") || isCommand("at")) {
        return pfm_cmd_transaction_add;
    }
    else if (isCommand("list transactions") || isCommand("lt")) {
        return pfm_cmd_transaction_list;
    }
    else if (isCommand("find transactions") || isCommand("find")) {
        return pfm_cmd_transaction_find;
    }
    else if (isCommand("update transaction") || isCommand("ut")) {
        return pfm_cmd_transaction_update;
    }
    else if (isCommand("delete transaction") || isCommand("dt")) {
        return pfm_cmd_transaction_delete;
    }
    else if (isCommand("add budget") || isCommand("ab")) {
        return pfm_cmd_budget_add;
    }
    else if (isCommand("list budgets") || isCommand("lb")) {
        return pfm_cmd_budget_list;
    }
    else if (isCommand("update budget") || isCommand("ub")) {
        return pfm_cmd_budget_update;
    }
    else if (isCommand("delete budget") || isCommand("db")) {
        return pfm_cmd_budget_delete;
    }
    else if (isCommand("list carried over logs") || isCommand("lco")) {
        return pfm_cmd_debug_carried_over;
    }
    else if (isCommand("list budget track records") || isCommand("lbt")) {
        return pfm_cmd_debug_budget_track;
    }
    else {
        throw pfm_validation_error(
                    pfm_error::buildMsg(
                        "Sorry, I do not understand command '%s', type help to see the list of commands", 
                        command.c_str()));
    }
}

bool Command::process(string & command) {
    Command::pfm_cmd_t cmd = getCommandCode(command);

    if (cmd == pfm_cmd_exit) {
        return false;
    }
    else if (cmd == pfm_cmd_help) {
        Command::help();
    }
    else if (cmd == pfm_cmd_version) {
        Command::version();
    }
    else if (cmd == pfm_cmd_account_add) {
        addAccount(loggedInUser);
    }
    else if (cmd == pfm_cmd_account_use) {
        string accountCode = getCommandParameter();
        chooseAccount(accountCode);
    }
    else if (cmd == pfm_cmd_account_list) {
        listAccounts();
    }
    else if (cmd == pfm_cmd_account_update) {
        updateAccount();
    }
    else if (cmd == pfm_cmd_account_delete) {
        checkAccountSelected();

        selectedAccount.remove();
        selectedAccount.clear();
    }
    else if (cmd == pfm_cmd_category_add) {
        addCategory();
    }
    else if (cmd == pfm_cmd_category_list) {
        listCategories();
    }
    else if (cmd == pfm_cmd_category_update) {
        string categoryCode = getCommandParameter();
        DBCategory category = getCategory(categoryCode);

        updateCategory(category);
    }
    else if (cmd == pfm_cmd_category_delete) {
        string categoryCode = getCommandParameter();
        DBCategory category = getCategory(categoryCode);

        deleteCategory(category);
    }
    else if (cmd == pfm_cmd_payee_add) {
        addPayee();
    }
    else if (cmd == pfm_cmd_payee_list) {
        listPayees();
    }
    else if (cmd == pfm_cmd_payee_update) {
        string payeeCode = getCommandParameter();
        DBPayee payee = getPayee(payeeCode);

        updatePayee(payee);
    }
    else if (cmd == pfm_cmd_payee_delete) {
        string payeeCode = getCommandParameter();
        DBPayee payee = getPayee(payeeCode);

        deletePayee(payee);
    }
    else if (cmd == pfm_cmd_charge_add) {
        addRecurringCharge();
    }
    else if (cmd == pfm_cmd_charge_list) {
        listRecurringCharges();
    }
    else if (cmd == pfm_cmd_charge_update) {
        string sequence = getCommandParameter();

        DBRecurringCharge charge = getRecurringCharge(atoi(sequence.c_str()));
        updateRecurringCharge(charge);
    }
    else if (cmd == pfm_cmd_charge_delete) {
        string sequence = getCommandParameter();

        DBRecurringCharge charge = getRecurringCharge(atoi(sequence.c_str()));
        deleteRecurringCharge(charge);
    }
    else if (cmd == pfm_cmd_transaction_add) {
        addTransaction();
    }
    else if (cmd == pfm_cmd_transaction_list) {
        listTransactions();
    }
    else if (cmd == pfm_cmd_transaction_find) {
        findTransactions();
    }
    else if (cmd == pfm_cmd_transaction_update) {
        string sequence = getCommandParameter();

        DBTransaction transaction = getTransaction(atoi(sequence.c_str()));
        updateTransaction(transaction);
    }
    else if (cmd == pfm_cmd_transaction_delete) {
        string sequence = getCommandParameter();

        DBTransaction transaction = getTransaction(atoi(sequence.c_str()));
        deleteTransaction(transaction);
    }
    else if (cmd == pfm_cmd_budget_add) {
        addBudget();
    }
    else if (cmd == pfm_cmd_budget_list) {
        listBudgets();
    }
    else if (cmd == pfm_cmd_budget_update) {
        string sequence = getCommandParameter();

        DBBudget budget = getBudget(atoi(sequence.c_str()));
        updateBudget(budget);
    }
    else if (cmd == pfm_cmd_budget_delete) {
        string sequence = getCommandParameter();

        DBBudget budget = getBudget(atoi(sequence.c_str()));
        deleteBudget(budget);
    }
    else if (cmd == pfm_cmd_debug_carried_over) {
        listCarriedOverLogs();
    }
    else if (cmd == pfm_cmd_debug_budget_track) {
        listBudgetTracks();
    }

    return true;
}

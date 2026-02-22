#include <string>
#include <string.h>
#include <vector>

#include "command.h"
#include "pfm_error.h"
#include "db.h"
#include "strdate.h"
#include "cfgmgr.h"
#include "cache.h"
#include "jfile.h"

#include "db_base.h"
#include "db_account.h"
#include "db_primary_account.h"
#include "db_v_transaction.h"
#include "account_views.h"
#include "transaction_views.h"

using namespace std;

void Command::addAccount() {
    AddAccountView view;
    view.show();

    DBAccount account = view.getAccount();
    account.save();

    DBResult<DBAccount> accounts;
    int numAccounts = accounts.retrieveAll();

    /*
    ** If this is the first account, then remove any existing records 
    ** and create the primary account record...
    */
    if (numAccounts == 1) {
        DBPrimaryAccount primaryAccount;
        primaryAccount.removeAll();

        primaryAccount.code = account.code;

        primaryAccount.save();
    }
}

void Command::listAccounts() {
    DBResult<DBAccount> result;
    result.retrieveAll();

    AccountListView view;

    view.addResults(result);
    view.show();
}

void Command::chooseAccount() {
    log.entry("Command::chooseAccount()");

    string accountCode = selectedAccount.code;

    if (hasParameters()) {
        accountCode = getParameter(SIMPLE_PARAM_NAME);
    }

    if (accountCode.length() == 0) {
        ChooseAccountView view;
        view.show();

        accountCode = view.getCode();
    }

    log.debug("Choose account with code '%s'", accountCode.c_str());

    DBAccount account;
    account.retrieveByCode(accountCode);
    
    selectedAccount = account;

    AccountDetailsView view;
    view.setAccount(account);
    view.show();

    DBTransactionView transaction;
    DBResult<DBTransactionView> result = transaction.retrieveByAccountID(account.id, DBCriteria::descending, 16);

    if (Terminal::isOverWidthThreshold()) {
        TransactionListView trView;
        trView.addResults(result, account.code);
        trView.show();
    }
    else {
        TransactionDetailsListView trView;
        trView.addResults(result);
        trView.show();
    }

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    cacheMgr.clearTransactions();

    for (size_t i = 0;i < result.size();i++) {
        DBTransactionView transaction = result[i];
        cacheMgr.addTransaction(transaction.sequence, transaction);
    }

    log.exit("Command::chooseAccount()");
}

void Command::showAccount() {
    log.entry("Command::showAccount()");

    string accountCode = selectedAccount.code;

    log.debug("Show account with code '%s'", accountCode.c_str());

    DBAccount account;
    account.retrieveByCode(accountCode);
    
    AccountDetailsView view;
    view.setAccount(account);
    view.show();

    log.exit("Command::showAccount()");
}

void Command::setPrimaryAccount() {
    log.entry("Command::setPrimaryAccount()");

    string accountCode = getParameter(SIMPLE_PARAM_NAME);
    
    log.debug("Set primary account to '%s'", accountCode.c_str());

    DBPrimaryAccount::setPrimaryAccount(accountCode);

    DBAccount account;
    account.retrieveByCode(accountCode);
    
    selectedAccount = account;

    log.exit("Command::setPrimaryAccount()");
}

void Command::updateAccount() {
    checkAccountSelected();

    UpdateAccountView view;
    view.setAccount(selectedAccount);
    view.show();

    DBAccount updatedAccount = view.getAccount();

    updatedAccount.save();

    selectedAccount = updatedAccount;
}

void Command::deleteAccount() {
    checkAccountSelected();
    selectedAccount.remove();
    selectedAccount.clear();
}

void Command::importAccounts() {
    string jsonFileName = getParameter(SIMPLE_PARAM_NAME);

    JFileReader jfile = JFileReader(jsonFileName);
    jfile.validate("DBAccount");

    vector<JRecord> records = jfile.read("accounts");

    for (JRecord & record : records) {
        DBAccount account;

        account.set(record);
        account.save();
    }
}

void Command::exportAccounts() {
    string jsonFileName = getParameter(SIMPLE_PARAM_NAME);

    DBResult<DBAccount> results;
    results.retrieveAll();

    vector<JRecord> records;

    for (size_t i = 0;i < results.size();i++) {
        DBAccount account = results.at(i);

        JRecord r = account.getRecord();
        records.push_back(r);
    }
    
    JFileWriter jFile(jsonFileName, "DBAccount");
    jFile.write(records, "accounts");
}

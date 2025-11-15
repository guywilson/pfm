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

#include "db_account.h"
#include "db_primary_account.h"
#include "account_views.h"

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

    cout << "Created account with ID " << account.id.getValue() << endl;
}

void Command::listAccounts() {
    DBResult<DBAccount> result;
    result.retrieveAll();

    AccountListView view;

    view.addResults(result);
    view.show();
}

void Command::chooseAccount(string & accountCode) {
    log.entry("Command::chooseAccount()");

    log.debug("Choose account with code '%s'", accountCode.c_str());

    if (accountCode.length() == 0) {
        ChooseAccountView view;
        view.show();

        accountCode = view.getCode();
    }

    DBAccount account;
    account.retrieveByCode(accountCode);
    
    selectedAccount = account;

    log.exit("Command::chooseAccount()");
}

void Command::setPrimaryAccount(string & accountCode) {
    log.entry("Command::setPrimaryAccount()");

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

void Command::importAccounts(const string & jsonFileName) {
    JFileReader jfile = JFileReader(jsonFileName, "DBAccount");

    vector<JRecord> records = jfile.read("accounts");

    for (JRecord & record : records) {
        DBAccount account;

        account.set(record);
        account.save();
    }
}

void Command::exportAccounts(const string & jsonFileName) {
    DBResult<DBAccount> results;
    results.retrieveAll();

    vector<JRecord> records;

    for (int i = 0;i < results.size();i++) {
        DBAccount account = results.at(i);

        JRecord r = account.getRecord();
        records.push_back(r);
    }
    
    JFileWriter jFile = JFileWriter(jsonFileName, "DBAccount");
    jFile.write(records, "accounts");
}

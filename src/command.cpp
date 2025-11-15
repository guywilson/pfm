#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <vector>
#include <unordered_map>

#include <sqlcipher/sqlite3.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "pfm_error.h"
#include "db.h"
#include "cfgmgr.h"
#include "cache.h"
#include "strdate.h"
#include "views.h"
#include "version.h"
#include "db_account.h"
#include "db_primary_account.h"
#include "db_category.h"
#include "db_recurring_charge.h"
#include "db_transaction.h"
#include "db_carried_over.h"
#include "db_v_carried_over.h"
#include "db_transfer_transaction_record.h"
#include "jfile.h"
#include "transaction_criteria.h"
#include "command.h"

using namespace std;

static DBAccount * createSampleAccount() {
    DBAccount * account = new DBAccount();

    account->code = "BANK";
    account->name = "Sample bank account";
    account->openingBalance = 1234.56;
    account->openingDate = "2025-04-01";

    return account;
}

static DBPayee * createSamplePayee() {
    DBPayee * payee = new DBPayee();

    payee->code = "JOES";
    payee->name = "Joe's coffee shop";

    return payee;
}

static DBCategory * createSampleCategory() {
    DBCategory * category = new DBCategory();

    category->code = "COFFE";
    category->description = "Takeaway coffee";

    return category;
}

static DBRecurringCharge * createSampleCharge() {
    DBRecurringCharge * charge = new DBRecurringCharge();

    DBAccount * account = createSampleAccount();
    DBCategory * category = createSampleCategory();
    DBPayee * payee = createSamplePayee();

    charge->account = *account;
    charge->amount = 12.63;
    charge->category = *category;
    charge->date = "2025-07-05";
    charge->description = "Sample charge";
    charge->endDate = "";
    charge->frequency = Frequency::parse("1m");
    charge->payee = *payee;

    return charge;
}

static DBTransaction * createSampleTransaction() {
    DBTransaction * transaction = new DBTransaction();

    DBAccount * account = createSampleAccount();
    DBCategory * category = createSampleCategory();
    DBPayee * payee = createSamplePayee();

    transaction->account = *account;
    transaction->amount = 15.78;
    transaction->category = *category;
    transaction->date = "2025-05-12";
    transaction->description = "Sample transaction";
    transaction->payee = *payee;
    transaction->reference = "";

    return transaction;
}

void Command::help() {
    cout << "For help, please see the manual" << endl;
    cout << "\tman pfm" << endl << endl;
}

void Command::version() {
    cout << "PFM version '" << getVersion() << "' - built [" << getBuildDate() << "]" << endl << endl;
}

void Command::listCarriedOverLogs() {
    DBResult<DBCarriedOverView> result;
    result.retrieveAll();

    CarriedOverListView view;
    view.addResults(result);
    view.show();
}

void Command::changePassword() {
    PFM_DB & db = PFM_DB::getInstance();

    db.changePassword();
}

void Command::getDBKey() {
    PFM_DB & db = PFM_DB::getInstance();

    string accessKey = db.getKey("Access password: ");

    if (accessKey.compare(cfg.getValue("access.key")) == 0) {
        string dbKey = db.getKey("Database password: ");
        cout << "Key: " << dbKey << endl;
    }
    else {
        cout << "Invalid access password supplied" << endl << endl;
    }
}

void Command::saveDBKey() {
    PFM_DB & db = PFM_DB::getInstance();

    string accessKey = db.getKey("Access password: ");

    if (accessKey.compare(cfg.getValue("access.key")) == 0) {
        string dbKey = db.getKey("Database password: ");
        db.saveKeyFile(dbKey);
    }
    else {
        cout << "Invalid access password supplied" << endl << endl;
    }
}

int Command::getLogLevelParameter(string & level) {
    int levelID = 0;

    if (level.compare("entry") == 0) {
        levelID = LOG_LEVEL_ENTRY;
    }
    else if (level.compare("exit") == 0) {
        levelID = LOG_LEVEL_EXIT;
    }
    else if (level.compare("debug") == 0) {
        levelID = LOG_LEVEL_DEBUG;
    }
    else if (level.compare("status") == 0) {
        levelID = LOG_LEVEL_STATUS;
    }
    else if (level.compare("info") == 0) {
        levelID = LOG_LEVEL_INFO;
    }
    else if (level.compare("error") == 0) {
        levelID = LOG_LEVEL_ERROR;
    }
    else if (level.compare("fatal") == 0) {
        levelID = LOG_LEVEL_FATAL;
    }
    else if (level.compare("all") == 0) {
        levelID = LOG_LEVEL_ALL;
    }
    else {
        throw pfm_validation_error(
                    pfm_error::buildMsg(
                        "Invalid logging level identifier '%s'", 
                        level.c_str()), 
                    __FILE__, 
                    __LINE__);
    }

    return levelID;
}

void Command::setLoggingLevel(string & level) {
    log.addLogLevel(getLogLevelParameter(level));
}

void Command::clearLoggingLevel(string & level) {
    log.clearLogLevel(getLogLevelParameter(level));
}

void Command::saveJsonTemplate() {
    cout << "For which entity do you want a JSON template:" << endl;
    cout << "1) Account" << endl;
    cout << "2) Payee" << endl;
    cout << "3) Category" << endl;
    cout << "4) Recurring Charge" << endl;
    cout << "5) Transaction" << endl;

    CLITextField optionField = CLITextField("Enter option: ");
    optionField.show();

    int option = (int)optionField.getIntegerValue();

    cout << "Option = " << option << endl;

    DBEntity * entity;
    string name;
    string className;

    switch (option) {
        case 1:
            entity = createSampleAccount();
            name = "accounts";
            break;

        case 2:
            entity = createSamplePayee();
            name = "payees";
            break;

        case 3:
            entity = createSampleCategory();
            name = "categories";
            break;

        case 4:
            entity = createSampleCharge();
            name = "charges";
            break;

        case 5:
            entity = createSampleTransaction();
            name = "transactions";
            break;

        default:
            throw pfm_validation_error(
                        pfm_error::buildMsg(
                            "saveJsonTemplate() : Invalid entity type %d", 
                            option), 
                        __FILE__, 
                        __LINE__);
    }

    string filename = name + "_template.json";
    JFileWriter writer(filename, entity->getClassName());

    vector<JRecord> records;

    records.push_back(entity->getRecord());
    records.push_back(entity->getRecord());

    writer.write(records, name);

    delete entity;
}

void Command::parse(const string & command) {
    const char * parameterDelimiters = ";:|";

    this->command.clear();
    this->parameters.clear();

    char * cmdString = strdup(command.c_str());;

    char * part = strtok(cmdString, " ");

    int i = 0;
    while (part != NULL) {
        if (i == 0) {
            this->command = part;
        }
        else {
            this->parameters.push_back(part);
        }

        part = strtok(NULL, parameterDelimiters);
        i++;
    }
}

bool Command::process(const string & command) {
    clear_history();

    for (string & cmd : commandHistory) {
        add_history(cmd.c_str());
    }

    parse(command);

    add_history(command.c_str());
    commandHistory.push_back(command);

    bool isContinue = true;

    if (isCommand("exit") || isCommand("quit") || isCommand("q")) {
        isContinue = false;
    }
    else if (isCommand("version")) {
        Command::version();
    }
    else if (isCommand("help")) {
        Command::help();
    }
    else if (isCommand("save-json-template") || isCommand("sjt")) {
        saveJsonTemplate();
    }
    else if (isCommand("add-account") || isCommand("aa")) {
        addAccount();
    }
    else if (isCommand("list-accounts") || isCommand("la")) {
        listAccounts();
    }
    else if (isCommand("use")) {
        string accountCode = getParameter(0);
        chooseAccount(accountCode);
    }
    else if (isCommand("set-primary-account") || isCommand("spa")) {
        string accountCode = getParameter(0);
        setPrimaryAccount(accountCode);
    }
    else if (isCommand("update-account") || isCommand("ua")) {
        updateAccount();
    }
    else if (isCommand("delete-account") || isCommand("da")) {
        checkAccountSelected();

        selectedAccount.remove();
        selectedAccount.clear();
    }
    else if (isCommand("import-accounts") || isCommand("ia")) {
        string filename = getParameter(0);
        importAccounts(filename);
    }
    else if (isCommand("export-accounts") || isCommand("xa")) {
        string filename = getParameter(0);
        exportAccounts(filename);
    }
    else if (isCommand("add-config-item") || isCommand("acfg")) {
        addConfig();
    }
    else if (isCommand("list-config-items") || isCommand("lcfg")) {
        listConfigItems();
    }
    else if (isCommand("update-config-item") || isCommand("ucfg")) {
        string configKey = getParameter(0);
        DBConfig config = getConfig(configKey);

        updateConfig(config);
    }
    else if (isCommand("delete-config-item") || isCommand("dcfg")) {
        string configKey = getParameter(0);
        DBConfig config = getConfig(configKey);

        deleteConfig(config);
    }
    else if (isCommand("add-category") || isCommand("ac")) {
        addCategory();
    }
    else if (isCommand("list-categories") || isCommand("lc")) {
        listCategories();
    }
    else if (isCommand("update-category") || isCommand("uc")) {
        string categoryCode = getParameter(0);
        DBCategory category = getCategory(categoryCode);

        updateCategory(category);
    }
    else if (isCommand("delete-category") || isCommand("dc")) {
        string categoryCode = getParameter(0);
        DBCategory category = getCategory(categoryCode);

        deleteCategory(category);
    }
    else if (isCommand("import-categories") || isCommand("ic")) {
        string filename = getParameter(0);
        importCategories(filename);
    }
    else if (isCommand("export-categories") || isCommand("xc")) {
        string filename = getParameter(0);
        exportCategories(filename);
    }
    else if (isCommand("clear-categories")) {
        clearCategories();
    }
    else if (isCommand("add-payee") || isCommand("ap")) {
        addPayee();
    }
    else if (isCommand("list-payees") || isCommand("lp")) {
        listPayees();
    }
    else if (isCommand("update-payee") || isCommand("up")) {
        string payeeCode = getParameter(0);
        DBPayee payee = getPayee(payeeCode);

        updatePayee(payee);
    }
    else if (isCommand("delete-payee") || isCommand("dp")) {
        string payeeCode = getParameter(0);
        DBPayee payee = getPayee(payeeCode);

        deletePayee(payee);
    }
    else if (isCommand("import-payees") || isCommand("ip")) {
        string filename = getParameter(0);
        importPayees(filename);
    }
    else if (isCommand("export-payees") || isCommand("xp")) {
        string filename = getParameter(0);
        exportPayees(filename);
    }
    else if (isCommand("add-recurring-charge") || isCommand("arc")) {
        addRecurringCharge();
    }
    else if (isCommand("list-recurring-charges") || isCommand("lrc")) {
        listRecurringCharges();
    }
    else if (isCommand("update-recurring-charge") || isCommand("urc")) {
        string sequence = getParameter(0);

        DBRecurringCharge charge = getRecurringCharge(atoi(sequence.c_str()));
        updateRecurringCharge(charge);
    }
    else if (isCommand("delete-recurring-charge") || isCommand("drc")) {
        string sequence = getParameter(0);

        DBRecurringCharge charge = getRecurringCharge(atoi(sequence.c_str()));
        deleteRecurringCharge(charge);
    }
    else if (isCommand("import-recurring-charges") || isCommand("irc")) {
        string filename = getParameter(0);
        importRecurringCharges(filename);
    }
    else if (isCommand("export-recurring-charges") || isCommand("xrc")) {
        string filename = getParameter(0);
        exportRecurringCharges(filename);
    }
    else if (isCommand("migrate-recurring-charge") || isCommand("mrc")) {
        string sequence = getParameter(0);

        DBRecurringCharge charge = getRecurringCharge(atoi(sequence.c_str()));
        migrateCharge(charge);
    }
    else if (isCommand("add-transaction") || isCommand("at") || isCommand("add")) {
        if (hasParameters()) {
            AddTransactionCriteriaBuilder builder(this->parameters);

            addTransaction(builder);
        }
        else {
            addTransaction();
        }
    }
    else if (isCommand("transfer-transaction") || isCommand("tr") || isCommand("transfer")) {
        addTransferTransaction();
    }
    else if (isCommand("list-transactions") || isCommand("lt") || isCommand("list")) {
        bool includeRecurringTransactions = false;
        uint32_t rowLimit = 25;
        DBCriteria::sql_order sortDirection = DBCriteria::descending;

        if (hasParameters()) {
            for (string & parameter : parameters) {
                if (isdigit(parameter[0])) {
                    rowLimit = strtoul(parameter.c_str(), NULL, 10);
                }
                else {
                    if (parameter.compare("all") == 0) {
                        includeRecurringTransactions = true;
                    }
                    else if (parameter.compare("nr") == 0) {
                        includeRecurringTransactions = false;
                    }
                    else if (parameter.compare("asc") == 0) {
                        sortDirection = DBCriteria::ascending;
                    }
                    else if (parameter.compare("desc") == 0) {
                        sortDirection = DBCriteria::descending;
                    }
                }
            }
        }

        listTransactions(rowLimit, sortDirection, includeRecurringTransactions);
    }
    else if (isCommand("find-transactions") || isCommand("find")) {
        if (hasParameters()) {
            FindTransactionCriteriaBuilder builder(this->parameters);

            if (builder.hasRawSQL()) {
                findTransactions(builder.getRawSQL());
            }
            else {
                DBCriteria criteria = builder.getCriteria();
                findTransactions(criteria);
            }
        }
        else {
            findTransactions();
        }
    }
    else if (isCommand("category-report") || isCommand("cr")) {
        transactionsByCategory();
    }
    else if (isCommand("payee-report") || isCommand("pr")) {
        transactionsByPayee();
    }
    else if (isCommand("update-transaction") || isCommand("ut")) {
        string sequence = getParameter(0);

        DBTransaction transaction = getTransaction(atoi(sequence.c_str()));
        updateTransaction(transaction);
    }
    else if (isCommand("delete-transaction") || isCommand("dt")) {
        string sequence = getParameter(0);

        DBTransaction transaction = getTransaction(atoi(sequence.c_str()));
        deleteTransaction(transaction);
    }
    else if (isCommand("reconcile-transaction") || isCommand("reconcile") || isCommand("rt")) {
        string sequence = getParameter(0);

        DBTransaction transaction = getTransaction(atoi(sequence.c_str()));
        reconcileTransaction(transaction);
    }
    else if (isCommand("import-transactions") || isCommand("it")) {
        string filename = getParameter(0);
        importTransactions(filename);
    }
    else if (isCommand("export-transactions") || isCommand("xt")) {
        string filename = getParameter(0);
        exportTransactions(filename);
    }
    else if (isCommand("export-transactions-csv") || isCommand("xtc")) {
        string filename = getParameter(0);
        exportTransactionsAsCSV(filename);
    }
    else if (isCommand("add-report") || isCommand("arp")) {
        addReport();
    }
    else if (isCommand("copy-report") || isCommand("crp")) {
        string sequence = getParameter(0);

        DBTransactionReport report = getReport(atoi(sequence.c_str()));
        copyReport(report);
    }
    else if (isCommand("list-reports") || isCommand("show-reports") || isCommand("lrp")) {
        listReports();
    }
    else if (isCommand("update-report") || isCommand("urp")) {
        string sequence = getParameter(0);

        DBTransactionReport report = getReport(atoi(sequence.c_str()));
        updateReport(report);
    }
    else if (isCommand("delete-report") || isCommand("drp")) {
        string sequence = getParameter(0);

        DBTransactionReport report = getReport(atoi(sequence.c_str()));
        deleteReport(report);
    }
    else if (isCommand("run-report") || isCommand("run")) {
        string sequence = getParameter(0);

        DBTransactionReport report = getReport(atoi(sequence.c_str()));
        runReport(report);
    }
    else if (isCommand("save-report") || isCommand("save")) {
        string description;

        if (hasParameters()) {
            description = getParameter(0);
        }

        saveReport(description);
    }
    else if (isCommand("show-report") || isCommand("srp")) {
        string sequence = getParameter(0);

        DBTransactionReport report = getReport(atoi(sequence.c_str()));
        showReport(report);
    }
    else if (isCommand("list-carried-over-logs") || isCommand("lco")) {
        listCarriedOverLogs();
    }
    else if (isCommand("change-password")) {
        changePassword();
    }
    else if (isCommand("clear-recurring-transactions")) {
        clearRecurringTransactions();
    }
    else if (isCommand("set-logging-level")) {
        string logLevel = getParameter(0);
        setLoggingLevel(logLevel);
    }
    else if (isCommand("clear-logging-level")) {
        string logLevel = getParameter(0);
        clearLoggingLevel(logLevel);
    }
    else if (isCommand("get-db-key")) {
        getDBKey();
    }
    else if (isCommand("save-db-key")) {
        saveDBKey();
    }
    else {
        throw pfm_validation_error(
                    pfm_error::buildMsg(
                        "Sorry, I do not understand command '%s', please see the manual for supported commands.", 
                        command.c_str()));
    }

    if (isContinue) {
        for (string & command : commandHistory) {
            add_history(command.c_str());
        }
    }

    return isContinue;
}

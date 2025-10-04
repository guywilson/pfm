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
    charge->frequency = "1m";
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

void Command::showAccountBalances(DBAccount & account) {
    Money currentBalance = account.calculateCurrentBalance();
    Money balanceAfterBills = account.calculateBalanceAfterBills();

    cout << "Current balance:     " << right << setw(13) << currentBalance.localeFormattedStringValue() << endl;
    cout << "Balance after bills: " << right << setw(13) << balanceAfterBills.localeFormattedStringValue() << endl;
}

void Command::importAccounts(string & jsonFileName) {
    JFileReader jfile = JFileReader(jsonFileName, "DBAccount");

    vector<JRecord> records = jfile.read("accounts");

    for (JRecord & record : records) {
        DBAccount account;

        account.set(record);
        account.save();
    }
}

void Command::exportAccounts(string & jsonFileName) {
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

void Command::addConfig() {
    PFM_DB & db = PFM_DB::getInstance();

    string accessKey = db.getKey("Access password: ");

    if (accessKey.compare(cfg.getValue("access.key")) == 0) {
        AddConfigView view;
        view.show();

        DBConfig config = view.getConfig();

        config.save();
    }
    else {
        cout << "Invalid access password supplied" << endl << endl;
    }
}

void Command::listConfigItems() {
    DBConfig config;
    DBResult<DBConfig> result = config.retrieveAllVisible();

    ConfigListView view;
    view.addResults(result);
    view.show();
}

DBConfig Command::getConfig(string & key) {
    if (key.length() == 0) {
        ChooseConfigView view;
        view.show();

        key = view.getKey();
    }

    DBConfig config;
    config.retrieveByKey(key);

    return config;
}

void Command::updateConfig(DBConfig & config) {
    if (config.isReadOnly) {
        throw pfm_validation_error(
            pfm_error::buildMsg(
                "Selected config item '%s' is read-only and cannot be modified", 
                config.key.c_str()));
    }

    UpdateConfigView view;
    view.setConfig(config);
    view.show();

    DBConfig updatedConfig = view.getConfig();
    updatedConfig.save();
}

void Command::deleteConfig(DBConfig & config) {
    PFM_DB & db = PFM_DB::getInstance();

    string accessKey = db.getKey("Access password: ");

    if (accessKey.compare(cfg.getValue("access.key")) == 0) {
        if (config.isReadOnly) {
            throw pfm_validation_error(
                pfm_error::buildMsg(
                    "Selected config item '%s' is read-only and cannot be modified", 
                    config.key.c_str()));
        }
        
        config.remove();
        config.clear();
    }
    else {
        cout << "Invalid access password supplied" << endl << endl;
    }
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

void Command::importCategories(string & jsonFileName) {
    JFileReader jfile = JFileReader(jsonFileName, "DBCategory");

    vector<JRecord> records = jfile.read("categories");

    for (JRecord & record : records) {
        DBCategory category;

        category.set(record);
        category.save();
    }
}

void Command::exportCategories(string & jsonFileName) {
    DBResult<DBCategory> results;
    results.retrieveAll();

    vector<JRecord> records;

    for (int i = 0;i < results.size();i++) {
        DBCategory category = results.at(i);

        JRecord r = category.getRecord();
        records.push_back(r);
    }
    
    JFileWriter jFile = JFileWriter(jsonFileName, "DBCategory");
    jFile.write(records, "categories");
}

void Command::clearCategories() {
    DBCategory category;
    category.removeAll();
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

void Command::importPayees(string & jsonFileName) {
    JFileReader jfile = JFileReader(jsonFileName, "DBPayee");

    vector<JRecord> records = jfile.read("payees");

    for (JRecord & record : records) {
        DBPayee payee;

        payee.set(record);
        payee.save();
    }
}

void Command::exportPayees(string & jsonFileName) {
    DBResult<DBPayee> results;
    results.retrieveAll();

    vector<JRecord> records;

    for (int i = 0;i < results.size();i++) {
        DBPayee payee = results.at(i);

        JRecord r = payee.getRecord();
        records.push_back(r);
    }
    
    JFileWriter jFile = JFileWriter(jsonFileName, "DBPayee");
    jFile.write(records, "payees");
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

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    cacheMgr.clearRecurringCharges();

    for (int i = 0;i < result.size();i++) {
        DBRecurringCharge charge = result.at(i);
        cacheMgr.addRecurringCharge(charge.sequence, charge);
    }

    RecurringChargeListView view;
    view.addResults(result, selectedAccount.code);
    view.show();
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
    PFM_DB & db = PFM_DB::getInstance();

    try {
        db.begin();

        charge.remove();
        charge.clear();

        db.commit();
    }
    catch (pfm_error & e) {
        db.rollback();
    }
}

void Command::importRecurringCharges(string & jsonFileName) {
    JFileReader jfile = JFileReader(jsonFileName, "DBRecurringCharge");

    vector<JRecord> records = jfile.read("charges");

    for (JRecord & record : records) {
        DBRecurringCharge charge;

        charge.set(record);
        charge.save();
    }
}

void Command::exportRecurringCharges(string & jsonFileName) {
    DBResult<DBRecurringCharge> results;
    results.retrieveAll();

    vector<JRecord> records;

    for (int i = 0;i < results.size();i++) {
        DBRecurringCharge charge = results.at(i);

        JRecord r = charge.getRecord();
        records.push_back(r);
    }
    
    JFileWriter jFile = JFileWriter(jsonFileName, "DBRecurringCharge");
    jFile.write(records, "charges");
}

void Command::clearRecurringTransactions() {
    checkAccountSelected();

    DBTransaction tr;
    tr.deleteAllRecurringTransactionsForAccount(selectedAccount.id);
}

void Command::addTransaction() {
    checkAccountSelected();

    AddTransactionView view;
    view.show();

    DBTransaction transaction = view.getTransaction();
    transaction.accountId = selectedAccount.id;
    transaction.save();
}

void Command::addTransaction(AddTransactionCriteriaBuilder & builder) {
    checkAccountSelected();

    DBTransaction transaction;

    try {
        DBCategory category;
        category.retrieveByCode(builder.categoryCode);
        transaction.categoryId = category.id;
    }
    catch (pfm_error & e) {
        transaction.categoryId = 0;
    }

    try {
        DBPayee payee;
        payee.retrieveByCode(builder.payeeCode);
        transaction.payeeId = payee.id;
    }
    catch (pfm_error & e) {
        transaction.payeeId = 0;
    }

    transaction.accountId = selectedAccount.id;
    transaction.date = builder.date;
    transaction.description = builder.description;
    transaction.reference = builder.reference;
    transaction.amount = builder.amount;
    transaction.isCredit = builder.isCredit;

    transaction.save();
}

void Command::addTransferTransaction() {
    checkAccountSelected();

    TransferToAccountView view;
    view.setSourceAccountCode(selectedAccount.code);
    view.show();

    DBTransaction sourceTransaction = view.getSourceTransaction();
    sourceTransaction.accountId = selectedAccount.id;

    DBTransaction targetTransaction = view.getTargetTransaction();

    PFM_DB & db = PFM_DB::getInstance();

    try {
        db.begin();

        sourceTransaction.save();
        targetTransaction.save();

        db.commit();
    }
    catch (pfm_error & e) {
        db.rollback();
    }
}

void Command::listTransactions(uint32_t rowLimit, db_sort_t sortDirection, bool includeRecurring) {
    checkAccountSelected();

    DBTransactionView transactionInstance;
    DBResult<DBTransactionView> result;

    if (includeRecurring) {
        result = transactionInstance.retrieveByAccountID(selectedAccount.id, sortDirection, rowLimit);
    }
    else {
        result = transactionInstance.retrieveNonRecurringByAccountID(selectedAccount.id, sortDirection, rowLimit);
    }

    TransactionListView view;
    view.addResults(result, selectedAccount.code);
    view.show();

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    cacheMgr.clearTransactions();

    for (int i = 0;i < result.size();i++) {
        DBTransactionView transaction = result.at(i);
        cacheMgr.addTransaction(transaction.sequence, transaction);
    }
}

void Command::findTransactions() {
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
    
    findTransactions(criteria);
}

void Command::findTransactions(const string & criteria) {
    DBTransactionView tr;
    DBResult<DBTransactionView> result = tr.findTransactionsForCriteria(criteria);

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    cacheMgr.clearTransactions();
    
    for (int i = 0;i < result.size();i++) {
        DBTransactionView transaction = result.at(i);
        cacheMgr.addTransaction(transaction.sequence, transaction);
    }

    cacheMgr.addFindCriteria(criteria);

    TransactionListView view;
    view.addTotal();
    view.addResults(result);
    view.show();
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

void Command::reconcileTransaction(DBTransaction & transaction) {
    transaction.retrieve();

    transaction.isReconciled = true;

    transaction.save();
}

void Command::importTransactions(string & jsonFileName) {
    JFileReader jfile = JFileReader(jsonFileName, "DBTransaction");

    vector<JRecord> records = jfile.read("transactions");

    for (JRecord & record : records) {
        DBTransaction transaction;

        transaction.set(record);
        transaction.save();
    }
}

void Command::exportTransactions(string & jsonFileName) {
    DBResult<DBTransaction> results;
    results.retrieveAll();

    vector<JRecord> records;

    for (int i = 0;i < results.size();i++) {
        DBTransaction transaction = results.at(i);

        JRecord r = transaction.getRecord();
        records.push_back(r);
    }
    
    JFileWriter jFile = JFileWriter(jsonFileName, "DBTransaction");
    jFile.write(records, "transactions");
}

void Command::exportTransactionsAsCSV(string & csvFileName) {
    DBResult<DBTransaction> results;
    results.retrieveAll();

    ofstream out(csvFileName);

    out << DBTransaction::getCSVHeader();

    for (int i = 0;i < results.size();i++) {
        DBTransaction transaction = results.at(i);

        string record = transaction.getCSVRecord();
        out << record;
    }
    
    out.close();
}

void Command::addReport() {
    AddReportView view;
    view.show();

    DBTransactionReport report = view.getReport();
    report.save();
}

void Command::copyReport(DBTransactionReport & report) {
    report.id = 0;
    report.save();
}

void Command::listReports() {
    DBResult<DBTransactionReport> result;
    result.retrieveAll();

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    cacheMgr.clearReports();

    for (int i = 0;i < result.size();i++) {
        DBTransactionReport report = result[i];
        cacheMgr.addReport(report.sequence, report);
    }

    ReportListView view;
    view.addResults(result);
    view.show();
}

DBTransactionReport Command::getReport(int sequence) {
    CacheMgr & cacheMgr = CacheMgr::getInstance();

    DBTransactionReport report = cacheMgr.getReport(sequence);

    return report;
}

void Command::updateReport(DBTransactionReport & report) {
    UpdateReportView view;
    view.setReport(report);
    view.show();

    DBTransactionReport updatedReport = view.getReport();
    updatedReport.save();
}

void Command::deleteReport(DBTransactionReport & report) {
    report.remove();
    report.clear();
}

void Command::runReport(DBTransactionReport & report) {
    findTransactions(report.sqlWhereClause);
}

void Command::saveReport(const string & description) {
    CacheMgr & cacheMgr = CacheMgr::getInstance();

    DBTransactionReport report;

    if (description.length() == 0) {
        SaveReportView view;
        view.show();

        report = view.getReport();
        report.sqlWhereClause = cacheMgr.getFindCriteria();
    }
    else {
        report.description = description;
        report.sqlWhereClause = cacheMgr.getFindCriteria();
    }

    report.save();

    cacheMgr.clearFindCriteria();
}

void Command::showReport(DBTransactionReport & report) {
    ShowReportView view;
    view.setReport(report);
    view.show();
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

void Command::parse(const string & cmd) {
    this->command.clear();
    this->parameters.clear();

    Tokenizer t(cmd);
    string part = t.next(" ");

    int i = 0;
    while (part.length() > 0) {
        if (i == 0) {
            this->command = part;
        }
        else {
            this->parameters.push_back(part);
        }

        part = t.next(";:|");
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
        db_sort_t sortDirection = sort_descending;

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
                        sortDirection = sort_ascending;
                    }
                    else if (parameter.compare("desc") == 0) {
                        sortDirection = sort_descending;
                    }
                }
            }
        }

        listTransactions(rowLimit, sortDirection, includeRecurringTransactions);
    }
    else if (isCommand("find-transactions") || isCommand("find")) {
        if (hasParameters()) {
            FindTransactionCriteriaBuilder builder(this->parameters);
            findTransactions(builder.getCriteria());
        }
        else {
            findTransactions();
        }
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

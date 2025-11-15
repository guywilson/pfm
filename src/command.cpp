#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <vector>
#include <unordered_map>
#include <functional>

#include <sqlcipher/sqlite3.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "pfm_error.h"
#include "db.h"
#include "cfgmgr.h"
#include "cache.h"
#include "strdate.h"
#include "version.h"
#include "db_account.h"
#include "db_category.h"
#include "db_payee.h"
#include "db_recurring_charge.h"
#include "db_transaction.h"
#include "jfile.h"
#include "command.h"

using namespace std;

struct CommandEntry {
    string                      name;
    vector<string>              aliases;

    function<void(Command&)>    handler;
};

static const vector<CommandEntry> commandTable = {
    {"save-json-template",         {"sjt"},                         [](Command& c){ c.saveJsonTemplate(); }},
    {"add-account",                {"aa"},                          [](Command& c){ c.addAccount(); }},
    {"list-accounts",              {"la"},                          [](Command& c){ c.listAccounts(); }},
    {"use",                        {"select"},                      [](Command& c){ c.chooseAccount(); }},
    {"set-primary-account",        {"spa"},                         [](Command& c){ c.setPrimaryAccount(); }},
    {"update-account",             {"ua"},                          [](Command& c){ c.updateAccount(); }},
    {"delete-account",             {"da"},                          [](Command& c){ c.deleteAccount(); }},
    {"import-accounts",            {"ia"},                          [](Command& c){ c.importAccounts(); }},
    {"export-accounts",            {"xa"},                          [](Command& c){ c.exportAccounts(); }},
    {"add-config-item",            {"acfg"},                        [](Command& c){ c.addConfig(); }},
    {"list-config-items",          {"lcfg"},                        [](Command& c){ c.listConfigItems(); }},
    {"update-config-item",         {"ucfg"},                        [](Command& c){ c.updateConfig(); }},
    {"delete-config-item",         {"dcfg"},                        [](Command& c){ c.deleteConfig(); }},
    {"add-category",               {"ac"},                          [](Command& c){ c.addCategory(); }},
    {"list-categories",            {"lc"},                          [](Command& c){ c.listCategories(); }},
    {"update-category",            {"uc"},                          [](Command& c){ c.updateCategory(); }},
    {"delete-category",            {"dc"},                          [](Command& c){ c.deleteCategory(); }},
    {"import-categories",          {"ic"},                          [](Command& c){ c.importCategories(); }},
    {"export-categories",          {"xc"},                          [](Command& c){ c.exportCategories(); }},
    {"clear-categories",           {"cc"},                          [](Command& c){ c.clearCategories(); }},
    {"add-payee",                  {"ap"},                          [](Command& c){ c.addPayee(); }},
    {"list-payees",                {"lp"},                          [](Command& c){ c.listPayees(); }},
    {"update-payee",               {"up"},                          [](Command& c){ c.updatePayee(); }},
    {"delete-payee",               {"dp"},                          [](Command& c){ c.deletePayee(); }},
    {"import-payees",              {"ip"},                          [](Command& c){ c.importPayees(); }},
    {"export-payees",              {"xp"},                          [](Command& c){ c.exportPayees(); }},
    {"add-recurring-charge",       {"arc"},                         [](Command& c){ c.addRecurringCharge(); }},
    {"list-recurring-charges",     {"lrc"},                         [](Command& c){ c.listRecurringCharges(); }},
    {"update-recurring-charge",    {"urc"},                         [](Command& c){ c.updateRecurringCharge(); }},
    {"delete-recurring-charge",    {"drc"},                         [](Command& c){ c.deleteRecurringCharge(); }},
    {"import-recurring-charges",   {"irc"},                         [](Command& c){ c.importRecurringCharges(); }},
    {"export-recurring-charges",   {"xrc"},                         [](Command& c){ c.exportRecurringCharges(); }},
    {"migrate-recurring-charge",   {"mrc"},                         [](Command& c){ c.migrateCharge(); }},
    {"add-transaction",            {"at", "add"},                   [](Command& c){ c.addTransaction(); }},
    {"transfer-transaction",       {"tr", "transfer"},              [](Command& c){ c.addTransferTransaction(); }},
    {"list-transactions",          {"lt", "list"},                  [](Command& c){ c.listTransactions(); }},
    {"find-transactions",          {"find"},                        [](Command& c){ c.findTransactions(); }},
    {"category-report",            {"cr"},                          [](Command& c){ c.transactionsByCategory(); }},
    {"payee-report",               {"pr"},                          [](Command& c){ c.transactionsByPayee(); }},
    {"update-transaction",         {"ut"},                          [](Command& c){ c.updateTransaction(); }},
    {"delete-transaction",         {"dt"},                          [](Command& c){ c.deleteTransaction(); }},
    {"reconcile-transaction",      {"reconcile", "rt"},             [](Command& c){ c.reconcileTransaction(); }},
    {"import-transactions",        {"it"},                          [](Command& c){ c.importTransactions(); }},
    {"export-transactions",        {"xt"},                          [](Command& c){ c.exportTransactions(); }},
    {"export-transactions-csv",    {"xtc"},                         [](Command& c){ c.exportTransactionsAsCSV(); }},
    {"add-report",                 {"arp"},                         [](Command& c){ c.addReport(); }},
    {"copy-report",                {"crp"},                         [](Command& c){ c.copyReport(); }},
    {"list-reports",               {"show-reports", "lrp"},         [](Command& c){ c.listReports(); }},
    {"update-report",              {"urp"},                         [](Command& c){ c.updateReport(); }},
    {"delete-report",              {"drp"},                         [](Command& c){ c.deleteReport(); }},
    {"run-report",                 {"run"},                         [](Command& c){ c.runReport(); }},
    {"save-report",                {"save"},                        [](Command& c){ c.saveReport(); }},
    {"show-report",                {"srp"},                         [](Command& c){ c.showReport(); }},
    {"list-carried-over-logs",     {"lco"},                         [](Command& c){ c.listCarriedOverLogs(); }},
    {"change-password",            {},                              [](Command& c){ c.changePassword(); }},
    {"clear-recurring-transactions", {},                            [](Command& c){ c.clearRecurringTransactions(); }},
    {"set-logging-level",          {},                              [](Command& c){ c.setLoggingLevel(); }},
    {"clear-logging-level",        {},                              [](Command& c){ c.clearLoggingLevel(); }},
    {"get-db-key",                 {},                              [](Command& c){ c.getDBKey(); }},
    {"save-db-key",                {},                              [](Command& c){ c.saveDBKey(); }}
};

void Command::help() {
    cout << "For help, please see the manual" << endl;
    cout << "\tman pfm" << endl << endl;
}

void Command::version() {
    cout << "PFM version '" << getVersion() << "' - built [" << getBuildDate() << "]" << endl << endl;
}

string Command::parse(const string & commandLine) {
    const char * parameterDelimiters = ";:|";

    char * cmdString = strdup(commandLine.c_str());;
    char * part = strtok(cmdString, " ");

    this->parameters.clear();

    string command;

    int i = 0;
    while (part != NULL) {
        if (i == 0) {
            command = part;
        }
        else {
            this->parameters.push_back(part);
        }

        part = strtok(NULL, parameterDelimiters);
        i++;
    }

    return command;
}

bool Command::process(const string & commandLine) {
    clear_history();

    for (string & cmd : commandHistory) {
        add_history(cmd.c_str());
    }

    string command = parse(commandLine);

    add_history(commandLine.c_str());
    commandHistory.push_back(commandLine);

    if (command.compare("exit") == 0 || command.compare("quit") == 0 || command.compare("q") == 0) {
        return false;
    }
    else if (command.compare("version") == 0) {
        Command::version();
        return true;
    }
    else if (command.compare("help") == 0) {
        Command::help();
        return true;
    }
    else {
        bool isAlias = false;
        for (const auto &entry : commandTable) {
            for (const auto & alias : entry.aliases) {
                if (command == alias) {
                    isAlias = true;
                }
            }
            if (command == entry.name || isAlias) {
                entry.handler(*this);

                for (string & command : commandHistory) {
                    add_history(command.c_str());
                }

                return true;
            }
        }
    }

    throw pfm_validation_error(
                pfm_error::buildMsg(
                    "Sorry, I do not understand command '%s', please see the manual for supported commands.", 
                    command.c_str()));
}

#include <string>
#include <vector>
#include <functional>

#include "command.h"

using namespace std;

#ifndef __INCL_COMMAND_TABLE
#define __INCL_COMMAND_TABLE

struct CommandEntry {
    string                      name;
    vector<string>              aliases;
    string                      helpText;

    function<void(Command&)>    handler;
};

const vector<CommandEntry> commandTable = {
    {"save-json-template", {"sjt"},
        "Save a json template for the selected entity", 
        [](Command& c){ c.saveJsonTemplate(); }},
    
    {"add-account", {"aa"},
        "Add a new account", 
        [](Command& c){ c.addAccount(); }},

    {"list-accounts", {"la"}, 
        "List all accounts", 
        [](Command& c){ c.listAccounts(); }},

    {"use", {"select"}, 
        "Use (select) the account with the specified code", 
        [](Command& c){ c.chooseAccount(); }},

    {"set-primary-account", {"spa"}, 
        "Set the primary account that is auto selected on start-up", 
        [](Command& c){ c.setPrimaryAccount(); }},
        
    {"update-account", {"ua"}, 
        "Update the account with the specified code", 
        [](Command& c){ c.updateAccount(); }},

    {"delete-account", {"da"}, 
        "Delete the account with the specified code and remove all it's transactions and charges", 
        [](Command& c){ c.deleteAccount(); }},

    {"import-accounts", {"ia"}, 
        "Import accounts from the specified JSON file", 
        [](Command& c){ c.importAccounts(); }},

    {"export-accounts", {"xa"}, 
        "Export all accounts in JSON format to the specified file", 
        [](Command& c){ c.exportAccounts(); }},
        
    {"add-config-item", {"add-cfg", "acfg"}, 
        "Add a new config item", 
        [](Command& c){ c.addConfig(); }},
        
    {"list-config-items", {"lcfg"}, 
        "List all config items", 
        [](Command& c){ c.listConfigItems(); }},

    {"update-config-item", {"ucfg"}, 
        "Update the specified config item", 
        [](Command& c){ c.updateConfig(); }},

    {"delete-config-item", {"dcfg"}, 
        "Delete the specified config item", 
        [](Command& c){ c.deleteConfig(); }},

    {"add-category", {"ac"}, 
        "", 
        [](Command& c){ c.addCategory(); }},

    {"list-categories", {"lc"}, 
        "", 
        [](Command& c){ c.listCategories(); }},

    {"update-category", {"uc"}, 
        "", 
        [](Command& c){ c.updateCategory(); }},

    {"delete-category", {"dc"}, 
        "", 
        [](Command& c){ c.deleteCategory(); }},

    {"import-categories", {"ic"}, 
        "", 
        [](Command& c){ c.importCategories(); }},

    {"export-categories", {"xc"}, 
        "", 
        [](Command& c){ c.exportCategories(); }},

    {"clear-categories", {"cc"}, 
        "", 
        [](Command& c){ c.clearCategories(); }},

    {"add-payee", {"ap"}, 
        "", 
        [](Command& c){ c.addPayee(); }},

    {"list-payees", {"lp"}, 
        "", 
        [](Command& c){ c.listPayees(); }},

    {"update-payee", {"up"}, 
        "", 
        [](Command& c){ c.updatePayee(); }},

    {"delete-payee", {"dp"}, 
        "", 
        [](Command& c){ c.deletePayee(); }},

    {"import-payees", {"ip"}, 
        "", 
        [](Command& c){ c.importPayees(); }},

    {"export-payees", {"xp"}, 
        "", 
        [](Command& c){ c.exportPayees(); }},

    {"add-recurring-charge", {"arc"}, 
        "", 
        [](Command& c){ c.addRecurringCharge(); }},

    {"list-recurring-charges", {"lrc"}, 
        "", 
        [](Command& c){ c.listRecurringCharges(); }},

    {"update-recurring-charge", {"urc"}, 
        "", 
        [](Command& c){ c.updateRecurringCharge(); }},

    {"delete-recurring-charge", {"drc"}, 
        "", 
        [](Command& c){ c.deleteRecurringCharge(); }},

    {"import-recurring-charges", {"irc"}, 
        "", 
        [](Command& c){ c.importRecurringCharges(); }},

    {"export-recurring-charges", {"xrc"}, 
        "", 
        [](Command& c){ c.exportRecurringCharges(); }},

    {"migrate-recurring-charge", {"mrc"}, 
        "", 
        [](Command& c){ c.migrateCharge(); }},

    {"add-transaction", {"at", "add"}, 
        "", 
        [](Command& c){ c.addTransaction(); }},

    {"transfer-transaction", {"tr", "transfer"}, 
        "", 
        [](Command& c){ c.addTransferTransaction(); }},

    {"list-transactions", {"lt", "list"}, 
        "", 
        [](Command& c){ c.listTransactions(); }},

    {"find-transactions", {"ft", "find"}, 
        "", 
        [](Command& c){ c.findTransactions(); }},

    {"category-report", {"cr"}, 
        "", 
        [](Command& c){ c.transactionsByCategory(); }},

    {"payee-report", {"pr"}, 
        "", 
        [](Command& c){ c.transactionsByPayee(); }},

    {"update-transaction", {"ut"}, 
        "", 
        [](Command& c){ c.updateTransaction(); }},

    {"delete-transaction", {"dt"}, 
        "", 
        [](Command& c){ c.deleteTransaction(); }},

    {"reconcile-transaction", {"reconcile", "rt"}, 
        "", 
        [](Command& c){ c.reconcileTransaction(); }},

    {"import-transactions", {"it"}, 
        "", 
        [](Command& c){ c.importTransactions(); }},

    {"export-transactions", {"xt"}, 
        "", 
        [](Command& c){ c.exportTransactions(); }},

    {"export-transactions-csv", {"xtc"}, 
        "", 
        [](Command& c){ c.exportTransactionsAsCSV(); }},

    {"add-report", {"arp"}, 
        "", 
        [](Command& c){ c.addReport(); }},

    {"copy-report", {"crp"}, 
        "", 
        [](Command& c){ c.copyReport(); }},

    {"list-reports", {"show-reports", "lrp"}, 
        "", 
        [](Command& c){ c.listReports(); }},

    {"update-report", {"urp"}, 
        "", 
        [](Command& c){ c.updateReport(); }},

    {"delete-report", {"drp"}, 
        "", 
        [](Command& c){ c.deleteReport(); }},

    {"run-report", {"run"}, 
        "", 
        [](Command& c){ c.runReport(); }},

    {"save-report", {"save"}, 
        "", 
        [](Command& c){ c.saveReport(); }},

    {"show-report", {"srp", "show"}, 
        "", 
        [](Command& c){ c.showReport(); }},

    {"list-carried-over-logs", {"lco"}, 
        "", 
        [](Command& c){ c.listCarriedOverLogs(); }},

    {"change-password", {}, 
        "", 
        [](Command& c){ c.changePassword(); }},

    {"clear-recurring-transactions", {}, 
        "", 
        [](Command& c){ c.clearRecurringTransactions(); }},

    {"set-logging-level", {}, 
        "", 
        [](Command& c){ c.setLoggingLevel(); }},

    {"clear-logging-level", {}, 
        "", 
        [](Command& c){ c.clearLoggingLevel(); }},

    {"get-db-key", {}, 
        "", 
        [](Command& c){ c.getDBKey(); }},

    {"save-db-key", {}, 
        "", 
        [](Command& c){ c.saveDBKey(); }}
};

#endif

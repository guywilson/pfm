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
        "Add a new category", 
        [](Command& c){ c.addCategory(); }},

    {"list-categories", {"lc"}, 
        "List all categories", 
        [](Command& c){ c.listCategories(); }},

    {"update-category", {"uc"}, 
        "Update the category with the specified code", 
        [](Command& c){ c.updateCategory(); }},

    {"delete-category", {"dc"}, 
        "Delete the category with the specified code", 
        [](Command& c){ c.deleteCategory(); }},

    {"import-categories", {"ic"}, 
        "Import categories from the specified JSON file", 
        [](Command& c){ c.importCategories(); }},

    {"export-categories", {"xc"}, 
        "Export all categories to the specified file in JSON format", 
        [](Command& c){ c.exportCategories(); }},

    {"clear-categories", {"cc"}, 
        "Delete all categories, useful if you don't want the default", 
        [](Command& c){ c.clearCategories(); }},

    {"add-payee", {"ap"}, 
        "Add a new payee", 
        [](Command& c){ c.addPayee(); }},

    {"list-payees", {"lp"}, 
        "List all payees", 
        [](Command& c){ c.listPayees(); }},

    {"update-payee", {"up"}, 
        "Update the payee with the specified code", 
        [](Command& c){ c.updatePayee(); }},

    {"delete-payee", {"dp"}, 
        "Delete the payee with the specified code", 
        [](Command& c){ c.deletePayee(); }},

    {"import-payees", {"ip"}, 
        "Import payees from the specified JSON file", 
        [](Command& c){ c.importPayees(); }},

    {"export-payees", {"xp"}, 
        "Export all payees to the specified file in JSON format", 
        [](Command& c){ c.exportPayees(); }},

    {"add-recurring-charge", {"arc"}, 
        "Add a new recurring charge for the currently selected account", 
        [](Command& c){ c.addRecurringCharge(); }},

    {"list-recurring-charges", {"lrc"}, 
        "List all recurring charges for the currently selected account", 
        [](Command& c){ c.listRecurringCharges(); }},

    {"list-outstanding-charges", {"loc"}, 
        "List all oustanding charges for this period for the currently selected account", 
        [](Command& c){ c.listOutstandingCharges(); }},

    {"update-recurring-charge", {"urc"}, 
        "Update the recurring charge specified with sequence supplied", 
        [](Command& c){ c.updateRecurringCharge(); }},

    {"delete-recurring-charge", {"drc"}, 
        "Delete the recurring charge specified with the sequence supplied", 
        [](Command& c){ c.deleteRecurringCharge(); }},

    {"import-recurring-charges", {"irc"}, 
        "Import recurring charges from the specified JSON file", 
        [](Command& c){ c.importRecurringCharges(); }},

    {"export-recurring-charges", {"xrc"}, 
        "Export all recurring charges to the specified file in JSON format", 
        [](Command& c){ c.exportRecurringCharges(); }},

    {"migrate-recurring-charge", {"mrc"}, 
        "", 
        [](Command& c){ c.migrateCharge(); }},

    {"add-transaction", {"at", "add"}, 
        "Add a new transaction for the currently selected account", 
        [](Command& c){ c.addTransaction(); }},

    {"transfer-transaction", {"tr", "transfer"}, 
        "Add a new transfer transaction for the currently selected account", 
        [](Command& c){ c.addTransferTransaction(); }},

    {"list-transactions", {"lt", "list"}, 
        "List all transactions for the currently selected account", 
        [](Command& c){ c.listTransactions(); }},

    {"find-transactions", {"ft", "find"}, 
        "Find transactions based on the criteria supplied. If no criteria are supplied, the user is prompted", 
        [](Command& c){ c.findTransactions(); }},

    {"category-report", {"cr"}, 
        "Show a report across all categories", 
        [](Command& c){ c.transactionsByCategory(); }},

    {"payee-report", {"pr"}, 
        "Show a report across all payees", 
        [](Command& c){ c.transactionsByPayee(); }},

    {"update-transaction", {"ut"}, 
        "Update the transaction specified with the sequence supplied", 
        [](Command& c){ c.updateTransaction(); }},

    {"delete-transaction", {"dt"}, 
        "Delete the transaction specified with the sequence supplied", 
        [](Command& c){ c.deleteTransaction(); }},

    {"reconcile-transaction", {"reconcile", "rt"}, 
        "Reconcile the transaction specified with the sequence supplied", 
        [](Command& c){ c.reconcileTransaction(); }},

    {"import-transactions", {"it"}, 
        "Import transactions from the specified JSON file", 
        [](Command& c){ c.importTransactions(); }},

    {"export-transactions", {"xt"}, 
        "Export all transactions to the specified file in JSON format", 
        [](Command& c){ c.exportTransactions(); }},

    {"export-transactions-csv", {"xtc"}, 
        "Export all transactions to the specified file in CSV format", 
        [](Command& c){ c.exportTransactionsAsCSV(); }},

    {"add-report", {"arp"}, 
        "Add a new report", 
        [](Command& c){ c.addReport(); }},

    {"copy-report", {"crp"}, 
        "Copy an existing report specified by the supplied sequence", 
        [](Command& c){ c.copyReport(); }},

    {"list-reports", {"show-reports", "lrp"}, 
        "List all reports", 
        [](Command& c){ c.listReports(); }},

    {"update-report", {"urp"}, 
        "Update the report specified by the supplied sequence", 
        [](Command& c){ c.updateReport(); }},

    {"delete-report", {"drp"}, 
        "Delete the report specified by the supplied sequence", 
        [](Command& c){ c.deleteReport(); }},

    {"run-report", {"run"}, 
        "Run the report specified by the supplied sequence, the same as running the find command with the same criteria", 
        [](Command& c){ c.runReport(); }},

    {"save-report", {"save"}, 
        "Save the criteria used to issue a find command to a new report", 
        [](Command& c){ c.saveReport(); }},

    {"show-report", {"srp", "show"}, 
        "Show the criteria of the report specified with the supplied sequence", 
        [](Command& c){ c.showReport(); }},

    {"add-shortcut", {"ash"}, 
        "Add a new shortcut", 
        [](Command& c){ c.addShortcut(); }},

    {"list-shortcuts", {"lsh"}, 
        "List all shortcuts", 
        [](Command& c){ c.listShortcuts(); }},

    {"update-shortcut", {"ush"}, 
        "Update the shortcut specified by the supplied sequence", 
        [](Command& c){ c.updateShortcut(); }},

    {"delete-shortcut", {"dsh"}, 
        "Delete the shortcut specified by the supplied sequence", 
        [](Command& c){ c.deleteShortcut(); }},

    {"list-carried-over-logs", {"lco"}, 
        "", 
        [](Command& c){ c.listCarriedOverLogs(); }},

    {"change-password", {}, 
        "Change the database password and re-encrypt the db", 
        [](Command& c){ c.changePassword(); }},

    {"sql-mode", {}, 
        "", 
        [](Command& c){ c.enterSQLMode(); }},

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
        "Save the database key to an encrypted file in the users current directory. Useful to access pfm without having to enter the password each time", 
        [](Command& c){ c.saveDBKey(); }}
};

#endif

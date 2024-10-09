#include <string>
#include <vector>
#include <string.h>

#include "logger.h"
#include "db_account.h"
#include "db_category.h"
#include "db_payee.h"
#include "db_transaction.h"
#include "db_budget.h"
#include "db_carried_over.h"
#include "db_budget_track.h"

using namespace std;

#ifndef __INCL_COMMAND
#define __INCL_COMMAND

class Command {
    public:
        typedef enum {
            pfm_cmd_exit,
            pfm_cmd_help,
            pfm_cmd_version,
            pfm_cmd_account_add,
            pfm_cmd_account_list,
            pfm_cmd_account_use,
            pfm_cmd_account_update,
            pfm_cmd_account_delete,
            pfm_cmd_account_balance,
            pfm_cmd_account_import,
            pfm_cmd_account_export,
            pfm_cmd_category_add,
            pfm_cmd_category_list,
            pfm_cmd_category_update,
            pfm_cmd_category_delete,
            pfm_cmd_category_import,
            pfm_cmd_category_export,
            pfm_cmd_payee_add,
            pfm_cmd_payee_list,
            pfm_cmd_payee_update,
            pfm_cmd_payee_delete,
            pfm_cmd_payee_import,
            pfm_cmd_payee_export,
            pfm_cmd_charge_add,
            pfm_cmd_charge_list,
            pfm_cmd_charge_update,
            pfm_cmd_charge_delete,
            pfm_cmd_charge_import,
            pfm_cmd_charge_export,
            pfm_cmd_transaction_add,
            pfm_cmd_transaction_list,
            pfm_cmd_transaction_find,
            pfm_cmd_transaction_update,
            pfm_cmd_transaction_delete,
            pfm_cmd_transaction_import,
            pfm_cmd_transaction_export,
            pfm_cmd_budget_add,
            pfm_cmd_budget_list,
            pfm_cmd_budget_update,
            pfm_cmd_budget_delete,
            pfm_cmd_budget_import,
            pfm_cmd_budget_export,
            pfm_cmd_debug_carried_over,
            pfm_cmd_debug_budget_track,
            pfm_cmd_debug_set_today,
            pfm_cmd_debug_clear_today,
            pfm_cmd_util_clear_recurring_transactions,
            pfm_cmd_util_clear_categories,
            pfm_cmd_logging_level_set,
            pfm_cmd_logging_level_clear
        }
        pfm_cmd_t;

    private:
        string command;
        vector<string> commandHistory;
        DBAccount selectedAccount;

        Logger & log = Logger::getInstance();

        bool isCommand(const char * compareTo) {
            string commandNoParameters = command.substr(0, command.find_last_of(' '));

            return (commandNoParameters.compare(compareTo) == 0 ? true : false);
        }

        string getCommandParameter() {
            return command.substr(command.find_last_of(' ') + 1);
        }

        const char * getNoAccountSelectedMsg() {
            return "No account selected, please choose an account with the use [account code] command";
        }

        void checkAccountSelected() {
            if (selectedAccount.id == 0) {
                throw pfm_error(getNoAccountSelectedMsg());
            }
        }

        void addAccount();
        void listAccounts();
        void chooseAccount(string & accountCode);
        void updateAccount();
        void showAccountBalances(DBAccount & account);
        void importAccounts(string & jsonFileName);
        void exportAccounts(string & jsonFileName);

        void addCategory();
        void listCategories();
        DBCategory getCategory(string & categoryCode);
        void updateCategory(DBCategory & category);
        void deleteCategory(DBCategory & category);
        void importCategories(string & jsonFileName);
        void exportCategories(string & jsonFileName);
        void clearCategories();

        void addPayee();
        void listPayees();
        DBPayee getPayee(string & payeeCode);
        void updatePayee(DBPayee & payee);
        void deletePayee(DBPayee & payee);
        void importPayees(string & jsonFileName);
        void exportPayees(string & jsonFileName);

        void addRecurringCharge();
        void listRecurringCharges();
        DBRecurringCharge getRecurringCharge(int sequence);
        void updateRecurringCharge(DBRecurringCharge & charge);
        void deleteRecurringCharge(DBRecurringCharge & charge);
        void importRecurringCharges(string & jsonFileName);
        void exportRecurringCharges(string & jsonFileName);
        void clearRecurringTransactions();

        void addTransaction();
        void listTransactions(bool isOnlyNonRecurring);
        void findTransactions();
        DBTransaction getTransaction(int sequence);
        void updateTransaction(DBTransaction & transaction);
        void deleteTransaction(DBTransaction & transaction);
        void importTransactions(string & jsonFileName);
        void exportTransactions(string & jsonFileName);

        void addBudget();
        void listBudgets();
        DBBudget getBudget(int sequence);
        void updateBudget(DBBudget & budget);
        void deleteBudget(DBBudget & budget);
        void importBudgets(string & jsonFileName);
        void exportBudgets(string & jsonFileName);

        void listCarriedOverLogs();
        void listBudgetTracks();

        int getLogLevelParameter(string & level);
        void setLoggingLevel(string & level);
        void clearLoggingLevel(string & level);

        bool isCommandValid();
        pfm_cmd_t getCommandCode(const string & command);

    public:
        static void help();
        static void version();

        bool process(const string & command);
};

#endif

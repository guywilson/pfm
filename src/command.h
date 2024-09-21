#include <string>
#include <string.h>

#include <nlohmann/json.hpp>

#include "logger.h"
#include "db_account.h"
#include "db_category.h"
#include "db_payee.h"
#include "db_transaction.h"
#include "db_budget.h"
#include "db_user.h"
#include "db_carried_over.h"
#include "db_budget_track.h"

using namespace std;
using json = nlohmann::json;

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
            pfm_cmd_category_add,
            pfm_cmd_category_list,
            pfm_cmd_category_update,
            pfm_cmd_category_delete,
            pfm_cmd_category_import,
            pfm_cmd_payee_add,
            pfm_cmd_payee_list,
            pfm_cmd_payee_update,
            pfm_cmd_payee_delete,
            pfm_cmd_charge_add,
            pfm_cmd_charge_list,
            pfm_cmd_charge_update,
            pfm_cmd_charge_delete,
            pfm_cmd_transaction_add,
            pfm_cmd_transaction_list,
            pfm_cmd_transaction_find,
            pfm_cmd_transaction_update,
            pfm_cmd_transaction_delete,
            pfm_cmd_budget_add,
            pfm_cmd_budget_list,
            pfm_cmd_budget_update,
            pfm_cmd_budget_delete,
            pfm_cmd_debug_carried_over,
            pfm_cmd_debug_budget_track
        }
        pfm_cmd_t;

    private:
        string command;
        DBAccount selectedAccount;
        DBUser loggedInUser;

        Logger & log = Logger::getInstance();

        bool isCommand(const char * compareTo) {
            string commandNoParameters = command.substr(0, command.find_first_of(' '));

            return (commandNoParameters.compare(compareTo) == 0 ? true : false);
        }

        string getCommandParameter() {
            return command.substr(command.find_first_of(' ') + 1);
        }

        const char * getNoAccountSelectedMsg() {
            return "No account selected, please choose an account with the use [account code] command";
        }

        void checkAccountSelected() {
            if (selectedAccount.id == 0) {
                throw pfm_error(getNoAccountSelectedMsg());
            }
        }

        json getJson(string & filename);
        void validateJsonClass(json & data, const char * expectedClassName);

        static DBUser addUser();

        void addAccount(DBUser & user);
        void listAccounts();
        void chooseAccount(string & accountCode);
        void updateAccount();
        void showAccountBalances(DBAccount & account);

        void addCategory();
        void listCategories();
        DBCategory getCategory(string & categoryCode);
        void updateCategory(DBCategory & category);
        void deleteCategory(DBCategory & category);
        void importCategories(string & jsonFileName);

        void addPayee();
        void listPayees();
        DBPayee getPayee(string & payeeCode);
        void updatePayee(DBPayee & payee);
        void deletePayee(DBPayee & payee);
        void importPayees(string & jsonFileName);

        void addRecurringCharge();
        void listRecurringCharges();
        DBRecurringCharge getRecurringCharge(int sequence);
        void updateRecurringCharge(DBRecurringCharge & charge);
        void deleteRecurringCharge(DBRecurringCharge & charge);
        // void importRecurringCharges(string & jsonFileName);

        void addTransaction();
        void listTransactions();
        void findTransactions();
        DBTransaction getTransaction(int sequence);
        void updateTransaction(DBTransaction & transaction);
        void deleteTransaction(DBTransaction & transaction);

        void addBudget();
        void listBudgets();
        DBBudget getBudget(int sequence);
        void updateBudget(DBBudget & budget);
        void deleteBudget(DBBudget & budget);

        void listCarriedOverLogs();
        void listBudgetTracks();

        bool isCommandValid();
        pfm_cmd_t getCommandCode(string & command);

    public:
        void setLoggedInUser(DBUser & user) {
            this->loggedInUser = user;
        }

        static void help();
        static void version();

        static DBUser login();
        bool process(string & command);
};

#endif

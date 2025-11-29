#include <string>
#include <vector>
#include <unordered_map>
#include <string.h>
#include <exception>

#include "logger.h"
#include "cfgmgr.h"
#include "db.h"
#include "db_base.h"
#include "db_account.h"
#include "db_config.h"
#include "db_category.h"
#include "db_payee.h"
#include "db_v_transaction.h"
#include "db_transaction.h"
#include "db_carried_over.h"
#include "db_transaction_report.h"
#include "db_shortcut.h"

using namespace std;

#ifndef __INCL_COMMAND
#define __INCL_COMMAND

#define SIMPLE_PARAM_NAME                   "param"
#define SEQUENCE_PARAM_NAME                 "sequence"

class Command {
    private:
        unordered_map<std::string, std::vector<std::string>> parameters;

        vector<string> commandHistory;
        DBAccount selectedAccount;

        Logger & log = Logger::getInstance();
        cfgmgr & cfg = cfgmgr::getInstance();

        bool isStringNumeric(const string & s);

        string parse(const string & commandLine);
        void handleExceptions(const string & command, const string & token);
 
        bool hasParameters() {
            return (this->parameters.size() > 0);
        }

        int getNumParameters() {
            return this->parameters.size();
        }

        vector<string> getParameters(const string & key) const {
            if (parameters.size() == 0) {
                throw pfm_error("Expected parameters but none were supplied");
            }

            try {
                vector<string> values = parameters.at(key);
                return values;
            }
            catch (out_of_range & e) {
                return {};
            }
        }

        string getParameter(const string & key) const {
            if (parameters.size() == 0) {
                throw pfm_error("Expected parameters but none were supplied");
            }

            vector<string> values = getParameters(key);
            return values.empty() ? "" : values[0];
        }

        static string trim(const string & s) {
            const char * whitespace = " \t\n\r\f\v";

            const auto start = s.find_first_not_of(whitespace);

            if (start == string::npos) {
                return "";
            }

            const auto end = s.find_last_not_of(whitespace);

            return s.substr(start, end - start + 1);
        }

        const char * getNoAccountSelectedMsg() {
            return "No account selected, please choose an account with the use [account code] command";
        }

        void checkAccountSelected() {
            if (selectedAccount.id == 0) {
                throw pfm_error(getNoAccountSelectedMsg());
            }
        }

    public:
        void saveJsonTemplate();
        
        void addAccount();
        void listAccounts();
        void chooseAccount();
        void setPrimaryAccount();
        void updateAccount();
        void deleteAccount();
        void importAccounts();
        void exportAccounts();

        void addConfig();
        void listConfigItems();
        DBConfig getConfig(string & key);
        void updateConfig();
        void deleteConfig();

        void addCategory();
        void listCategories();
        DBCategory getCategory(string & categoryCode);
        void updateCategory();
        void deleteCategory();
        void importCategories();
        void exportCategories();
        void clearCategories();

        void addPayee();
        void listPayees();
        DBPayee getPayee(string & payeeCode);
        void updatePayee();
        void deletePayee();
        void importPayees();
        void exportPayees();

        void addRecurringCharge();
        void listRecurringCharges();
        DBRecurringCharge getRecurringCharge(int sequence);
        void updateRecurringCharge();
        void deleteRecurringCharge();
        void importRecurringCharges();
        void exportRecurringCharges();
        void migrateCharge();
        void clearRecurringTransactions();

        void addTransaction();
        void addTransferTransaction();
        void listTransactions();
        void findTransactions();
        void findTransactions(const string & where);
        void findTransactions(DBCriteria & criteria);
        void transactionsByCategory();
        void transactionsByPayee();
        string buildFindTransactionCriteria();
        DBTransaction getTransaction(int sequence);
        void updateTransaction();
        void deleteTransaction();
        void reconcileTransaction();
        void importTransactions();
        void exportTransactions();
        void exportTransactionsAsCSV();

        void addReport();
        void copyReport();
        void listReports();
        DBTransactionReport getReport(int sequence);
        void updateReport();
        void deleteReport();
        void runReport();
        void saveReport();
        void showReport();

        void addShortcut();
        void listShortcuts();
        DBShortcut getShortcut(int sequence);
        void updateShortcut();
        void deleteShortcut();

        void listCarriedOverLogs();

        void changePassword();
        void getDBKey();
        void saveDBKey();

        int getLogLevelParameter(string & level);
        void setLoggingLevel();
        void clearLoggingLevel();

        static void help();
        static void version();

        bool process(const string & commandLine);
};

#endif

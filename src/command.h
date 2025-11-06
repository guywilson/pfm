#include <string>
#include <vector>
#include <string.h>

#include "logger.h"
#include "cfgmgr.h"
#include "db_account.h"
#include "db_config.h"
#include "db_category.h"
#include "db_payee.h"
#include "db_v_transaction.h"
#include "db_transaction.h"
#include "db_carried_over.h"
#include "db_transaction_report.h"
#include "transaction_criteria.h"

using namespace std;

#ifndef __INCL_COMMAND
#define __INCL_COMMAND

class Command {
    private:
        string command;
        vector<string> parameters;

        vector<string> commandHistory;
        DBAccount selectedAccount;

        Logger & log = Logger::getInstance();
        cfgmgr & cfg = cfgmgr::getInstance();

        void parse(const string & command);
        
        bool isCommand(const char * compareTo) {
            return (command.compare(compareTo) == 0 ? true : false);
        }

        bool hasParameters() {
            return (this->parameters.size() > 0);
        }

        int getNumParameters() {
            return this->parameters.size();
        }

        string getParameter(int index) const {
            if (parameters.size() == 0) {
                throw pfm_error("Expected parameters but none were supplied");
            }
            else if (index >= (int)parameters.size()) {
                throw pfm_error(
                        pfm_error::buildMsg(
                            "Expecting at least %d parameters but only %d have been supplied", 
                            index + 1, 
                            parameters.size()));
            }

            return parameters[index];
        }

        const char * getNoAccountSelectedMsg() {
            return "No account selected, please choose an account with the use [account code] command";
        }

        void checkAccountSelected() {
            if (selectedAccount.id == 0) {
                throw pfm_error(getNoAccountSelectedMsg());
            }
        }

        void saveJsonTemplate();
        
        void addAccount();
        void listAccounts();
        void chooseAccount(string & accountCode);
        void setPrimaryAccount(string & accountCode);
        void updateAccount();
        void importAccounts(string & jsonFileName);
        void exportAccounts(string & jsonFileName);

        void addConfig();
        void listConfigItems();
        DBConfig getConfig(string & key);
        void updateConfig(DBConfig & config);
        void deleteConfig(DBConfig & config);

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
        void addTransaction(AddTransactionCriteriaBuilder & builder);
        void addTransferTransaction();
        void migrateCharge(DBRecurringCharge & charge);
        void listTransactions(uint32_t rowLimit, db_sort_t sortDirection, bool includeRecurring);
        void findTransactions();
        void findTransactions(const string & criteria);
        void transactionsByCategory();
        void transactionsByPayee();
        string buildFindTransactionCriteria();
        DBTransaction getTransaction(int sequence);
        void updateTransaction(DBTransaction & transaction);
        void deleteTransaction(DBTransaction & transaction);
        void reconcileTransaction(DBTransaction & transaction);
        void importTransactions(string & jsonFileName);
        void exportTransactions(string & jsonFileName);
        void exportTransactionsAsCSV(string & csvFileName);

        void addReport();
        void copyReport(DBTransactionReport & report);
        void listReports();
        DBTransactionReport getReport(int sequence);
        void updateReport(DBTransactionReport & report);
        void deleteReport(DBTransactionReport & report);
        void runReport(DBTransactionReport & report);
        void saveReport(const string & description);
        void showReport(DBTransactionReport & report);

        void listCarriedOverLogs();

        void changePassword();
        void getDBKey();
        void saveDBKey();

        int getLogLevelParameter(string & level);
        void setLoggingLevel(string & level);
        void clearLoggingLevel(string & level);

    public:
        static void help();
        static void version();

        bool process(const string & command);
};

#endif

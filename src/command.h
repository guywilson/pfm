#include <string>
#include <vector>
#include <string.h>

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
#include "transaction_criteria.h"

using namespace std;

#ifndef __INCL_COMMAND
#define __INCL_COMMAND

class Command {
    private:
        vector<string> parameters;

        vector<string> commandHistory;
        DBAccount selectedAccount;

        Logger & log = Logger::getInstance();
        cfgmgr & cfg = cfgmgr::getInstance();

        string parse(const string & commandLine);

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

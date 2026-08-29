#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <string.h>
#include <exception>

#include <httpserver.hpp>

#include "logger.h"
#include "cfgmgr.h"
#include "db.h"
#include "db_base.h"
#include "db_account.h"
#include "db_primary_account.h"
#include "db_config.h"
#include "db_category.h"
#include "db_payee.h"
#include "db_v_transaction.h"
#include "db_transaction.h"
#include "db_v_transfer_record.h"
#include "db_transfer_transaction_record.h"
#include "db_carried_over.h"
#include "db_transaction_report.h"
#include "db_shortcut.h"
#include "db_public_holiday.h"
#include "web_api.h"


#define SIMPLE_PARAM_NAME                   "param"
#define SEQUENCE_PARAM_NAME                 "sequence"

class Command {
    private:
        std::unordered_map<std::string, std::vector<std::string>> parameters;

        std::vector<std::string> commandHistory;
        DBAccount selectedAccount;
        APIListener listener;

        Logger & log = Logger::getInstance();
        cfgmgr & cfg = cfgmgr::getInstance();

        bool isStringNumeric(const std::string & s);

        std::string parse(const std::string & commandLine);
        std::string parse(const httpserver::http_request & request);
        void handleExceptions(const std::string & command, const std::string & token);
 
        bool hasParameters() {
            return (this->parameters.size() > 0);
        }

        int getNumParameters() {
            return this->parameters.size();
        }

        std::vector<std::string> getParameters(const std::string & key) const {
            if (parameters.size() == 0) {
                throw pfm_error("Expected parameters but none were supplied");
            }

            try {
                std::vector<std::string> values = parameters.at(key);
                return values;
            }
            catch (std::out_of_range & e) {
                return {};
            }
        }

        std::string getParameter(const std::string & key) const {
            if (parameters.size() == 0) {
                throw pfm_error("Expected parameters but none were supplied");
            }

            std::vector<std::string> values = getParameters(key);
            return values.empty() ? "" : values[0];
        }

        static std::string trim(const std::string & s) {
            const char * whitespace = " \t\n\r\f\v";

            const auto start = s.find_first_not_of(whitespace);

            if (start == std::string::npos) {
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
        DBAccount selectAccount(const std::string & accountCode);
        void chooseAccount();
        void showAccount();
        void setPrimaryAccount();
        void updateAccount();
        void deleteAccount();
        void importAccounts();
        void exportAccounts();

        void addConfig();
        void listConfigItems();
        DBConfig getConfig(std::string & key);
        void updateConfig();
        void deleteConfig();

        void addHoliday();
        void listHolidays();
        DBPublicHoliday getHoliday(int sequence);
        void updateHoliday();
        void deleteHoliday();
        void importHolidays();

        void addCategory();
        void listCategories();
        DBCategory getCategory(std::string & categoryCode);
        void updateCategory();
        void deleteCategory();
        void importCategories();
        void exportCategories();
        void clearCategories();

        void addPayee();
        void listPayees();
        DBPayee getPayee(std::string & payeeCode);
        void updatePayee();
        void deletePayee();
        void importPayees();
        void exportPayees();

        void addRecurringCharge();
        void listRecurringCharges();
        void listOutstandingCharges();
        void listPaidCharges();
        DBRecurringCharge getRecurringCharge(int sequence);
        void updateRecurringCharge();
        void deleteRecurringCharge();
        void importRecurringCharges();
        void exportRecurringCharges();
        void migrateCharge();
        void clearRecurringTransactions();

        void addTransaction();
        void copyTransaction();
        void listTransactions();
        void findTransactions();
        void findTransactions(const std::string & where);
        void findTransactions(DBCriteria & criteria);
        void transactionsByCategory();
        void transactionsByPayee();
        std::string buildFindTransactionCriteria();
        DBTransaction getTransaction(int sequence);
        void updateTransaction();
        void deleteTransaction();
        void reconcileTransaction();
        void showTransaction();
        bool matchExistingTransactions(DBTransactionView & matchTransaction);
        void importTransactions();
        void exportTransactions();
        void exportTransactionsAsCSV();

        void addTransferTransaction();
        void listTransferRecords();
        DBTransferTransactionRecord getTransferRecord(int sequence);
        void deleteTransferRecord();

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

        void backup();
        void restore();
        
        void listCarriedOverLogs();

        void changePassword();
        void getDBKey();
        void saveDBKey();

        void startAPIServer();

        void enterSQLMode();
        void enterCalcMode();

        int getLogLevelParameter(std::string & level);
        void setLoggingLevel();
        void clearLoggingLevel();

        static void help();
        static void version();

        bool process(const std::string & commandLine);
        bool process(const httpserver::http_request & request);
};


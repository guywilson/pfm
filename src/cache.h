#include <string>
#include <unordered_map>
#include <stdint.h>

#include <sqlcipher/sqlite3.h>

#include "pfm_error.h"
#include "db_account.h"
#include "db_category.h"
#include "db_payee.h"
#include "db_recurring_charge.h"
#include "db_v_transaction.h"
#include "db_transaction.h"
#include "db_transaction_report.h"
#include "db_shortcut.h"

using namespace std;

#ifndef __INCL_CACHE
#define __INCL_CACHE

class CacheMgr {
    public:
        static CacheMgr & getInstance() {
            static CacheMgr instance;
            return instance;
        }

    private:
        CacheMgr() {}

        unordered_map<int, DBRecurringCharge> recurringChargeBySequence;
        unordered_map<int, DBTransaction> transactionBySequence;
        unordered_map<int, DBTransactionReport> reportBySequence;
        unordered_map<int, DBShortcut> shortcutBySequence;

        string findCriteria;

    public:
        ~CacheMgr() {}

        void clearRecurringCharges() {
            recurringChargeBySequence.clear();
        }

        void clearTransactions() {
            transactionBySequence.clear();
        }

        void clearReports() {
            reportBySequence.clear();
        }

        void clearShortcuts() {
            shortcutBySequence.clear();
        }

        void clearFindCriteria() {
            findCriteria.clear();
        }

        void addRecurringCharge(int sequence, DBRecurringCharge & charge) {
            recurringChargeBySequence.insert({sequence, charge});
        }

        void addTransaction(int sequence, DBTransaction & transaction) {
            transactionBySequence.insert({sequence, transaction});
        }

        void addReport(int sequence, DBTransactionReport & report) {
            reportBySequence.insert({sequence, report});
        }

        void addShortcut(int sequence, DBShortcut & shortcut) {
            shortcutBySequence.insert({sequence, shortcut});
        }

        DBRecurringCharge getRecurringCharge(int sequence) {
            unordered_map<int, DBRecurringCharge>::const_iterator item = recurringChargeBySequence.find(sequence);

            if (item != recurringChargeBySequence.end()) {
                return item->second;
            }

            throw pfm_error("DBRecurringCharge not found in cache.");
        }

        DBTransaction getTransaction(int sequence) {
            unordered_map<int, DBTransaction>::const_iterator item = transactionBySequence.find(sequence);

            if (item != transactionBySequence.end()) {
                return item->second;
            }

            throw pfm_error("DBTransaction not found in cache.");
        }

        DBTransactionReport getReport(int sequence) {
            unordered_map<int, DBTransactionReport>::const_iterator item = reportBySequence.find(sequence);

            if (item != reportBySequence.end()) {
                return item->second;
            }

            throw pfm_error("DBTransactionReport not found in cache.");
        }

        DBShortcut getShortcut(int sequence) {
            unordered_map<int, DBShortcut>::const_iterator item = shortcutBySequence.find(sequence);

            if (item != shortcutBySequence.end()) {
                return item->second;
            }

            throw pfm_error("DBShortcut not found in cache.");
        }

        void addFindCriteria(const string & criteria) {
            findCriteria = criteria;
        }

        string getFindCriteria() {

            if (findCriteria.length() > 0) {
                return findCriteria;
            }

            throw pfm_error("No find criteria found in cache.");
        }
};

#endif

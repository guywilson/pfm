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
#include "db_budget.h"

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
        unordered_map<int, DBTransactionView> transactionBySequence;
        unordered_map<int, DBBudget> budgetBySequence;

    public:
        ~CacheMgr() {}

        void clearRecurringCharges() {
            recurringChargeBySequence.clear();
        }

        void clearTransactions() {
            transactionBySequence.clear();
        }

        void clearBudgets() {
            budgetBySequence.clear();
        }

        void addRecurringCharge(int sequence, DBRecurringCharge & charge) {
            recurringChargeBySequence.insert({sequence, charge});
        }

        void addTransaction(int sequence, DBTransactionView & transaction) {
            transactionBySequence.insert({sequence, transaction});
        }

        void addBudget(int sequence, DBBudget & budget) {
            budgetBySequence.insert({sequence, budget});
        }

        DBRecurringCharge getRecurringCharge(int sequence) {
            unordered_map<int, DBRecurringCharge>::const_iterator item = recurringChargeBySequence.find(sequence);

            if (item != recurringChargeBySequence.end()) {
                return item->second;
            }

            throw pfm_error("DBRecurringCharge not found in cache.");
        }

        DBTransactionView getTransaction(int sequence) {
            unordered_map<int, DBTransactionView>::const_iterator item = transactionBySequence.find(sequence);

            if (item != transactionBySequence.end()) {
                return item->second;
            }

            throw pfm_error("DBTransaction not found in cache.");
        }

        DBBudget getBudget(int sequence) {
            unordered_map<int, DBBudget>::const_iterator item = budgetBySequence.find(sequence);

            if (item != budgetBySequence.end()) {
                return item->second;
            }

            throw pfm_error("DBBudget not found in cache.");
        }
};

#endif

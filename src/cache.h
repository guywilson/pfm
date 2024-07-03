#include <string>
#include <unordered_map>
#include <stdint.h>

#include <sqlite3.h>

#include "pfm_error.h"
#include "account.h"
#include "category.h"
#include "payee.h"
#include "recurring_charge.h"

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

        unordered_map<int, RecurringCharge> recurringChargeBySequence;

    public:
        ~CacheMgr() {}

        void addRecurringCharge(int sequence, RecurringCharge & charge) {
            recurringChargeBySequence.insert({sequence, charge});
        }

        RecurringCharge getRecurringCharge(int sequence) {
            unordered_map<int, RecurringCharge>::const_iterator item = recurringChargeBySequence.find(sequence);

            if (item != recurringChargeBySequence.end()) {
                return item->second;
            }

            throw pfm_error("RecurringCharge not found in cache.");
        }
};

#endif

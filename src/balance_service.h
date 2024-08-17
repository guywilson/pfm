#include <iostream>
#include <string>
#include <string.h>

#include <sqlite3.h>

#include "db_account.h"
#include "db_transaction.h"
#include "db_recurring_charge.h"

using namespace std;

#ifndef __INCL_BALANCE_SERVICE
#define __INCL_BALANCE_SERVICE

class BalanceService {
    public:
        static BalanceService & getInstance() {
            static BalanceService service;
            return service;
        }

    private:
        BalanceService() {}

    public:
        ~BalanceService();

        void createRecurringTransactions(DBAccount & account) {
            DBRecurringCharge ch;
            DBRecurringChargeResult chargeResult = ch.retrieveByAccountID(account.id);

            for (int i = 0;i < chargeResult.getNumRows();i++) {
                DBRecurringCharge charge = chargeResult.getResultAt(i);

                DBTransaction transaction;
                int hasFoundTransaction = transaction.findLatestByRecurringChargeID(charge.id);

            }
        }
};

#endif

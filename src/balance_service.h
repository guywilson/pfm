#include <iostream>
#include <string>
#include <string.h>

#include <sqlite3.h>

#include "strdate.h"
#include "money.h"
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

        /*
        ** Balance after Bills
        **
        **      Return true if this charge is due this month
        **
        ** Create Outstanding Transactions
        **
        **      From the latest transaction for the charge, if the next charge is
        **      prior to today, create it
        */

    public:
        ~BalanceService();

        Money calculateBalanceAfterBills(DBAccount & account) {
            
        }

        void createRecurringTransactions(DBAccount & account) {
            StrDate dateToday;

            DBRecurringCharge ch;
            DBRecurringChargeResult chargeResult = ch.retrieveByAccountID(account.id);

            for (int i = 0;i < chargeResult.getNumRows();i++) {
                DBRecurringCharge charge = chargeResult.getResultAt(i);

                DBTransaction transaction;
                transaction.date = charge.date;

                while (transaction.date <= dateToday) {
                    int hasFoundTransaction = transaction.findLatestByRecurringChargeID(charge.id);

                    if (hasFoundTransaction) {
                        charge.createNextTransactionForCharge(transaction.date);
                    }
                }
            }
        }
};

#endif

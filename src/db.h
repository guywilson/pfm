#include <string>
#include <stdint.h>

#include <sqlite3.h>

#include "account.h"
#include "category.h"
#include "payee.h"
#include "recurring_charge.h"

using namespace std;

#ifndef __INCL_DB
#define __INCL_DB

class PFM_DB {
    public:
        static PFM_DB & getInstance() {
            static PFM_DB instance;
            return instance;
        }

    private:
        PFM_DB() {}

        sqlite3 *       dbHandle;

        void            createSchema();

    public:
        ~PFM_DB();

        bool            open(string dbName);

        sqlite3_int64   createAccount(Account & account);
        int             getAccounts(AccountResult * result);
        int             getAccount(string code, AccountResult * result);
        int             updateAccount(Account & account);
        int             deleteAccount(Account & account);

        sqlite3_int64   createCategory(Category & category);
        int             getCategories(CategoryResult * result);
        int             getCategory(sqlite3_int64 id, CategoryResult * result);
        int             getCategory(string code, CategoryResult * result);
        int             updateCategory(Category & category);
        int             deleteCategory(Category & category);

        sqlite3_int64   createPayee(Payee & payee);
        int             getPayees(PayeeResult * result);
        int             getPayee(sqlite3_int64 id, PayeeResult * result);
        int             getPayee(string code, PayeeResult * result);
        int             updatePayee(Payee & payee);
        int             deletePayee(Payee & payee);

        sqlite3_int64   createRecurringCharge(RecurringCharge & charge);
        int             getRecurringChargesForAccount(sqlite3_int64 accountId, RecurringChargeResult * result);
        int             getRecurringCharge(sqlite3_int64 id, RecurringChargeResult * result);
        int             updateRecurringCharge(RecurringCharge & charge);
        int             deleteRecurringCharge(RecurringCharge & charge);
};

#endif

#include <string>
#include <stdint.h>

#include <sqlite3.h>

#include "db_criteria.h"
#include "db_config.h"
#include "db_currency.h"
#include "db_account.h"
#include "db_category.h"
#include "db_payee.h"
#include "db_transaction.h"
#include "db_recurring_charge.h"

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
        sqlite3_int64   createCurrency(DBCurrency & currency);
        sqlite3_int64   createConfig(DBConfig & config);

    public:
        ~PFM_DB();

        bool            open(string dbName);

        sqlite3_int64   createAccount(DBAccount & account);
        int             getAccounts(DBAccountResult * result);
        int             getAccount(string code, DBAccountResult * result);
        int             updateAccount(DBAccount & account);
        int             deleteAccount(DBAccount & account);

        sqlite3_int64   createCategory(DBCategory & category);
        int             getCategories(DBCategoryResult * result);
        int             getCategory(sqlite3_int64 id, DBCategoryResult * result);
        int             getCategory(string code, DBCategoryResult * result);
        int             updateCategory(DBCategory & category);
        int             deleteCategory(DBCategory & category);
        void            translateCategoryCriteria(DBCriteria * c);

        sqlite3_int64   createPayee(DBPayee & payee);
        int             getPayees(DBPayeeResult * result);
        int             getPayee(sqlite3_int64 id, DBPayeeResult * result);
        int             getPayee(string code, DBPayeeResult * result);
        int             updatePayee(DBPayee & payee);
        int             deletePayee(DBPayee & payee);
        void            translatePayeeCriteria(DBCriteria * c);

        sqlite3_int64   createRecurringCharge(DBRecurringCharge & charge);
        int             getRecurringChargesForAccount(sqlite3_int64 accountId, DBRecurringChargeResult * result);
        int             getRecurringCharge(sqlite3_int64 id, DBRecurringChargeResult * result);
        int             updateRecurringCharge(DBRecurringCharge & charge);
        int             deleteRecurringCharge(DBRecurringCharge & charge);

        sqlite3_int64   createTransaction(DBTransaction & transaction);
        int             getTransactionsForAccount(sqlite3_int64 accountId, DBTransactionResult * result);
        int             findTransactionsForAccount(sqlite3_int64 accountId, DBCriteria * criteria, int numCriteria, DBTransactionResult * result);
        int             getTransaction(sqlite3_int64 id, DBTransactionResult * result);
        int             updateTransaction(DBTransaction & transaction);
        int             deleteTransaction(DBTransaction & transaction);
};

#endif

#include <string>
#include <stdint.h>

#include <sqlite3.h>

#include "account.h"
#include "category.h"

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
        int             getCategory(string code, CategoryResult * result);
        int             updateCategory(Category & category);
        int             deleteCategory(Category & category);
};

#endif

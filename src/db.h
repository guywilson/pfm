#include <string>
#include <stdint.h>

#include <sqlite3.h>

#include "account.h"
#include "category.h"

using namespace std;

#ifndef __INCL_DB
#define __INCL_DB

class AccountDB {
    public:
        static AccountDB & getInstance() {
            static AccountDB instance;
            return instance;
        }

    private:
        AccountDB() {}

        sqlite3 *       dbHandle;

        void            createSchema();

    public:
        ~AccountDB();

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

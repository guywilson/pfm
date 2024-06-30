#ifndef __INCL_SCHEMA
#define __INCL_SCHEMA

/*
** You must get this right, there is no way of the
** code that uses the array of categories, just how
** many there are, unless you tell it...
*/
#define NUM_DEFAULT_CATEGORIES                       27

const char * defaultCategories[][2] = {
    {"INCME", "Income"},
    {"UTILS", "Utility bills"},
    {"GROCS", "Food & groceries"},
    {"RENT", "Rent payments"},
    {"MTGE", "Mortgage payments"},
    {"FUEL", "Vehicle fuel"},
    {"CARD", "Credit card payments"},
    {"LOAN", "Loan repayments"},
    {"BUSE", "Business expenses"},
    {"PETS", "Pets food and supplies"},
    {"HOME", "Home and DIY"},
    {"FOOD", "Eating out & take-away"},
    {"CASH", "ATM withdrawal"},
    {"CARM", "Car maintenance"},
    {"GIFT", "Gifts & presents"},
    {"INTE", "Interest & bank charges"},
    {"HHOLD", "Household expenses"},
    {"HEALT", "Health expenses"},
    {"INSUR", "Insurance payments"},
    {"LEISR", "Leisure and fun"},
    {"LUNCH", "Lunch at work"},
    {"DRINK", "Drinking & going out"},
    {"ENTMT", "Books, music and cinema"},
    {"CLOTH", "Shoes & clothing"},
    {"EDUCN", "Education costs"},
    {"TRAVL", "Travel expenses"},
    {"CHARY", "Charities & giving"}
};

const char * pszCreateAccountTable = 
    "CREATE TABLE account (" \
    "id INTEGER PRIMARY KEY," \
    "name TEXT NOT NULL," \
    "code TEXT NOT NULL," \
    "opening_balance NUMERIC NOT NULL," \
    "current_balance NUMERIC NOT NULL," \
    "UNIQUE(code) ON CONFLICT ROLLBACK" \
    ");";

const char * pszCreateCategoryTable = 
    "CREATE TABLE category (" \
    "id INTEGER PRIMARY KEY," \
    "code TEXT NOT NULL," \
    "description TEXT NOT NULL" \
    ");";

const char * pszCreatePayeeTable = 
    "CREATE TABLE payee (" \
    "id INTEGER PRIMARY KEY," \
    "code TEXT NOT NULL," \
    "name TEXT NOT NULL" \
    ");";

const char * pszCreateRCTable = 
    "CREATE TABLE recurring_charge (" \
    "id INTEGER PRIMARY KEY," \
    "account_id INTEGER," \
    "category_id INTEGER," \
    "payee_id INTEGER," \
    "date TEXT," \
    "description TEXT NOT NULL," \
    "amount NUMERIC NOT NULL," \
    "frequency TEXT NOT NULL," \
    "FOREIGN KEY(account_id) REFERENCES account(id)," \
    "FOREIGN KEY(category_id) REFERENCES category(id)," \
    "FOREIGN KEY(payee_id) REFERENCES payee(id)" \
    ");";

static const char * pszCreateTransationTable = 
    "CREATE TABLE account_transaction (" \
    "id INTEGER PRIMARY KEY," \
    "account_id INTEGER," \
    "category_id INTEGER," \
    "payee_id INTEGER," \
    "date TEXT NOT NULL," \
    "description TEXT NOT NULL," \
    "credit_debit TEXT NOT NULL," \
    "amount NUMERIC NOT NULL," \
    "FOREIGN KEY(account_id) REFERENCES account(id)," \
    "FOREIGN KEY(category_id) REFERENCES category(id)," \
    "FOREIGN KEY(payee_id) REFERENCES payee(id)" \
    ");";

#endif

#ifndef __INCL_SCHEMA
#define __INCL_SCHEMA

/*
** You must get this right, there is no way for the code
** that uses the arrays to know just how many there are, 
** unless you tell it...
*/
#define NUM_DEFAULT_CATEGORIES                       27
#define NUM_CURRENCIES                              168
#define NUM_CONFIG_ITEMS                              4

/*
** Each row specifies: code, description
*/
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

/*
** Each row specifies: key, value, description, is_read_only
*/
const char * defaultConfig[][5] = {
    {"cycle.start", "1", "The day each monthly cycle starts on", "Y", "Y"},
    {"cycle.end", "last-working-day", "The day each monthly cycle ends on", "N", "Y"},
    {"limits.row", "50", "The row limit when retrieving records", "N", "Y"},
    {"money.locale", "", "Locale used for money fields", "N", "Y"}
};

/*
** Each row specifies: code, name, symbol
**
*/
const char * currencies[][3] = {
    {"AFN", "Afghani", ""},
    {"ALL", "Lek", ""},
    {"DZD", "Algerian Dinar", ""},
    {"USD", "US Dollar", "$"},
    {"EUR", "Euro", "€"},
    {"AOA", "Kwanza", ""},
    {"XCD", "East Caribbean Dollar", "$"},
    {"ARS", "Argentine Peso", ""},
    {"AMD", "Armenian Dram", ""},
    {"AWG", "Aruban Florin", ""},
    {"AUD", "Australian Dollar", "$"},
    {"AZN", "Azerbaijanian Manat", ""},
    {"BSD", "Bahamian Dollar", "$"},
    {"BHD", "Bahraini Dinar", ""},
    {"BDT", "Taka", ""},
    {"BBD", "Barbados Dollar", "$"},
    {"BYN", "Belarussian Ruble", ""},
    {"BZD", "Belize Dollar", "$"},
    {"XOF", "CFA Franc BCEAO", ""},
    {"BMD", "Bermudian Dollar", "$"},
    {"BTN", "Ngultrum", ""},
    {"INR", "Indian Rupee", ""},
    {"BOB", "Boliviano", ""},
    {"BOV", "Mvdol", ""},
    {"BAM", "Convertible Mark", ""},
    {"BWP", "Pula", ""},
    {"NOK", "Norwegian Krone", "Kr"},
    {"BRL", "Brazilian Real", ""},
    {"BND", "Brunei Dollar", "$"},
    {"BGN", "Bulgarian Lev", ""},
    {"BIF", "Burundi Franc", ""},
    {"CVE", "Cabo Verde Escudo", ""},
    {"KHR", "Riel", ""},
    {"XAF", "CFA Franc BEAC", ""},
    {"CAD", "Canadian Dollar", "$"},
    {"KYD", "Cayman Islands Dollar", "$"},
    {"CLF", "Unidad de Fomento", ""},
    {"CLP", "Chilean Peso", ""},
    {"CNY", "Yuan Renminbi", ""},
    {"COP", "Colombian Peso", ""},
    {"COU", "Unidad de Valor Real", ""},
    {"KMF", "Comoro Franc", ""},
    {"CDF", "Congolese Franc", ""},
    {"NZD", "New Zealand Dollar", "$"},
    {"CRC", "Costa Rican Colon", ""},
    {"CUC", "Peso Convertible", ""},
    {"CUP", "Cuban Peso", ""},
    {"ANG", "Netherlands Antillean Guilder", ""},
    {"CZK", "Czech Koruna", ""},
    {"DKK", "Danish Krone", "Kr"},
    {"DJF", "Djibouti Franc", ""},
    {"DOP", "Dominican Peso", ""},
    {"EGP", "Egyptian Pound", "£"},
    {"SVC", "El Salvador Colon", ""},
    {"ERN", "Nakfa", ""},
    {"ETB", "Ethiopian Birr", ""},
    {"FKP", "Falkland Islands Pound", "£"},
    {"FJD", "Fiji Dollar", "$"},
    {"XPF", "CFP Franc", ""},
    {"GMD", "Dalasi", ""},
    {"GEL", "Lari", ""},
    {"GHS", "Ghana Cedi", ""},
    {"GIP", "Gibraltar Pound", "£"},
    {"GTQ", "Quetzal", ""},
    {"GBP", "Pound Sterling", "£"},
    {"GNF", "Guinea Franc", ""},
    {"GYD", "Guyana Dollar", ""},
    {"HTG", "Gourde", ""},
    {"HNL", "Lempira", ""},
    {"HKD", "Hong Kong Dollar", "$"},
    {"HUF", "Forint", ""},
    {"ISK", "Iceland Krona", "Kr"},
    {"IDR", "Rupiah", ""},
    {"XDR", "SDR (Special Drawing Right)", ""},
    {"IRR", "Iranian Rial", ""},
    {"IQD", "Iraqi Dinar", ""},
    {"ILS", "New Israeli Sheqel", ""},
    {"JMD", "Jamaican Dollar", "$"},
    {"JPY", "Yen", ""},
    {"JOD", "Jordanian Dinar", ""},
    {"KZT", "Tenge", ""},
    {"KES", "Kenyan Shilling", ""},
    {"KPW", "North Korean Won", ""},
    {"KRW", "Won", ""},
    {"KWD", "Kuwaiti Dinar", ""},
    {"KGS", "Som", ""},
    {"LAK", "Kip", ""},
    {"LBP", "Lebanese Pound", "£"},
    {"LSL", "Loti", ""},
    {"ZAR", "Rand", ""},
    {"LRD", "Liberian Dollar", "$"},
    {"LYD", "Libyan Dinar", ""},
    {"CHF", "Swiss Franc", "Fr"},
    {"MOP", "Pataca", ""},
    {"MGA", "Malagasy Ariary", ""},
    {"MWK", "Kwacha", ""},
    {"MYR", "Malaysian Ringgit", ""},
    {"MVR", "Rufiyaa", ""},
    {"MRU", "Ouguiya", ""},
    {"MUR", "Mauritius Rupee", ""},
    {"XUA", "ADB Unit of Account", ""},
    {"MXN", "Mexican Peso", ""},
    {"MXV", "Mexican Unidad de Inversion (UDI)", ""},
    {"MDL", "Moldovan Leu", ""},
    {"MNT", "Tugrik", ""},
    {"MAD", "Moroccan Dirham", ""},
    {"MZN", "Mozambique Metical", ""},
    {"MMK", "Kyat", ""},
    {"NAD", "Namibia Dollar", "$"},
    {"NPR", "Nepalese Rupee", ""},
    {"NIO", "Cordoba Oro", ""},
    {"NGN", "Naira", ""},
    {"OMR", "Rial Omani", ""},
    {"PKR", "Pakistan Rupee", ""},
    {"PAB", "Balboa", ""},
    {"PGK", "Kina", ""},
    {"PYG", "Guarani", ""},
    {"PEN", "Nuevo Sol", ""},
    {"PHP", "Philippine Peso", ""},
    {"PLN", "Zloty", ""},
    {"QAR", "Qatari Rial", ""},
    {"MKD", "Denar", ""},
    {"RON", "Romanian Leu", ""},
    {"RUB", "Russian Ruble", ""},
    {"RWF", "Rwanda Franc", ""},
    {"SHP", "Saint Helena Pound", "£"},
    {"WST", "Tala", ""},
    {"STN", "Dobra", ""},
    {"SAR", "Saudi Riyal", ""},
    {"RSD", "Serbian Dinar", ""},
    {"SCR", "Seychelles Rupee", ""},
    {"SLE", "Leone", ""},
    {"SGD", "Singapore Dollar", "$"},
    {"XSU", "Sucre", ""},
    {"SBD", "Solomon Islands Dollar", "$"},
    {"SOS", "Somali Shilling", ""},
    {"SSP", "South Sudanese Pound", ""},
    {"LKR", "Sri Lanka Rupee", ""},
    {"SDG", "Sudanese Pound", ""},
    {"SRD", "Surinam Dollar", ""},
    {"SZL", "Lilangeni", ""},
    {"SEK", "Swedish Krona", "Kr"},
    {"CHE", "WIR Euro", ""},
    {"CHW", "WIR Franc", ""},
    {"SYP", "Syrian Pound", ""},
    {"TWD", "New Taiwan Dollar", "$"},
    {"TJS", "Somoni", ""},
    {"TZS", "Tanzanian Shilling", ""},
    {"THB", "Baht", ""},
    {"TOP", "Pa’anga", ""},
    {"TTD", "Trinidad and Tobago Dollar", "$"},
    {"TND", "Tunisian Dinar", ""},
    {"TRY", "Turkish Lira", ""},
    {"TMT", "Turkmenistan New Manat", ""},
    {"UGX", "Uganda Shilling", ""},
    {"UAH", "Hryvnia", ""},
    {"AED", "UAE Dirham", ""},
    {"USN", "US Dollar (Next day)", ""},
    {"UYI", "Uruguay Peso en Unidades Indexadas (URUIURUI)", ""},
    {"UYU", "Peso Uruguayo", ""},
    {"UZS", "Uzbekistan Sum", ""},
    {"VUV", "Vatu", ""},
    {"VEF", "Bolivar", ""},
    {"VED", "Bolivar", ""},
    {"VND", "Dong", ""},
    {"YER", "Yemeni Rial", ""},
    {"ZMW", "Zambian Kwacha", ""},
    {"ZWL", "Zimbabwe Dollar", "$"}
};

const char * pszCreateConfigTable =
    "CREATE TABLE config (" \
    "id INTEGER PRIMARY KEY," \
    "key TEXT NOT NULL," \
    "value TEXT NOT NULL," \
    "description TEXT NULL," \
    "is_read_only TEXT NOT NULL DEFAULT 'N'," \
    "is_visible TEXT NOT NULL DEFAULT 'Y'," \
    "created TEXT NOT NULL," \
    "updated TEXT NOT NULL," \
    "UNIQUE(key) ON CONFLICT ROLLBACK);";

const char * pszCreateCurrencyTable =
    "CREATE TABLE currency (" \
    "id INTEGER PRIMARY KEY," \
    "code TEXT NOT NULL," \
    "name TEXT NOT NULL," \
    "symbol TEXT NULL," \
    "created TEXT NOT NULL," \
    "updated TEXT NOT NULL," \
    "UNIQUE(code) ON CONFLICT ROLLBACK);";

const char * pszCreateAccountTable = 
    "CREATE TABLE account (" \
    "id INTEGER PRIMARY KEY," \
    "name TEXT NOT NULL," \
    "code TEXT NOT NULL," \
    "opening_date TEXT NOT NULL," \
    "opening_balance NUMERIC NOT NULL," \
    "balance_limit NUMERIC NOT NULL DEFAULT 0.0," \
    "currency_id INTEGER,"
    "created TEXT NOT NULL," \
    "updated TEXT NOT NULL," \
    "UNIQUE(code) ON CONFLICT ROLLBACK," \
    "FOREIGN KEY(currency_id) REFERENCES currency(id));";

const char * pszCreatePrimaryAccountTable = 
    "CREATE TABLE primary_account (" \
    "id INTEGER PRIMARY KEY," \
    "account_code TEXT NOT NULL," \
    "created TEXT NOT NULL," \
    "updated TEXT NOT NULL," \
    "FOREIGN KEY(account_code) REFERENCES account(code));";

const char * pszCreateCategoryTable = 
    "CREATE TABLE category (" \
    "id INTEGER PRIMARY KEY," \
    "code TEXT NOT NULL," \
    "description TEXT NOT NULL," \
    "created TEXT NOT NULL," \
    "updated TEXT NOT NULL," \
    "UNIQUE(code) ON CONFLICT ROLLBACK);";

const char * pszCreatePayeeTable = 
    "CREATE TABLE payee (" \
    "id INTEGER PRIMARY KEY," \
    "code TEXT NOT NULL," \
    "name TEXT NULL," \
    "created TEXT NOT NULL," \
    "updated TEXT NOT NULL," \
    "UNIQUE(code) ON CONFLICT ROLLBACK);";

const char * pszCreateRCTable = 
    "CREATE TABLE recurring_charge (" \
    "id INTEGER PRIMARY KEY," \
    "account_id INTEGER," \
    "category_id INTEGER," \
    "payee_id INTEGER," \
    "date TEXT," \
    "end_date TEXT," \
    "description TEXT NOT NULL," \
    "amount NUMERIC NOT NULL," \
    "last_payment_date TEXT" \
    "frequency TEXT NOT NULL," \
    "created TEXT NOT NULL," \
    "updated TEXT NOT NULL," \
    "FOREIGN KEY(account_id) REFERENCES account(id)," \
    "FOREIGN KEY(category_id) REFERENCES category(id)," \
    "FOREIGN KEY(payee_id) REFERENCES payee(id));";

const char * pszCreateRecurringTransferTable = 
    "CREATE TABLE recurring_transfer (" \
    "id INTEGER PRIMARY KEY," \
    "recurring_charge_id INTEGER," \
    "account_to_id INTEGER," \
    "created TEXT NOT NULL," \
    "updated TEXT NOT NULL," \
    "FOREIGN KEY(account_to_id) REFERENCES account(id)," \
    "FOREIGN KEY(recurring_charge_id) REFERENCES category(id));";

static const char * pszCreateTransactionTable = 
    "CREATE TABLE account_transaction (" \
    "id INTEGER PRIMARY KEY," \
    "account_id INTEGER," \
    "category_id INTEGER," \
    "payee_id INTEGER," \
    "recurring_charge_id INTEGER,"
    "date TEXT NOT NULL," \
    "reference TEXT," \
    "description TEXT NOT NULL," \
    "credit_debit TEXT NOT NULL," \
    "amount NUMERIC NOT NULL," \
    "is_reconciled TEXT NOT NULL," \
    "created TEXT NOT NULL," \
    "updated TEXT NOT NULL," \
    "FOREIGN KEY(account_id) REFERENCES account(id)," \
    "FOREIGN KEY(category_id) REFERENCES category(id)," \
    "FOREIGN KEY(payee_id) REFERENCES payee(id)," \
    "FOREIGN KEY(recurring_charge_id) REFERENCES recurring_charge(id));";

static const char * pszCreateTransferTransactionRecordTable =
    "CREATE TABLE transfer_transaction_record (" \
    "id INTEGER PRIMARY KEY," \
    "transaction_to_id INTEGER NOT NULL," \
    "transaction_from_id INTEGER NOT NULL," \
    "transaction_date TEXT NOT NULL," \
    "created TEXT NOT NULL," \
    "updated TEXT NOT NULL," \
    "FOREIGN KEY(transaction_to_id) REFERENCES account_transaction(id)," \
    "FOREIGN KEY(transaction_from_id) REFERENCES account_transaction(id));";

static const char * pszCreateCarriedOverTable = 
    "CREATE TABLE carried_over_log (" \
    "id INTEGER PRIMARY KEY," \
    "account_id INTEGER," \
    "date TEXT NOT NULL," \
    "description TEXT NOT NULL," \
    "balance NUMERIC NOT NULL," \
    "created TEXT NOT NULL," \
    "updated TEXT NOT NULL," \
    "FOREIGN KEY(account_id) REFERENCES account(id));";

static const char * pszCreateReportTable = 
    "CREATE TABLE transaction_report (" \
    "id INTEGER PRIMARY KEY," \
    "description TEXT NOT NULL," \
    "sql_where_clause TEXT NOT NULL," \
    "created TEXT NOT NULL," \
    "updated TEXT NOT NULL);";

static const char * pszCreateShortcutTable = 
    "CREATE TABLE shortcut (" \
    "id INTEGER PRIMARY KEY," \
    "shortcut_text TEXT NOT NULL," \
    "replacement_text TEXT NOT NULL," \
    "created TEXT NOT NULL," \
    "updated TEXT NOT NULL);";

/*
** Create views, do this last...
*/
const char * pszCreateCOView = 
    "CREATE VIEW v_carried_over_log AS SELECT " \
    "co.id," \
    "co.account_id," \
    "a.code AS account_code," \
    "co.date," \
    "co. description," \
    "co.balance," \
    "co.created," \
    "co.updated " \
    "FROM carried_over_log co " \
    "LEFT JOIN account a ON a.id = co.account_id " \
    "ORDER BY co.date DESC;";

const char * pszCreateListRCView = 
    "CREATE VIEW v_recurring_charge_list AS SELECT " \
    "rc.id," \
    "rc.account_id," \
    "rc.date," \
    "rc.end_date," \
    "rc.description," \
    "rc.payee_id," \
    "p.code AS payee_code," \
    "rc.category_id," \
    "c.code AS category_code," \
    "rc.amount," \
    "rc.last_payment_date," \
    "rc.frequency," \
    "rc.created," \
    "rc.updated " \
    "FROM recurring_charge rc " \
    "LEFT JOIN payee p ON p.id = rc.payee_id " \
    "LEFT JOIN category c ON c.id = rc.category_id " \
    "ORDER BY rc.date ASC;";

static const char * pszCreateListTransationView = 
    "CREATE VIEW v_transaction_list AS SELECT " \
    "tr.id," \
    "tr.account_id," \
    "a.code AS account," \
    "tr.recurring_charge_id," \
    "CASE " \
    "    WHEN tr.recurring_charge_id IS NOT NULL THEN 'Y' " \
    "    ELSE 'N' " \
    "END AS recurring," \
    "tr.date," \
    "tr.reference," \
    "tr.description," \
    "tr.payee_id," \
    "p.code AS payee," \
    "tr.category_id," \
    "c.code AS category," \
    "tr.credit_debit AS type," \
    "tr.amount," \
    "tr.is_reconciled AS reconciled," \
    "tr.created," \
    "tr.updated " \
    "FROM account_transaction tr " \
    "LEFT JOIN payee p ON p.id = tr.payee_id " \
    "LEFT JOIN category c ON c.id = tr.category_id " \
    "LEFT JOIN account a ON a.id = tr.account_id " \
    "ORDER BY tr.date ASC;";

static const char * pszCreateRecurringTransactionsView =
    "CREATE VIEW v_recurring_transactions AS " \
    "SELECT tr.id," \
    "tr.account_id," \
    "a.code AS account_code," \
    "tr.recurring_charge_id," \
    "tr.date," \
    "tr.reference," \
    "tr.description," \
    "tr.payee_id," \
    "p.code AS payee_code," \
    "tr.category_id," \
    "c.code AS category_code," \
    "rc.frequency AS frequency_code," \
    "tr.amount," \
    "tr.is_reconciled," \
    "tr.created," \
    "tr.updated " \
    "FROM account_transaction tr " \
    "LEFT JOIN payee p ON p.id = tr.payee_id " \
    "LEFT JOIN category c ON c.id = tr.category_id " \
    "LEFT JOIN account a ON a.id = tr.account_id " \
    "LEFT JOIN recurring_charge rc ON rc.id = tr.recurring_charge_id " \
    "WHERE tr.recurring_charge_id <> 0 " \
    "ORDER BY tr.description, tr.date DESC";

static const char * pszCreateTransferRecordView =
    "CREATE VIEW v_transfer_record AS " \
    "SELECT r.id," \
    "af.code AS account_from," \
    "at.code AS account_to," \
    "trf.id AS transaction_from_id," \
    "trt.id AS transaction_to_id," \
    "trf.date," \
    "trf.description," \
    "p.code AS payee_code," \
    "c.code AS category_code," \
    "trf.amount," \
    "trf.is_reconciled," \
    "trf.created," \
    "trf.updated " \
    "FROM transfer_transaction_record r " \
    "LEFT JOIN account_transaction trf ON trf.id = r.transaction_from_id " \
    "LEFT JOIN account_transaction trt ON trt.id = r.transaction_to_id " \
    "LEFT JOIN payee p ON p.id = trf.payee_id " \
    "LEFT JOIN category c ON c.id = trf.category_id " \
    "LEFT JOIN account af ON af.id = trf.account_id " \
    "LEFT JOIN account at ON at.id = trt.account_id " \
    "ORDER BY trf.date ASC;";

#endif

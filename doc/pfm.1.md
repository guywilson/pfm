% PFM(1) PFM User Manual
% Guy Wilson
% July 1, 2025

# NAME

pfm - personal finance manager

# SYNOPSIS

pfm [*options*]

# DESCRIPTION

PFM is a personal finance application that uses a command-line interface that supports many commands for interacting with accounts and transactions.

PFM supports multiple accounts, transactions, recurring charges, categories and payees. The data is stored in an encrypted SQLite database file, requiring a password to open.

# OPTIONS

**-db [db file]**

Specifies the filename of the SQLite database to open. If you do not specify this option, it assumes a database file named ‘.pfm’ in the current directory.

**--version, -v**

Prints the version information of the PFM program and exits.

**-h, -?**

Prints the supported command line options and exits.

# COMMANDS

For commands that support optional arguments, if no arguments are supplied, the user is presented with interactive prompts to enter the required data. Each entity supports the standard CRUD commands to add, list, update and delete records. For entities that have a unique code, that is account, category and payee, update and delete commands take a code to reference the required record. For transaction and recurring charge entities that do not have a code, update and delete commands take a sequence number as displayed by the list command.

The full list of ‘standard’ CRUD commands are as follows, commands outside of this model are listed separately. Command parameters are delimited with the ‘|’, ‘:’ or ‘;’ character. Date fields accept the format yyyy-mm-dd or dd-mm-yyyy, they do not accept the the frankly weird US date format of mm-dd-yyyy.

**add-account, aa**
**list-accounts, la**
**update-account, ua**
**delete-account, da**

**add-category, ac**
**list-categories, lc**
**update-category, uc**
**delete-category, dc**

**add-payee, ap**
**list-payees, lp**
**update-payee, up**
**delete-payee, dp**

**add-recurring-charge, arc**
**list-recurring-charges, lrc**
**update-recurring-charge, urc**
**delete-recurring-charge, drc**

**add-transaction, at**
**list-transactions, list, lt** (*See extended forms below*)
**update-transaction, ut**
**delete-transaction, dt**

PFM also supports import and export of entities via the following commands, these all accept/output files in JSON format, other than the **export-transasctions-csv** command which will output a CSV file, useful; for analysis in a spreadsheet program for example:

**import-categories, ic**
**export-categories, xc**
**import-payees, ip**
**export-payees, xp**
**import-recurring-charges, irc**
**export-recurring-charges, xrc**
**import-transactions, it**
**export-transactions, xt**
**export-transactions-csv, xtc**

JSON import files must be in the format:

```
	{
		"className": "DBAccount",
		"accounts": [
			{
				"code": "BANK",
				"name": "Current account",
				"openingBalance": "2312.56",
				"openingData", "2025-06-15"
			}
		]
	}
```

The PFM command **save-json-template** will save an example json file for the entity you choose, making it easy to edit this in your favourite text editor. 

> [!TIP] Google Sheets has extensions available to allow export of spreadsheet data into JSON format

**list [options]**

- num - number of results to be returned
- all - return non-recurring and recurring transactions
- nr - return only non-recurring transactions (the default)
- asc - sort the results in ascending date order
- desc - sort the results in descending date order (the default)

An example list command could be:
```
list 50:all:desc
```

**find-transactions, find [options]**

Parameters are listed below, any parameters that accept wildcards recognise * as any string, _ as any character, e.g. ds:travel*. Parameters are separated by the ‘|’ character:

- d:[date] - transactions on the specified date
- D:[date]|D:[date] - transactions between these dates
- ds:[description] - transactions with this description
- acc:[account code] - transactions with this account code
- c:[category code] - transactions with this category code
- p:[payee code] - transactions with this payee code
- ds:[description] - transactions like this description, wildcards are accepted
- r:[reference] - transactions with this reference, wildcards are accepted
- rc:[r/n] - transactions that are [r]ecurring or [n]on-recurring
- db - transactions that are a debit
- cr - transactions that are a credit
- a:[amount] - transactions where the amount is less than this
- A:[amount]|A:[amount] - transactions where the amount is between these 

**use [account-code]**

Set the current account context to the account specified with account-code. The list-accounts command will show which account is the *primary* account, the initial state will set this account as the current account context until it is changed with the use command.

**save-json-template, sjt**

Presents a menu to choose which entity to create a json template for, choose from account, payee, category, recurring charge, transaction. A file with the name <entity>_template.json is created in the current directory.

**set-primary-account, spa [account code]**

Sets the primary account, like the immortals in *Highlander*, there can be only one. On pfm startup, the primary account is selected so you don’t need to issue a **use** command.

**clear-categories**

Deletes **all** stored categories, used for example if you don’t want any of the default categories created with a new file.

**add-transaction, at, add [parameters]**

For a quicker user experience when entering transactions, the following parameters can be supplied. Parameters are separated by the ‘|’ character:

- d:[date] - The transaction date in the format dd-mm-yyyy or yyyy-mm-dd
- c:[category code] - The category for the transaction, it must exist if specified. Skip if not required
- p:[payee code] - The payee for the transaction, it must exist if specified. Skip if not required
- ds:[description] - The transaction description
- r:[reference] - The transaction reference, e.g. cheque number (does anyone still use those?)
- db - Create a debit transaction
- cr - Create a credit transaction
- a:[amount] - The transaction amount

**transfer-transaction, tr, transfer**

Add a transfer transaction to another account, this will create a debit transaction from the currently selected account, and a debit transaction in the ‘transfer to’ account.

**list-transactions, lt, list [parameters]**

List the transactions of the currently selected account. Allows filtering of the transaction results with the following parameters, parameters are separated with the ‘|’ character:

- num - The number of results to return
- all - Return both recurring and non-recurring transactions
- nr - Return only non-recurring transactions (the default)
- asc - Sort the results in ascending date order
- desc - Sort the results in descending date order (the default)

An example command is:
	**list 50:all:asc** - returns the oldest 50 transactions, including recurring transactions.

**reconcile-transaction, reconcile, rt [sequence number]**

Reconcile the transaction with the supplied sequence number.

**change-password**

Change the database password.

**version**

Print the pfm version string.

**help**

Display some help information.

**exit, quit, q**

Quit the application.

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

The full list of ‘standard’ CRUD commands are as follows, commands outside of this model are listed separately. Command parameters take the form of
```
parameter_name:parameter_value
```

Date fields accept the formats yyyy-mm-dd or dd-mm-yyyy only. Text fields should be delimited with "" if they contain spaces, e.g.
```
desc:"The quick brown fox jumped over the lazy dog"
```

**add-account, aa**
**list-accounts, la**
**update-account, ua**
**delete-account, da**

**add-config-item, acfg**
**list-config-items, lcfg**
**update-config-item, ucfg**
**delete-config-item, dcfg**

**add-category, ac**
**list-categories, lc**
**update-category, uc**
**delete-category, dc**

**add-payee, ap**
**list-payees, lp**
**update-payee, up**
**delete-payee, dp**

**add-recurring-charge, arc** (*See extended form below*)
**list-recurring-charges, lrc**
**update-recurring-charge, urc**
**delete-recurring-charge, drc**

**add-transaction, at** (*See extended form below*)
**list-transactions, list, lt** (*See extended forms below*)
**update-transaction, ut**
**delete-transaction, dt**

**transfer-transaction, transfer, tr** (*See extended form below*)

**add-report, arp**
**list-reports, show-reports, lrp**
**update-report, urp**
**delete-report, drp**

Manage shortcuts to commands that you run frequently, shortcuts are activated by following with the backslash character '\' which will
replace the shortcut with the command you have specified

**add-shortcut, ash**
**list-shortcuts, lsh**
**update-shortcut, ush**
**delete-shortcut, dsh**

Run a report after using **list-reports** using the **run-report or run** command with the sequence number of the report.

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

**add-recurring-charge, arc [parameters]**

Add a new recurring charge to the currently selected account

- c:[category code] - The category code of the charge
- p:[payee code] - The payee code of the charge
- start:[date] - The start date for the charge (defaults to today)
- end:[date] - The end date for the charge (defaults to N/A)
- desc:[description] - The description
- freq:[frequency code] - The frequency of the charge - n[y|m|w|d], e.g. '1m' for monthly
- amnt:[amount] - The amount
- to:[account code] - The account code to transfer to, e.g. recurring transfer

**list-outstanding-charges, loc**

Lists the oustanding charges for the currently selected account.

**list-paid-charges, lpc**

Lists the paid charges for the currently selected account.

**add-transaction, at, add [parameters]**

Add a new transaction to the currently selected account

- c:[category code] - The category code of the transaction
- p:[payee code] - The payee code of the transaction
- date:[date] - The date for the transaction (defaults to today)
- desc:[description] - The description
- ref:[reference] - The reference
- type:[DB/CR] - Whether this is a debit or credit transaction
- amnt:[amount] - The amount
- rec:[Y/N] - Is the transaction reconciled or not

**transfer-transaction, transfer, tr [parameters]**

Add a new transfer transaction to the currently selected account

- to:[account code] - The code of the account to transfer to
- c:[category code] - The category code of the transaction
- date:[date] - The date for the transaction (defaults to today)
- desc:[description] - The description
- amnt:[amount] - The amount
- rec:[Y/N] - Is the transaction reconciled or not

**list-transactions, lt, list [parameters]**

List transactions for the currently selected account

- num - number of results to be returned
- all - return non-recurring and recurring transactions
- nr - return only non-recurring transactions (the default)
- rc - return only recurring transactions
- period - return transactions in the current period
- any - return transations from any timeframe
- asc - sort the results in ascending date order
- desc - sort the results in descending date order (the default)

An example list command could be:
```
list 50 all desc
```

**find-transactions, find [parameters]**

Find transactions for the currently selected account

Parameters are listed below, any parameters that accept wildcards recognise * as any string, ? as any character, e.g. desc:travel*.

- date:[date] - transactions on the specified date(s)
- date>:[date] - transactions after this date
- date<:[date] - transactions before this date
- desc:[description] - transactions with this description, wildcards are accepted
- acc:[account code] - transactions with this account code
- c:[category code] - transactions with this category code
- p:[payee code] - transactions with this payee code
- ref:[reference] - transactions with this reference, wildcards are accepted
- rec:[r/n] - transactions that are [r]ecurring or [n]on-recurring
- type:[DB/CR] - transactions with type either debit or credit
- amnt>:[amount] - transactions where the amount is greater than this
- amnt<:[amount] - transactions where the amount is less than this

- sql:[where clause parameters] - find transactions specified by the criteria

This option will likely be useful only to those familiar with SQL (Structured Query Language). The **find** command
queries a view with the following column names:

- account - the account code of the transaction
- recurring - whether the transaction is a recurring charge ('Y' or 'N')
- date - the date the transaction was posted for
- reference - the transaction reference
- description
- payee - the transaction payee code
- category - the transaction category code
- type - whether this transacion is a CRedit or a DeBit ('CR' or 'DB')
- amount
- reconciled - whether the transaction has been reconciled ('Y' or 'N')

An example:
```
find sql:category = 'BOOKS' AND amount > 25 AND recurring = 'N'
```

The query used in the **find** command can be saved as a report by issuing the **save-report** or **save** command.

**use [account-code]**

Set the current account context to the account specified with account-code. The list-accounts command will show which account is the *primary* account, the initial state will set this account as the current account context until it is changed with the use command.

**save-json-template, sjt**

Presents a menu to choose which entity to create a json template for, choose from account, payee, category, recurring charge, transaction. A file with the name <entity>_template.json is created in the current directory.

**set-primary-account, spa [account code]**

Sets the primary account, like the immortals in *Highlander*, there can be only one. On pfm startup, the primary account is selected so you don’t need to issue a **use** command.

**clear-categories**

Deletes **all** stored categories, used for example if you don’t want any of the default categories created with a new file.

**transfer-transaction, tr, transfer**

Add a transfer transaction to another account, this will create a debit transaction from the currently selected account, and a debit transaction in the ‘transfer to’ account.

**reconcile-transaction, reconcile, rt [sequence number]**

Reconcile the transaction with the supplied sequence number.

**change-password**

Change the database password.

**version**

Print the pfm version string.

**help**

Display some usage information.

**exit, quit, q**

Quit the application.

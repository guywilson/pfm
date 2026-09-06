% PFM(1) PFM User Manual
% Guy Wilson
% Sep 4, 2026

# NAME

pfm - personal finance manager

# SYNOPSIS

pfm [*options*]

# DESCRIPTION

PFM is a personal finance application that uses a command-line interface to interact with accounts, transactions and recurring charges.

PFM supports multiple accounts, transactions, recurring charges, categories and payees. The data is stored in an encrypted SQLite database file, requiring a password to open.

# OPTIONS

**-db [db file]**

Specifies the filename of the SQLite database to open. If you do not specify this option, it assumes a database file named ‘.pfm’ in the current directory.

**--headless**

Explicitly tells PFM that it is running in a headless (server) environment. Without this, PFM will try to work it out for itself. PFM uses this to determine if it should get the database key from secure storage (macOS keychain, GNU Key Manager or Windows Credential Manager) or from the user entering a password.

**--license, -l**

Prints the license information of the PFM program and exits.

**--version, -v**

Prints the version information of the PFM program and exits.

**-h, -?**

Prints the supported command line options and exits.

# COMMANDS

Commands are listed below with the full name followed by accepted short forms in brackets.

Some commands take parameters, these take the form of:
```
parameter_name:parameter_value
```

Some commands, particularly those that update or delete an entity take a parameter to identify the particular entity, that is typically shown when the entities are listed. Sometimes this identifier is a code (as is the case with category or payee), and sometimes it is a sequence number (as is the case with transaction or recurring charge). This parameter is shown in square brackets after the command name. These parameters do not take the form above, they are simply the command followed by the code/sequence. If the parameter is a code, it is case sensitive.

Date fields accept the formats yyyy-mm-dd , dd-mm-yyyy, yyyy-Mmm-dd or dd-Mmm-yyyy only. Text fields should be delimited with "" if they contain spaces, e.g.
```
desc:"The quick brown fox jumped over the lazy dog"
```

**add-account (aa)**

Add a new account to the system. The user is prompted to enter the account details such as the account name and opening balance.

**list-accounts (la)**

List all accounts, displaying current balance, reconciled balance and balance after bills.

**update-account (ua)** [account code]

Update the account details with the given account code.

**delete-account (da)** [account code]

Delete the account with the given account code. This will also delete all transactions and recurring charges associated with the account.

**add-config-item (acfg)**

Add a new config item. The user is prompted to enter the config item details.

**list-config-items (lcfg)**

List all config items with their values.

**update-config-item (ucfg)** [config item code]

Update the config item with the given config item code.

**delete-config-item (dcfg)** [config item code]

Delete the config item with the given code.

**add-category (ac)**

Add a new category to the system. The user is prompted to enter the category details.

**list-categories (lc)**

List all categories with their codes and descriptions.

**update-category (uc)** [category code]

Update the category details with the given category code.

**delete-category (dc)** [category code]

Delete the category with the given category code.

**add-payee (ap)**

Add a new payee to the system. The user is prompted to enter the payee details.

**list-payees (lp)**

List all payees with their codes and names.

**update-payee (up)** [payee code]

Update the payee details with the given payee code.

**delete-payee (dp)** [payee code]

Delete the payee with the given payee code.

**add-recurring-charge (arc)** (*See extended form below*)

Add a new recurring charge to the currently selected account. The user is prompted to enter the charge details if no parameters are supplied.

**list-recurring-charges (lrc)**

List all recurring charges for the currently selected account, displaying their details and sequence numbers.

**update-recurring-charge (urc)** [recurring charge sequence number]

Update the recurring charge details with the given sequence number.

**delete-recurring-charge (drc)** [recurring charge sequence number]

Delete the recurring charge with the given sequence number.

**add-transaction (at, add)** (*See extended form below*)

Add a new transaction to the currently selected account. The user is prompted to enter the transaction details if no parameters are supplied.

**copy-transaction (ct, copy)** [transaction sequence number]

Copy the transaction with the given sequence number. If no sequence number is supplied, copy the last non-recurring transaction created.

**show-transaction (show, st)** [transaction sequence number]

Show the details of the transaction with the given sequence number.

**list-transactions (list, lt)** (*See extended forms below*)

List transactions for the currently selected account, displaying their details and sequence numbers. By default, show up to 25 non-recurring transactions in descending date order.

**update-transaction (ut)** [transaction sequence number]

Update the transaction details with the given sequence number.

**delete-transaction (dt)** [transaction sequence number]

Delete the transaction with the given sequence number.

**transfer-transaction (transfer, tr)** (*See extended form below*)

Transfer money from the currently selected account to another account, creating a transaction in each account. The user is prompted to enter the transfer details if no parameters are supplied.

**list-transfer-records (ltr)**

List all transfer records, displaying their details and sequence numbers.

**delete-transfer-record (dtr)** [transfer record sequence number]

Delete the transfer record with the given sequence number. This will also delete both transactions associated with the transfer.

**add-report (arp)**

Add a new transaction report to the system. The user is prompted to enter the report details and transaction selection criteria.

**list-reports (show-reports, lrp)**

List all saved transaction reports with their details and sequence numbers.

**update-report (urp)** [report sequence number]

Update the transaction report details and transaction selection criteria with the given sequence number.

**delete-report (drp)** [report sequence number]

Delete the saved transaction report with the given sequence number.

**run-report (run)** [report sequence number]

Run the specified transaction report and display the results. Running a report is equivalent to running the **find-transaction (find)** command specifying the sql parameter.

Manage shortcuts to commands that you run frequently, shortcuts are activated by following with the backslash character '\' which will replace the shortcut with the command you have specified

**add-shortcut (ash)**

Add a new shortcut to the system, the user is prompted to add the shortcut details.

**list-shortcuts (lsh)**

List all the shortcuts stored in the system including the sequence number.

**update-shortcut (ush)** [shortcut sequence number]

Update the given shortcut, the user is prompted to enter the details.

**delete-shortcut (dsh)** [shortcut sequence number]

Delete the given shortcut from the system.

PFM also supports import and export of entities via the following commands, these all accept/output files in JSON format, other than the **export-transasctions-csv** command which will output a CSV file, useful; for analysis in a spreadsheet program for example.

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

**import-categories (ic)** [JSON file path]

Import categories from the supplied JSON file that complies to the template above.

**export-categories (xc)** [JSON file path]

Export all categories to the supplied JSON file.

**import-payees (ip)** [JSON file path]

Import payees from the supplied JSON file that complies to the template above.

**export-payees (xp)** [JSON file path]

Export all payees to the supplied JSON file.

**import-recurring-charges (irc)** [JSON file path]

Import recurring charges from the supplied JSON file that complies to the template above.

**export-recurring-charges (xrc)** [JSON file path]

Export all recurring charges to the supplied JSON file.

**import-transactions (it)** [JSON file path]

Import transactions from the supplied JSON file that complies to the template above.

**export-transactions (xt)** [JSON file path]

Export all transactions to the supplied JSON file.

**export-transactions-csv (xtc)** [CSV file path]

Export all transactions to the supplied CSV file.

**add-recurring-charge (arc)** [parameters]

Add a new recurring charge to the currently selected account

- c:[category code] - The category code of the charge
- p:[payee code] - The payee code of the charge
- start:[date] - The start date for the charge (defaults to today)
- end:[date] - The end date for the charge (defaults to N/A)
- desc:[description] - The description
- freq:[frequency code] - The frequency of the charge - n[y|m|w|d], e.g. '1m' for monthly
- amnt:[amount] - The amount
- to:[account code] - The account code to transfer to, e.g. recurring transfer

**list-outstanding-charges (loc)**

Lists the oustanding charges this period for the currently selected account.

**list-paid-charges (llc)**

Lists the paid charges this period for the currently selected account.

**add-transaction (at, add)** [parameters]

Add a new transaction to the currently selected account

- c:[category code] - The category code of the transaction
- p:[payee code] - The payee code of the transaction
- date:[date] - The date for the transaction (defaults to today)
- desc:[description] - The description
- ref:[reference] - The reference
- type:[DB/CR] - Whether this is a debit or credit transaction
- amnt:[amount] - The amount
- rec:[Y/N] - Is the transaction reconciled or not

**list-transactions (lt, list)** [parameters]

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

**find-transactions (find)** [parameters]

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

- sql:[where clause parameters] - find transactions specified by the criteria. When you run a report, it will run this command with the report’s SQL criteria.

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
find sql:"category = 'BOOKS' AND amount > 25 AND recurring = 'N'"
```

The query used in the **find** command can be saved as a report by issuing the **save-report** or **save** command.

**use** [account-code]

Set the current account context to the account specified with account-code. The list-accounts command will show which account is the *primary* account, the initial state will set this account as the current account context until it is changed with the use command.

**save-json-template (sjt)**

Presents a menu to choose which entity to create a json template for, choose from account, payee, category, recurring charge, transaction. A file with the name <entity>_template.json is created in the current directory.

**set-primary-account (spa)** [account code]

Sets the primary account, like the immortals in *Highlander*, there can be only one. On pfm startup, the primary account is selected so you don’t need to issue a **use** command.

**clear-categories**

Deletes **all** stored categories, used for example if you don’t want any of the default categories created with a new file.

**transfer-transaction (transfer, tr)** [parameters]

Add a new transfer transaction to the currently selected account

- to:[account code] - The code of the account to transfer to
- c:[category code] - The category code of the transaction
- date:[date] - The date for the transaction (defaults to today)
- desc:[description] - The description
- amnt:[amount] - The amount
- rec:[Y/N] - Is the transaction reconciled or not

**reconcile-transaction (reconcile, rt)** [sequence number]

Toggle the reconciled status of the transaction specified by sequence.

**change-password**

Change the database password.

**version**

Print the pfm version string.

**help**

Display some usage information.

**exit (quit, q)**

Quit the application.

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

The database file is specified using the option -db <*filename*>, if this is not specified, pfm assumes the filename is .pfm residing in the current directory.

PFM supports importing and exporting of accounts, transactions, recurring charges, categories and payees. Import supports JSON format only, whilst pfm can export in JSON or CSV formats.

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

The PFM command save-json-template will save an example json file for the entity you choose.

# OPTIONS

**-db [db file]**

Specifies the filename of the SQLite database to open. If you do not specify this option, it assumes a database file named ‘.pfm’ in the current directory.

**--version, -v**

Prints the version information of the PFM program and exits.

**-h, -?**

Prints the supported command line options and exits.

# COMMANDS

For commands that support optional arguments, if no arguments are supplied, the user is presented with interactive prompts to enter the required data.

**add-account, aa**

Adds a new account to the system with the following fields:

	code: A unique max 5 character code for the account
	name: The account name
	opening balance: The balance at the opening date
	opening date: The date the account is active from

**list-accounts, la**

List all accounts, the following fields are displayed:

	code: The unique account code
	name: The account name
	current balance: The balance of the account as of now
	balance after bills: The projected balance at the end of the current period

**update-account, ua <account code>**

Update the account details as entered with the add-account command.


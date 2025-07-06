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




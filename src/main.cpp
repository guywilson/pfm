#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <gmp.h>
#include <mpfr.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "db.h"

static void printUsage(void) {

}

static void printVersion(void) {

}

static void add_account(void) {
    string          accountName;
    string          accountCode;
    string          openingBalance;
    double          balance;
    sqlite3_int64   accountId;

    cout << "*** Add account ***" << endl;

    cout << "Account name: ";
    cin >> accountName;

    cout << "Account code (max. 3 chars): ";
    cin >> accountCode;

    cout << "Opening balance [0.00]: ";
    cin >> openingBalance;

    if (openingBalance.length() > 0) {
        balance = strtod(openingBalance.c_str(), NULL);
    }
    else {
        balance = 0.0;
    }

    AccountDB db = AccountDB::getInstance();

    accountId = db.createAccount(
                        accountName, 
                        accountCode, 
                        balance);

    cout << "Created account with ID " << accountId << endl;
}

int main(int argc, char ** argv) {
    int             i;
    char *          pszCommand;
    char *          pszDatabase;
    bool            loop = true;

    rl_bind_key('\t', rl_complete);

    using_history();

	if (argc > 1) {
		for (i = 1;i < argc;i++) {
			if (argv[i][0] == '-') {
				if (strncmp(&argv[i][1], "db", 2) == 0) {
					pszDatabase = strdup(&argv[++i][0]);
				}
				else if (argv[i][1] == 'h' || argv[i][1] == '?') {
					printUsage();
					return 0;
				}
				else {
					printf("Unknown argument '%s'", &argv[i][0]);
					printUsage();
					return 0;
				}
			}
		}
	}
	else {
		printUsage();
		return -1;
	}

    AccountDB db = AccountDB::getInstance();

    db.open(pszDatabase);

    while (loop) {
        pszCommand = readline("pfm > ");

        /*
        ** Commands:
        **
        ** add account      n:name, c:code, b:opening_balance
        ** add transaction  
        */
        if (strlen(pszCommand)) {
            add_history(pszCommand);

            if (strncmp(pszCommand, "exit", 4) == 0 || strncmp(pszCommand, "quit", 4) == 0 || pszCommand[0] == 'q') {
                loop = false;
            }
            else if (strncmp(pszCommand, "help", 4) == 0) {
                printUsage();
            }
            else if (strncmp(pszCommand, "version", 7) == 0) {
                printVersion();
            }
            else if (strncmp(pszCommand, "add account", 11) == 0 || strncmp(pszCommand, "aa", 2) == 0) {
                add_account();
            }
        }
    }

    return 0; 
}

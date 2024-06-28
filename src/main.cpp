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

static string fixStrWidth(string & src, int requiredLen) {
    string          target;

    if (src.length() > requiredLen) {
        target = src.substr(0, requiredLen - 2);
        target.append("..");
    }
    else if (src.length() < requiredLen) {
        target = src;
        
        for (int i = 0;i < requiredLen - src.length();i++) {
            target.append(" ");
        }
    }
    else {
        target = src;
    }

    return target;
}

static string formatCurrency(double src) {
    static char szAmount[16];

    snprintf(szAmount, 15, "£%.2f", (float)src);

    return string(szAmount);
}

static char * readString(const char * pszPrompt, const char * pszDefault, const size_t maxLength) {
    char *      pszAnswer;

    pszAnswer = readline(pszPrompt);

    if (strlen(pszAnswer) == 0 && pszDefault != NULL) {
        pszAnswer = strndup(pszDefault, maxLength);
    }

    return pszAnswer;
}

static char readChar(const char * pszPrompt) {
    char        answer;

    printf("%s", pszPrompt);
    fflush(stdout);

    answer = getchar();
    fflush(stdin);

    return answer;
}

static void add_account(void) {
    char *          accountName;
    char *          accountCode;
    char *          openingBalance;
    double          balance;
    sqlite3_int64   accountId;

    cout << "*** Add account ***" << endl;

    accountName = readString("Account name: ", NULL, 32);
    accountCode = readString("Account code (max. 3 chars): ", NULL, 3);
    openingBalance = readString("Opening balance [0.00]: ", "0.00", 32);

    if (strlen(openingBalance) > 0) {
        balance = strtod(openingBalance, NULL);
    }
    else {
        balance = 0.0;
    }

    AccountDB & db = AccountDB::getInstance();

    accountId = db.createAccount(
                        accountName, 
                        accountCode, 
                        balance);

    cout << "Created account with ID " << accountId << endl;
}

static void list_accounts(void) {
    AccountResult           result;
    int                     numAccounts;
    int                     i;

    AccountDB & db = AccountDB::getInstance();

    numAccounts = db.getAccounts(&result);

    cout << "*** Accounts (" << numAccounts << ") ***" << endl << endl;
    cout << "| Code | Name            | Balance" << endl;
    cout << "----------------------------------" << endl;

    for (i = 0;i < numAccounts;i++) {
        Account account = result.results[i];

        cout << 
            "| " << 
            fixStrWidth(account.code, 4) << 
            " | " << 
            fixStrWidth(account.name, 15) << 
            " | " << 
            formatCurrency(account.currentBalance) << endl;
    }

    cout << endl;
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

    AccountDB & db = AccountDB::getInstance();

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
            else if (strncmp(pszCommand, "list accounts", 14) == 0 || strncmp(pszCommand, "la", 2) == 0) {
                list_accounts();
            }
        }
    }

    return 0; 
}

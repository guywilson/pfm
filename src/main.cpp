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

#define DEFAULT_DATABASE_NAME                   ".pfm"

#define FIELD_STRING_LEN                        64
#define MAX_PROMPT_LENGTH                      128
#define AMOUNT_FIELD_STRING_LEN                 16

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
    accountCode = readString("Account code (max. 4 chars): ", NULL, 4);
    openingBalance = readString("Opening balance [0.00]: ", "0.00", 32);

    if (strlen(openingBalance) > 0) {
        balance = strtod(openingBalance, NULL);
    }
    else {
        balance = 0.0;
    }

    if (strlen(accountCode) == 0) {
        fprintf(stderr, "\nAccount code must have a value.\n");
        return;
    }

    Account account;
    account.name = accountName;
    account.code = accountCode;
    account.openingBalance = balance;
    account.currentBalance = balance;

    AccountDB & db = AccountDB::getInstance();

    accountId = db.createAccount(account);

    cout << "Created account with ID " << accountId << endl;

    free(openingBalance);
    free(accountCode);
    free(accountName);
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

static Account choose_account(const char * szAccountCode) {
    char *          accountCode;
    AccountResult   result;

    if (szAccountCode == NULL || strlen(szAccountCode) == 0) {
        cout << "*** Use account ***" << endl;
        accountCode = readString("Account code (max. 4 chars): ", NULL, 4);
    }
    else {
        accountCode = strdup(szAccountCode);
    }

    AccountDB & db = AccountDB::getInstance();

    db.getAccount(accountCode, &result);

    free(accountCode);

    return result.results[0];
}

static void update_account(Account & account) {
    char            szPrompt[MAX_PROMPT_LENGTH];
    char            szBalance[AMOUNT_FIELD_STRING_LEN];
    char *          pszBalance;

    cout << "*** Update account ***" << endl;

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Account name ['%s']: ", account.name.c_str());
    account.name = readString(szPrompt, account.name.c_str(), FIELD_STRING_LEN);

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Account code ['%s']: ", account.code.c_str());
    account.code = readString(szPrompt, account.code.c_str(), FIELD_STRING_LEN);

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Opening balance [%.2f]: ", account.openingBalance);
    snprintf(szBalance, AMOUNT_FIELD_STRING_LEN, "%.2f", account.openingBalance);
    pszBalance = readString(szPrompt, szBalance, AMOUNT_FIELD_STRING_LEN);
    account.openingBalance = strtod(pszBalance, NULL);

    snprintf(szPrompt, MAX_PROMPT_LENGTH, "Current balance [%.2f]: ", account.currentBalance);
    snprintf(szBalance, AMOUNT_FIELD_STRING_LEN, "%.2f", account.currentBalance);
    pszBalance = readString(szPrompt, szBalance, AMOUNT_FIELD_STRING_LEN);
    account.currentBalance = strtod(pszBalance, NULL);

    if (account.code.length() == 0) {
        fprintf(stderr, "\nAccount code must have a value.\n");
        return;
    }

    AccountDB & db = AccountDB::getInstance();

    db.updateAccount(account);

    free(pszBalance);
}

static void delete_account(Account & account) {
    AccountDB & db = AccountDB::getInstance();

    db.deleteAccount(account);
}

static void list_categories(void) {
    CategoryResult          result;
    int                     numCategories;
    int                     i;

    AccountDB & db = AccountDB::getInstance();

    numCategories = db.getCategories(&result);

    cout << "*** Categories (" << numCategories << ") ***" << endl << endl;
    cout << "| Code  | Description              " << endl;
    cout << "-----------------------------------" << endl;

    for (i = 0;i < numCategories;i++) {
        Category category = result.results[i];

        cout << 
            "| " << 
            fixStrWidth(category.code, 5) << 
            " | " << 
            fixStrWidth(category.description, 25) << endl;
    }

    cout << endl;
}

int main(int argc, char ** argv) {
    int             i;
    char *          pszCommand;
    char *          pszDatabase = NULL;
    bool            loop = true;
    Account         selectedAccount;

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

    if (pszDatabase == NULL) {
        pszDatabase = strdup(DEFAULT_DATABASE_NAME);
    }

    AccountDB & db = AccountDB::getInstance();

    db.open(pszDatabase);

    while (loop) {
        pszCommand = readline("pfm > ");

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
            else if (strncmp(pszCommand, "use account", 12) == 0 || strncmp(pszCommand, "use", 3) == 0) {
                char * accountCode = NULL;

                if (strncmp(pszCommand, "use account", 12) == 0) {
                    if (strlen(pszCommand) >= 16) {
                        accountCode = &pszCommand[13];
                    }
                }
                else if (strncmp(pszCommand, "use", 3) == 0) {
                    if (strlen(pszCommand) >= 7) {
                        accountCode = &pszCommand[4];
                    }
                }

                selectedAccount.setAccount(choose_account(accountCode));
            }
            else if (strncmp(pszCommand, "update account", 15) == 0 || strncmp(pszCommand, "ua", 2) == 0) {
                update_account(selectedAccount);
            }
            else if (strncmp(pszCommand, "delete account", 15) == 0 || strncmp(pszCommand, "da", 2) == 0) {
                delete_account(selectedAccount);
                selectedAccount.clear();
            }
            else if (strncmp(pszCommand, "list categories", 16) == 0 || strncmp(pszCommand, "lc", 2) == 0) {
                list_categories();
            }
        }
    }

    return 0; 
}

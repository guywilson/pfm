#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "db.h"
#include "account.h"
#include "category.h"
#include "cli.h"

using namespace std;

#define DEFAULT_DATABASE_NAME                   ".pfm"

static void printUsage(void) {

}

static void printVersion(void) {

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

    PFM_DB & db = PFM_DB::getInstance();

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
            else if (strncmp(pszCommand, "add category", 13) == 0 || strncmp(pszCommand, "ac", 2) == 0) {
                add_category();
            }
            else if (strncmp(pszCommand, "list categories", 16) == 0 || strncmp(pszCommand, "lc", 2) == 0) {
                list_categories();
            }
            else if (strncmp(pszCommand, "update category", 15) == 0 || strncmp(pszCommand, "uc", 2) == 0) {
                char * categoryCode = NULL;

                if (strncmp(pszCommand, "update category", 15) == 0) {
                    if (strlen(pszCommand) >= 19) {
                        categoryCode = &pszCommand[16];
                    }
                }
                else if (strncmp(pszCommand, "uc", 2) == 0) {
                    if (strlen(pszCommand) >= 6) {
                        categoryCode = &pszCommand[3];
                    }
                }

                Category c;
                c.setCategory(get_category(categoryCode));

                update_category(c);
            }
            else if (strncmp(pszCommand, "delete category", 15) == 0 || strncmp(pszCommand, "dc", 2) == 0) {
                char * categoryCode = NULL;

                if (strncmp(pszCommand, "delete category", 15) == 0) {
                    if (strlen(pszCommand) >= 19) {
                        categoryCode = &pszCommand[16];
                    }
                }
                else if (strncmp(pszCommand, "dc", 2) == 0) {
                    if (strlen(pszCommand) >= 6) {
                        categoryCode = &pszCommand[3];
                    }
                }

                Category c;
                c.setCategory(get_category(categoryCode));
                c.print();

                delete_category(c);
            }
            else if (strncmp(pszCommand, "add payee", 9) == 0 || strncmp(pszCommand, "ap", 2) == 0) {
                add_payee();
            }
            else if (strncmp(pszCommand, "list payees", 11) == 0 || strncmp(pszCommand, "lp", 2) == 0) {
                list_payees();
            }
            else if (strncmp(pszCommand, "update payee", 12) == 0 || strncmp(pszCommand, "up", 2) == 0) {
                char * payeeCode = NULL;

                if (strncmp(pszCommand, "update payee", 12) == 0) {
                    if (strlen(pszCommand) >= 16) {
                        payeeCode = &pszCommand[13];
                    }
                }
                else if (strncmp(pszCommand, "up", 2) == 0) {
                    if (strlen(pszCommand) >= 6) {
                        payeeCode = &pszCommand[3];
                    }
                }

                Payee p;
                p.setPayee(get_payee(payeeCode));

                update_payee(p);
            }
            else if (strncmp(pszCommand, "delete payee", 12) == 0 || strncmp(pszCommand, "dc", 2) == 0) {
                char * payeeCode = NULL;

                if (strncmp(pszCommand, "delete payee", 12) == 0) {
                    if (strlen(pszCommand) >= 16) {
                        payeeCode = &pszCommand[13];
                    }
                }
                else if (strncmp(pszCommand, "dc", 2) == 0) {
                    if (strlen(pszCommand) >= 6) {
                        payeeCode = &pszCommand[3];
                    }
                }

                Payee p;
                p.setPayee(get_payee(payeeCode));
                p.print();

                delete_payee(p);
            }
        }
    }

    return 0; 
}

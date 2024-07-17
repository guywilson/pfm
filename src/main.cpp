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
#include "db_account.h"
#include "db_category.h"
#include "db_payee.h"
#include "db_recurring_charge.h"
#include "db_transaction.h"
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
    DBAccount         selectedAccount;

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

                DBCategory c;
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

                DBCategory c;
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

                DBPayee p;
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

                DBPayee p;
                p.setPayee(get_payee(payeeCode));
                p.print();

                delete_payee(p);
            }
            else if (strncmp(pszCommand, "add recurring charge", 20) == 0 || strncmp(pszCommand, "arc", 3) == 0) {
                add_recurring_charge(selectedAccount);
            }
            else if (strncmp(pszCommand, "list recurring charges", 22) == 0 || strncmp(pszCommand, "lrc", 3) == 0) {
                list_recurring_charges(selectedAccount);
            }
            else if (strncmp(pszCommand, "update recurring charge", 23) == 0 || strncmp(pszCommand, "urc", 3) == 0) {
                int sequence = 0;

                if (strncmp(pszCommand, "update recurring charge", 23) == 0) {
                    if (strlen(pszCommand) >= 25) {
                        sequence = atoi(&pszCommand[24]);
                    }
                }
                else if (strncmp(pszCommand, "urc", 3) == 0) {
                    if (strlen(pszCommand) >= 5) {
                        sequence = atoi(&pszCommand[4]);
                    }
                }

                DBRecurringCharge rc;
                rc.setRecurringCharge(get_recurring_charge(sequence));

                update_recurring_charge(rc);
            }
            else if (strncmp(pszCommand, "delete recurring charge", 23) == 0 || strncmp(pszCommand, "drc", 3) == 0) {
                int sequence = 0;

                if (strncmp(pszCommand, "delete recurring charge", 23) == 0) {
                    if (strlen(pszCommand) >= 25) {
                        sequence = atoi(&pszCommand[24]);
                    }
                }
                else if (strncmp(pszCommand, "drc", 3) == 0) {
                    if (strlen(pszCommand) >= 5) {
                        sequence = atoi(&pszCommand[4]);
                    }
                }

                DBRecurringCharge rc;
                rc.setRecurringCharge(get_recurring_charge(sequence));

                delete_recurring_charge(rc);
            }
            else if (strncmp(pszCommand, "add transaction", 15) == 0 || strncmp(pszCommand, "at", 2) == 0) {
                add_transaction(selectedAccount);
            }
            else if (strncmp(pszCommand, "list transactions", 17) == 0 || strncmp(pszCommand, "lt", 2) == 0) {
                list_transactions(selectedAccount);
            }
            else if (strncmp(pszCommand, "update transaction", 18) == 0 || strncmp(pszCommand, "ut", 2) == 0) {
                int sequence = 0;

                if (strncmp(pszCommand, "update transaction", 18) == 0) {
                    if (strlen(pszCommand) >= 20) {
                        sequence = atoi(&pszCommand[19]);
                    }
                }
                else if (strncmp(pszCommand, "ut", 2) == 0) {
                    if (strlen(pszCommand) >= 4) {
                        sequence = atoi(&pszCommand[3]);
                    }
                }

                DBTransaction t;
                t.setTransaction(get_transaction(sequence));

                update_transaction(t);
            }
            else if (strncmp(pszCommand, "delete transaction", 18) == 0 || strncmp(pszCommand, "dt", 2) == 0) {
                int sequence = 0;

                if (strncmp(pszCommand, "delete transaction", 18) == 0) {
                    if (strlen(pszCommand) >= 20) {
                        sequence = atoi(&pszCommand[19]);
                    }
                }
                else if (strncmp(pszCommand, "dt", 2) == 0) {
                    if (strlen(pszCommand) >= 4) {
                        sequence = atoi(&pszCommand[3]);
                    }
                }

                DBTransaction t;
                t.setTransaction(get_transaction(sequence));

                delete_transaction(t);
            }
        }
    }

    return 0; 
}

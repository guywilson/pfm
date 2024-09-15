#include <iostream>
#include <string>
#include <locale>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "db.h"
#include "logger.h"
#include "keymgr.h"
#include "db_account.h"
#include "db_category.h"
#include "db_payee.h"
#include "db_recurring_charge.h"
#include "db_transaction.h"
#include "cli.h"
#include "money.h"
#include "strdate.h"
#include "version.h"

using namespace std;

#define DEFAULT_DATABASE_NAME                   ".pfm"

#ifdef PFM_TEST_SUITE_ENABLED
extern void testAccount();
#endif

static void printUsage(void) {

}

static void printVersion(void) {
    cout << "PFM version '" << getVersion() << "' - built [" << getBuildDate() << "]" << endl << endl;
}

int main(int argc, char ** argv) {
    int             i;
    char *          pszCommand;
    char *          pszDatabase = NULL;
    bool            loop = true;
    DBAccount       selectedAccount;
    DBUser          loggedInUser;

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
                else if (argv[i][1] == 'v' || strncmp(&argv[i][1], "-version", 8) == 0) {
                    printVersion();
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

    string logFileName = "./pfm.log";

    Logger & log = Logger::getInstance();
    log.initLogger(logFileName, LOG_LEVEL_ALL);

    PFM_DB & db = PFM_DB::getInstance();
    db.open(pszDatabase);
    
#ifdef PFM_TEST_SUITE_ENABLED
    MoneyTest::run();
    cout << endl << endl;
    StrDateTest::run();
    cout << endl << endl;
    EncryptionTest::run();
    cout << endl << endl;
    testAccount();
    exit(0);
#endif

    loggedInUser = login();

    while (loop) {
        pszCommand = readline("pfm > ");

        if (strlen(pszCommand)) {
            add_history(pszCommand);

            try {
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
                    addAccount(loggedInUser);
                }
                else if (strncmp(pszCommand, "list accounts", 14) == 0 || strncmp(pszCommand, "la", 2) == 0) {
                    listAccounts();
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

                    selectedAccount.set(chooseAccount(accountCode));
                }
                else if (strncmp(pszCommand, "update account", 15) == 0 || strncmp(pszCommand, "ua", 2) == 0) {
                    if (selectedAccount.id != 0) {
                        updateAccount(selectedAccount);
                    }
                    else {
                        throw pfm_error("No account selected");
                    }
                }
                else if (strncmp(pszCommand, "delete account", 15) == 0 || strncmp(pszCommand, "da", 2) == 0) {
                    if (selectedAccount.id != 0) {
                        selectedAccount.remove();
                        selectedAccount.clear();
                    }
                    else {
                        throw pfm_error("No account selected");
                    }
                }
                else if (strncmp(pszCommand, "show balance", 12) == 0 || strncmp(pszCommand, "sb", 2) == 0) {
                    if (selectedAccount.id != 0) {
                        showAccountBalances(selectedAccount);
                    }
                    else {
                        throw pfm_error("No account selected");
                    }
                }
                else if (strncmp(pszCommand, "add category", 13) == 0 || strncmp(pszCommand, "ac", 2) == 0) {
                    addCategory();
                }
                else if (strncmp(pszCommand, "list categories", 16) == 0 || strncmp(pszCommand, "lc", 2) == 0) {
                    listCategories();
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
                    c.set(getCategory(categoryCode));

                    updateCategory(c);
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
                    c.set(getCategory(categoryCode));
                    c.print();

                    c.remove();
                }
                else if (strncmp(pszCommand, "add payee", 9) == 0 || strncmp(pszCommand, "ap", 2) == 0) {
                    addPayee();
                }
                else if (strncmp(pszCommand, "list payees", 11) == 0 || strncmp(pszCommand, "lp", 2) == 0) {
                    listPayees();
                }
                else if (strncmp(pszCommand, "update payee", 12) == 0 || strncmp(pszCommand, "up", 2) == 0) {
                    char * payeeCode = NULL;

                    if (strncmp(pszCommand, "update payee", 12) == 0) {
                        if (strlen(pszCommand) >= 16) {
                            payeeCode = &pszCommand[13];
                        }
                    }
                    else if (strncmp(pszCommand, "up", 2) == 0) {
                        if (strlen(pszCommand) >= 5) {
                            payeeCode = &pszCommand[3];
                        }
                    }

                    DBPayee p;
                    p.set(getPayee(payeeCode));

                    updatePayee(p);
                }
                else if (strncmp(pszCommand, "delete payee", 12) == 0 || strncmp(pszCommand, "dc", 2) == 0) {
                    char * payeeCode = NULL;

                    if (strncmp(pszCommand, "delete payee", 12) == 0) {
                        if (strlen(pszCommand) >= 16) {
                            payeeCode = &pszCommand[13];
                        }
                    }
                    else if (strncmp(pszCommand, "dc", 2) == 0) {
                        if (strlen(pszCommand) >= 5) {
                            payeeCode = &pszCommand[3];
                        }
                    }

                    DBPayee p;
                    p.set(getPayee(payeeCode));
                    p.print();

                    p.remove();
                }
                else if (strncmp(pszCommand, "add recurring charge", 20) == 0 || strncmp(pszCommand, "arc", 3) == 0) {
                    if (selectedAccount.id != 0) {
                        addRecurringCharge(selectedAccount);
                    }
                    else {
                        throw pfm_error("No account selected");
                    }
                }
                else if (strncmp(pszCommand, "list recurring charges", 22) == 0 || strncmp(pszCommand, "lrc", 3) == 0) {
                    if (selectedAccount.id != 0) {
                        listRecurringCharges(selectedAccount);
                    }
                    else {
                        throw pfm_error("No account selected");
                    }
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
                    rc.set(getRecurringCharge(sequence));

                    updateRecurringCharge(rc);
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
                    rc.set(getRecurringCharge(sequence));

                    rc.remove();
                }
                else if (strncmp(pszCommand, "add transaction", 15) == 0 || strncmp(pszCommand, "at", 2) == 0) {
                    if (selectedAccount.id != 0) {
                        addTransaction(selectedAccount);
                    }
                    else {
                        throw pfm_error("No account selected");
                    }
                }
                else if (strncmp(pszCommand, "list transactions", 17) == 0 || strncmp(pszCommand, "lt", 2) == 0) {
                    if (selectedAccount.id != 0) {
                        listTransactions(selectedAccount);
                    }
                    else {
                        throw pfm_error("No account selected");
                    }
                }
                else if (strncmp(pszCommand, "find transactions", 17) == 0 || strncmp(pszCommand, "ft", 2) == 0) {
                    if (selectedAccount.id != 0) {
                        findTransactions(selectedAccount);
                    }
                    else {
                        throw pfm_error("No account selected");
                    }
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
                    t.set(getTransaction(sequence));

                    updateTransaction(t);
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
                    t.set(getTransaction(sequence));

                    deleteTransaction(t);
                }
                else if (strncmp(pszCommand, "add budget", 10) == 0 || strncmp(pszCommand, "ab", 2) == 0) {
                    addBudget();
                }
                else if (strncmp(pszCommand, "list budgets", 12) == 0 || strncmp(pszCommand, "lb", 2) == 0) {
                    listBudgets();
                }
                else if (strncmp(pszCommand, "update budget", 13) == 0 || strncmp(pszCommand, "ub", 2) == 0) {
                    int sequence = 0;

                    if (strncmp(pszCommand, "update budget", 13) == 0) {
                        if (strlen(pszCommand) >= 15) {
                            sequence = atoi(&pszCommand[14]);
                        }
                    }
                    else if (strncmp(pszCommand, "ub", 2) == 0) {
                        if (strlen(pszCommand) >= 4) {
                            sequence = atoi(&pszCommand[3]);
                        }
                    }

                    DBBudget b;
                    b.set(getBudget(sequence));

                    b.print();

                    updateBudget(b);
                }
                else if (strncmp(pszCommand, "delete budget", 13) == 0 || strncmp(pszCommand, "db", 2) == 0) {
                    int sequence = 0;

                    if (strncmp(pszCommand, "delete budget", 13) == 0) {
                        if (strlen(pszCommand) >= 15) {
                            sequence = atoi(&pszCommand[14]);
                        }
                    }
                    else if (strncmp(pszCommand, "db", 2) == 0) {
                        if (strlen(pszCommand) >= 4) {
                            sequence = atoi(&pszCommand[3]);
                        }
                    }

                    DBBudget b;
                    b.set(getBudget(sequence));

                    deleteBudget(b);
                }
                else if (strncmp(pszCommand, "list carried over logs", 22) == 0) {
                    listCarriedOverLogs(selectedAccount);
                }
                else if (strncmp(pszCommand, "list budget track records", 25) == 0) {
                    listBudgetTracks();
                }
            }
            catch (pfm_fatal & f) {
                fprintf(stderr, "Fatal error: %s\n", f.what());
                exit(-1);
            }
            catch (pfm_error & e) {
                cout << "Error running command: " << e.what() << endl;
            }
        }
    }

    return 0; 
}

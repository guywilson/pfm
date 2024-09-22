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
#include "db_account.h"
#include "db_category.h"
#include "db_payee.h"
#include "db_recurring_charge.h"
#include "db_transaction.h"
#include "command.h"
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

int main(int argc, char ** argv) {
    int             i;
    char *          pszDatabase = NULL;
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
                else if (argv[i][1] == 'v' || strncmp(&argv[i][1], "-version", 8) == 0) {
                    Command::version();
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

    // loggedInUser = Command::login();

    Command command;
    // command.setLoggedInUser(loggedInUser);

    while (loop) {
        string cmdString = readline("pfm > ");

        if (cmdString.length()) {
            add_history(cmdString.c_str());

            try {
                loop = command.process(cmdString);
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

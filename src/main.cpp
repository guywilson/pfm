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
#include "terminal.h"
#include "money.h"
#include "strdate.h"
#include "rlcustom.h"
#include "version.h"

using namespace std;

#define DEFAULT_DATABASE_NAME                   ".pfm"

#ifdef PFM_TEST_SUITE_ENABLED
extern void testAccount();
#endif

static void printUsage(void) {

}

#ifdef PFM_TEST_SUITE_ENABLED
void test() {
    MoneyTest::run();
    cout << endl << endl;
    StrDateTest::run();
    cout << endl << endl;
    testAccount();
    exit(0);
}
#endif

static void checkTerminalSize(void) {
    Terminal & t = Terminal::getInstance();

    if ((int)t.getWidth() < TERMINAL_MIN_WIDTH || (int)t.getHeight() < TERMINAL_MIN_HEIGHT) {
        fprintf(
            stderr, 
            "Terminal size must be at least %d x %d to run this program. " \
            "Current size is %d x %d\n\n", 
            TERMINAL_MIN_WIDTH,
            TERMINAL_MIN_HEIGHT,
            (int)t.getWidth(), 
            (int)t.getHeight());

        exit(-1);
    }
}

int main(int argc, char ** argv) {
    int             i;
    char *          pszDatabase = NULL;
    bool            loop = true;

    rl_utils::setup();

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

#ifndef RUN_IN_DEBUGGER
    checkTerminalSize();
#endif

    string logFileName = "./pfm.log";

    Logger & log = Logger::getInstance();
    log.initLogger(logFileName, LOG_LEVEL_FATAL | LOG_LEVEL_ERROR);

    PFM_DB & db = PFM_DB::getInstance();
    db.open(pszDatabase);
    free(pszDatabase);

#ifdef PFM_TEST_SUITE_ENABLED
    test();
#endif

    Command command;
    command.process("list-accounts");

#ifdef RUN_IN_DEBUGGER
    command.process("set-logging-level all");
    command.process("use HSBC");
#endif

    while (loop) {
        rl_utils::setLineLength(DEFAULT_LINE_LENGTH);
        
        string cmdString = readline("pfm > ");

        if (cmdString.length()) {
            try {
                loop = command.process(cmdString);
            }
            catch (pfm_fatal & f) {
                fprintf(stderr, "Fatal error: %s\n", f.what());
                exit(-1);
            }
            catch (pfm_field_cancel_error & cancelledError) {
                cout << endl;
                continue;
            }
            catch (pfm_error & e) {
                cout << "Error running command: " << e.what() << endl;
            }
        }
    }

    return 0; 
}

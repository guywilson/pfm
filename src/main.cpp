/*
** Copyright (c) 2025, Guy Wilson
** 
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
** 
** 1. Redistributions of source code must retain the above copyright notice, this
**    list of conditions and the following disclaimer.
** 
** 2. Redistributions in binary form must reproduce the above copyright notice,
**    this list of conditions and the following disclaimer in the documentation
**    and/or other materials provided with the distribution.
** 
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
** CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
** OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
#include "db_primary_account.h"
#include "db_category.h"
#include "db_payee.h"
#include "db_recurring_charge.h"
#include "db_transaction.h"
#include "command.h"
#include "cfgmgr.h"
#include "terminal.h"
#include "money.h"
#include "strdate.h"
#include "rlcustom.h"
#include "license.h"
#include "version.h"

//#define RUN_IN_DEBUGGER

using namespace std;

#define DEFAULT_DATABASE_NAME                   ".pfm"

#ifdef PFM_TEST_SUITE_ENABLED
extern void testAccount();
#endif

static void printUsage(void) {
    cout << "pfm [options]" << endl;
    cout << "Options:" << endl;
    cout << "\t-db <db name> if not specified, defaults to '.pfm' in the current dir" << endl;
    cout << "\t-h displays the help" << endl;
    cout << "\t--license, -l print the license" << endl;
    cout << "\t--version, -v print the program version" << endl << endl;
}

static void printLicense(void) {
    cout << licenseText << endl << endl;
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
    char * pszDatabase = strdup(DEFAULT_DATABASE_NAME);

    rl_utils::setup();

	if (argc > 1) {
		for (int i = 1;i < argc;i++) {
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
                else if (argv[i][1] == 'l' || strncmp(&argv[i][1], "-license", 8) == 0) {
                    printLicense();
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

#ifndef RUN_IN_DEBUGGER
    checkTerminalSize();
#endif

    string logFileName = "./pfm.log";

    Logger & log = Logger::getInstance();
    log.initLogger(logFileName, LOG_LEVEL_FATAL | LOG_LEVEL_ERROR);

    PFM_DB & db = PFM_DB::getInstance();

    try {
        db.open(pszDatabase);
    }
    catch (pfm_fatal & f) {
        log.logFatal("Fatal error: %s", f.what());
        log.closeLogger();
        
        free(pszDatabase);
        return -1;
    }

    free(pszDatabase);

#ifdef PFM_TEST_SUITE_ENABLED
    test();
#endif

    cfgmgr & cfg = cfgmgr::getInstance();
    cfg.initialise();

    DBResult<DBAccount> accounts;
    accounts.retrieveAll();

    int numAccounts = accounts.getNumRows();

    Command command;
    
    if (numAccounts > 0) {
        command.process("list-accounts");
    }
    else {
        cout << endl << "*** Welcome to PFM ***" << endl << endl << "Create your first account..." << endl << endl;
        command.process("add-account");
    }

#ifdef RUN_IN_DEBUGGER
    command.process("set-logging-level all");
#endif

    string primaryAccountCode = DBPrimaryAccount::getPrimaryAccountCode();
    command.process("use " + primaryAccountCode);

    command.process("list 12:all:desc");
    
    bool loop = true;

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

    db.close();
    log.closeLogger();

    return 0; 
}

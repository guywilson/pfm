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

int main(int argc, char ** argv) {
    int             i;
    char *          pszCommand;
    char *          pszDatabase;
    bool            loop = true;
    bool            isDumpConfig = false;

    rl_bind_key('\t', rl_complete);

    using_history();

	if (argc > 1) {
		for (i = 1;i < argc;i++) {
			if (argv[i][0] == '-') {
				if (strncmp(&argv[i][1], "db", 2) == 0) {
					pszDatabase = strdup(&argv[++i][0]);
				}
				else if (strcmp(&argv[i][1], "-dump-config") == 0) {
					isDumpConfig = true;
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
            else if (strncmp(pszCommand, "init", 4) == 0) {
                printVersion();
            }
        }
    }

    return 0; 
}

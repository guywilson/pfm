#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <vector>
#include <unordered_map>
#include <functional>

#include <sqlcipher/sqlite3.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "pfm_error.h"
#include "db.h"
#include "cfgmgr.h"
#include "cache.h"
#include "strdate.h"
#include "version.h"
#include "db_account.h"
#include "db_category.h"
#include "db_payee.h"
#include "db_recurring_charge.h"
#include "db_transaction.h"
#include "jfile.h"
#include "command.h"
#include "command_table.h"
#include "bold_modifier.h"

using namespace std;

void Command::help() {
    cout << "For more detailed help, please see the manual" << endl;
    cout << "e.g. man pfm" << endl << endl;

    cout << "Commands supported:" << endl << endl;

    for (const auto &entry : commandTable) {
        if (entry.helpText.length() == 0) {
            continue;
        }
        
        cout << bold_on << entry.name << bold_off;

        if (entry.aliases.empty()) {
            cout << endl;
        }
        else {
            cout << " [ ";

            size_t i = 0;
            for (const auto & alias : entry.aliases) {
                cout << alias;

                if (i < entry.aliases.size() - 1) {
                    cout << ", ";
                }

                i++;
            }

            cout << " ]" << endl;
        }

        if (entry.helpText.length() > 0) {
            cout << entry.helpText << endl;
        }
        cout << endl;
    }

    cout << endl;
}

void Command::version() {
    cout << "PFM version '" << getVersion() << "' - built [" << getBuildDate() << "]" << endl << endl;
}

string Command::parse(const string & commandLine) {
    const char * parameterDelimiters = ";:|";

    char * cmdString = strdup(commandLine.c_str());;
    char * part = strtok(cmdString, " ");

    this->parameters.clear();

    string command;

    int i = 0;
    while (part != NULL) {
        if (i == 0) {
            command = part;
        }
        else {
            this->parameters.push_back(part);
        }

        part = strtok(NULL, parameterDelimiters);
        i++;
    }

    return command;
}

bool Command::process(const string & commandLine) {
    clear_history();

    for (string & cmd : commandHistory) {
        add_history(cmd.c_str());
    }

    string command = parse(commandLine);

    add_history(commandLine.c_str());
    commandHistory.push_back(commandLine);

    if (command.compare("exit") == 0 || command.compare("quit") == 0 || command.compare("q") == 0) {
        return false;
    }
    else if (command.compare("version") == 0) {
        Command::version();
        return true;
    }
    else if (command.compare("help") == 0) {
        Command::help();
        return true;
    }
    else {
        bool isAlias = false;
        for (const auto &entry : commandTable) {
            for (const auto & alias : entry.aliases) {
                if (command == alias) {
                    isAlias = true;
                }
            }
            if (command == entry.name || isAlias) {
                entry.handler(*this);

                for (string & command : commandHistory) {
                    add_history(command.c_str());
                }

                return true;
            }
        }
    }

    throw pfm_validation_error(
                pfm_error::buildMsg(
                    "Sorry, I do not understand command '%s', please see the manual for supported commands.", 
                    command.c_str()));
}

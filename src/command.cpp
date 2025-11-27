#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <vector>
#include <unordered_map>
#include <sstream>
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

bool Command::isStringNumeric(const string & s) {
    bool isNumeric = true;

    for (char c : s) {
        if (!isdigit(c)) {
            isNumeric = false;
            break;
        }
    }

    return isNumeric;
}

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

void Command::handleExceptions(const string & command, const string & token) {
    string value = trim(token);

    if (isStringNumeric(token)) {
        if (command == "list") {
            string name = "rows";
            parameters[name].push_back(value);
        }
        else {
            string name = SEQUENCE_PARAM_NAME;
            parameters[name].push_back(value);
        }
    }
    else {
        if (value == "all" || value == "nr") {
            string name = "recurring";
            parameters[name].push_back(value);
        }
        else if (value == "asc" || value == "desc") {
            string name = "sort";
            parameters[name].push_back(value);
        }
        else {
            string name = SIMPLE_PARAM_NAME;
            parameters[name].push_back(value);
        }
    }
}

string Command::parse(const string & commandLine) {
    parameters.clear();

    // Find command name (before first space)
    auto firstSpace = commandLine.find(' ');

    if (firstSpace == string::npos) {
        // No parameters; the whole line is just the command
        return trim(commandLine);
    }

    string command = trim(commandLine.substr(0, firstSpace));
    string paramPart = commandLine.substr(firstSpace + 1);

    // Split parameter part on '|'
    stringstream ss(paramPart);
    string token;

    while (getline(ss, token, '|')) {
        token = trim(token);

        if (token.empty()) {
            continue;
        }

        // Split "name:value" on the first ':'
        auto colonPos = token.find(':');

        if (colonPos == string::npos) {
            /*
            ** Here we have detected that there is no ':' present, so it is
            ** likely just a 'simple' command with one parameter. Handle that
            ** in handleExceptions()...
            */
            handleExceptions(command, token);
            continue;
        }

        string name  = trim(token.substr(0, colonPos));
        string value = trim(token.substr(colonPos + 1));

        if (!name.empty()) {
            parameters[name].push_back(value);
        }
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

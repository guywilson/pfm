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

#include <httpserver.hpp>

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
#include "custom_modifiers.h"
#include "system.h"


bool Command::isStringNumeric(const std::string & s) {
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
    std::cout << "For more detailed help, please see the manual" << std::endl;
    std::cout << "e.g. man pfm" << std::endl << std::endl;

    std::cout << "Commands supported:" << std::endl << std::endl;

    for (const auto &entry : commandTable) {
        if (entry.helpText.length() == 0) {
            continue;
        }
        
        std::cout << set_style(TextStyle::Bold, Colour::Magenta) << entry.name << set_style(TextStyle::Reset);

        if (entry.aliases.empty()) {
            std::cout << std::endl;
        }
        else {
            std::cout << " [ ";

            size_t i = 0;
            for (const auto & alias : entry.aliases) {
                std::cout << alias;

                if (i < entry.aliases.size() - 1) {
                    std::cout << ", ";
                }

                i++;
            }

            std::cout << " ]" << std::endl;
        }

        if (entry.helpText.length() > 0) {
            std::cout << entry.helpText << std::endl;
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;
}

void Command::version() {
    std::cout << "PFM version '" << getVersion() << "' - built [" << getBuildDate() << "]" << std::endl << std::endl;
}

void Command::handleExceptions(const std::string & command, const std::string & token) {
    std::string value = trim(token);

    if (isStringNumeric(token)) {
        if (command == "list") {
            std::string name = "rows";
            parameters[name].push_back(value);
        }
        else {
            std::string name = SEQUENCE_PARAM_NAME;
            parameters[name].push_back(value);
        }
    }
    else {
        if (value == "all" || value == "nr" || value == "rc") {
            std::string name = "recurring";
            parameters[name].push_back(value);
        }
        else if (value == "period" || value == "any") {
            std::string name = "timeframe";
            parameters[name].push_back(value);
        }
        else if (value == "asc" || value == "desc") {
            std::string name = "sort";
            parameters[name].push_back(value);
        }
        else if (value == "total") {
            std::string name = "total";
            parameters[name].push_back(value);
        }
        else {
            std::string name = SIMPLE_PARAM_NAME;
            parameters[name].push_back(value);
        }
    }
}

std::string Command::parse(const std::string & commandLine) {
    parameters.clear();

    // Find command name (before first space)
    auto firstSpace = commandLine.find(' ');

    if (firstSpace == std::string::npos) {
        // No parameters; the whole line is just the command
        return trim(commandLine);
    }

    std::string command   = trim(commandLine.substr(0, firstSpace));
    std::string paramPart = commandLine.substr(firstSpace + 1);

    // Tokenize parameter part:
    // - split on whitespace when we are NOT inside quotes
    // - keep everything (including spaces) between quotes as part of one token
    std::vector<std::string> tokens;
    bool inQuotes = false;
    std::string current;

    for (char ch : paramPart) {
        if (ch == '"') {
            // Toggle quoted state, but don't include the quote itself
            inQuotes = !inQuotes;
            continue;
        }

        if (!inQuotes && (isspace(static_cast<unsigned char>(ch)))) {
            // Separator outside quotes
            if (!current.empty()) {
                tokens.push_back(trim(current));
                current.clear();
            }
        }
        else {
            current.push_back(ch);
        }
    }

    // Flush last token
    if (!current.empty()) {
        tokens.push_back(trim(current));
    }

    // Process tokens as name:value or delegated to handleExceptions
    for (const auto & rawToken : tokens) {
        std::string token = trim(rawToken);
        if (token.empty()) {
            continue;
        }

        // Split "name:value" on the first ':'
        auto colonPos = token.find(':');

        if (colonPos == std::string::npos) {
            /*
            ** No ':' present: treat as a 'simple' parameter and let
            ** handleExceptions() interpret it according to the command.
            */
            handleExceptions(command, token);
            continue;
        }

        std::string name  = trim(token.substr(0, colonPos));
        std::string value = trim(token.substr(colonPos + 1));

        if (!name.empty()) {
            parameters[name].push_back(value);
        }
    }

    return command;
}

std::string Command::parse(const httpserver::http_request & request) {
    std::string_view sessionKey = request.get_header("X-Session-ID");

    SessionManager session;

    if (!session.isValid(sessionKey)) {
        throw pfm_validation_error("Invalid session supplied");
    }

    httpserver::http::header_view_map headers = request.get_headers();

    for (auto & header : headers) {
        if (header.first != "command-name" && header.first != "X-Session-ID") {
            parameters[header.first.data()].push_back(header.second.data());
        }
    }

    return request.get_header("command-name").data();
}

bool Command::process(const std::string & commandLine) {
    clear_history();

    for (std::string & cmd : commandHistory) {
        add_history(cmd.c_str());
    }

    std::string command = parse(commandLine);

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

                for (std::string & command : commandHistory) {
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

bool Command::process(const httpserver::http_request & request) {
    std::string command = parse(request);

    if (command.compare("version") == 0) {
        Command::version();
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

                return true;
            }
        }
    }

    return false;
}

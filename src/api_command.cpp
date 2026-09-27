/*
 * Copyright (C) 2025-2026 Guy Wilson
 *
 * This file is part of PFM.
 *
 * PFM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PFM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PFM. If not, see <https://www.gnu.org/licenses/>.
 */
#include <string>
#include <set>

#include <httpserver.hpp>
#include <nlohmann/json.hpp>

#include "command.h"
#include "pfm_error.h"
#include "logger.h"
#include "api.h"

#ifdef _COMPILE_TESTING_API_
using namespace httpserver;

const std::set<std::string> allowedCommands = {
    "add-account",
    "delete_account",
    "add-public-holiday",
    "add-category",
    "add-payee",
    "add-recurring-charge",
    "delete-recurring-charge",
    "add-transaction", 
    "delete-transaction",
    "reconcile-transaction",
    "transfer-transaction"
};

http_response API::handleRunCommand(const http_request & request) {
    Logger & log = Logger::getInstance();

    log.entry("API::handleRunCommand()");

    log.debug("API::handleRunCommand() - received request body:");
    log.debug("%s", request.get_content().data());

    json js = json::parse(request.get_content().data());

    std::string cmd;
    if (js.contains("command")) {
        cmd = js["command"].get<std::string>();

        if (!allowedCommands.contains(cmd)) {
            log.error("Command '%s' is not allowed in this context", cmd.c_str());
            log.exit("API::handleRunCommand()");
            return httpserver::http_response::string("Command not allowed");
        }
    }
    if (js.contains("parms")) {
        cmd += " " + js["parms"].get<std::string>();
    }

    Command command;
    command.process(cmd);

    log.exit("API::handleRunCommand()");

    return httpserver::http_response::string("OK");
}
#endif

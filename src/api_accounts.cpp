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

#include <httpserver.hpp>
#include <nlohmann/json.hpp>

#include "db_account.h"
#include "db_primary_account.h"
#include "pfm_error.h"
#include "logger.h"
#include "cfgmgr.h"
#include "api.h"

using namespace httpserver;

http_response API::handleListAccounts(const http_request & request) {
    Logger & log = Logger::getInstance();
    cfgmgr & cfg = cfgmgr::getInstance();

    log.entry("API::handleListAccounts()");

    log.debug("API::handleListAccounts() - received request body:");
    log.debug("%s", request.get_content().data());

    bool obfuscateNameField = cfg.getValueAsBoolean("server.obfuscate");

    DBResult<DBAccount> results;
    results.retrieveAll();

    auto jsonEntities = json::array();

    for (size_t i = 0;i < results.size();i++) {
        DBAccount account = results.at(i);

        if (obfuscateNameField) {
            account.name = "*****";
        }
        
        json j = account.getJson();

        jsonEntities.push_back(j);
    }

    json entity;
    entity["accounts"] = {jsonEntities};

    log.exit("API::handleListAccounts()");

    return httpserver::http_response::string(entity.dump());
}

#ifdef _COMPILE_TESTING_API_
http_response API::handleAddAccount(const http_request & request) {
    Logger & log = Logger::getInstance();

    log.entry("API::handleAddAccount()");

    log.debug("API::handleAddAccount() - received request body:");
    log.debug("%s", request.get_content().data());

    json js = json::parse(request.get_content().data());

    DBAccount account;

    if (js.contains("code")) {
        account.code = js["code"].get<std::string>();
    }

    if (js.contains("name")) {
        account.name = js["name"].get<std::string>();
    }

    if (js.contains("openingDate")) {
        account.openingDate = js["openingDate"].get<std::string>();
    }

    if (js.contains("openingBalance")) {
        account.openingBalance = js["openingBalance"].get<std::string>();
    }

    if (js.contains("balanceLimit")) {
        account.balanceLimit = js["balanceLimit"].get<std::string>();
    }

    account.save();

    DBResult<DBAccount> accounts;
    if (accounts.retrieveAll() == 1) {
        DBPrimaryAccount primaryAccount;
        primaryAccount.removeAll();
        primaryAccount.code = account.code;
        primaryAccount.save();
    }

    DBAccount savedEntity;
    savedEntity.retrieve(account.id);

    json j = savedEntity.getJson();

    log.exit("API::handleAddAccount()");

    return httpserver::http_response::string(j.dump());
}

http_response API::handleDeleteAccount(const http_request & request) {
    Logger & log = Logger::getInstance();

    log.entry("API::handleDeleteAccount()");

    log.debug("API::handleDeleteAccount() - received request body:");
    log.debug("%s", request.get_content().data());

    json js = json::parse(request.get_content().data());

    if (!js.contains("code")) {
        return httpserver::http_response::string("No account code supplied");
    }

    std::string code = js["code"].get<std::string>();

    DBAccount account;
    account.retrieveByCode(code);
    account.remove();

    log.exit("API::handleDeleteAccount()");

    return httpserver::http_response::string("OK");
}
#endif

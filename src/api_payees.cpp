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

#include "db_payee.h"
#include "pfm_error.h"
#include "logger.h"
#include "api.h"

using namespace httpserver;

http_response API::handleListPayees(const http_request & request) {
    Logger & log = Logger::getInstance();

    log.entry("API::handleListPayees()");

    log.debug("API::handleListPayees() - received request body:");
    log.debug("%s", request.get_content().data());

    DBResult<DBPayee> results;
    results.retrieveAll();

    auto jsonEntities = json::array();

    for (size_t i = 0;i < results.size();i++) {
        DBPayee payee = results.at(i);
        JRecord record = payee.getRecord();

        json j = json::object();
        object_t o = record.getObject();

        for (const auto& [key, value] : o) {
            j[key] = value;
        }

        jsonEntities.push_back(j);
    }

    json entity;
    entity["payees"] = {jsonEntities};

    log.exit("API::handleListPayees()");

    return httpserver::http_response::string(entity.dump());
}

#ifdef _COMPILE_TESTING_API_
http_response API::handleAddPayee(const http_request & request) {
    Logger & log = Logger::getInstance();

    log.entry("API::handleAddPayee()");

    log.debug("API::handleAddPayee() - received request body:");
    log.debug("%s", request.get_content().data());

    json js = json::parse(request.get_content().data());

    DBPayee payee;

    if (js.contains("code")) {
        payee.code = js["code"].get<std::string>();
    }

    if (js.contains("name")) {
        payee.name = js["name"].get<std::string>();
    }

    payee.save();

    DBPayee savedEntity;
    savedEntity.retrieve(payee.id);

    JRecord record = savedEntity.getRecord();

    json j = json::object();
    object_t o = record.getObject();

    for (const auto& [key, value] : o) {
        j[key] = value;
    }

    log.exit("API::handleAddPayee()");

    return httpserver::http_response::string(j.dump());
}
#endif

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

#include "db_category.h"
#include "pfm_error.h"
#include "logger.h"
#include "api.h"

using namespace httpserver;

http_response API::handleListCategories(const http_request & request) {
    Logger & log = Logger::getInstance();

    log.entry("API::handleListCategories()");

    log.debug("API::handleListCategories() - received request body:");
    log.debug("%s", request.get_content().data());

    DBResult<DBCategory> results;
    results.retrieveAll();

    auto jsonEntities = json::array();

    for (size_t i = 0;i < results.size();i++) {
        DBCategory category = results.at(i);
        json j = category.getJson();

        jsonEntities.push_back(j);
    }

    json entity;
    entity["categories"] = {jsonEntities};

    log.exit("API::handleListCategories()");

    return httpserver::http_response::string(entity.dump());
}

#ifdef _COMPILE_TESTING_API_
http_response API::handleAddCategory(const http_request & request) {
    Logger & log = Logger::getInstance();

    log.entry("API::handleAddCategory()");

    log.debug("API::handleAddCategory() - received request body:");
    log.debug("%s", request.get_content().data());

    json js = json::parse(request.get_content().data());

    DBCategory category;

    if (js.contains("code")) {
        category.code = js["code"].get<std::string>();
    }

    if (js.contains("description")) {
        category.description = js["description"].get<std::string>();
    }

    category.save();

    DBCategory savedEntity;
    savedEntity.retrieve(category.id);

    json j = savedEntity.getJson();

    log.exit("API::handleAddCategory()");

    return httpserver::http_response::string(j.dump());
}
#endif

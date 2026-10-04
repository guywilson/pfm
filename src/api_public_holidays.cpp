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

#include "db_public_holiday.h"
#include "logger.h"
#include "api.h"

using namespace httpserver;

#ifdef _COMPILE_TESTING_API_
http_response API::handleAddHoliday(const http_request & request) {
    Logger & log = Logger::getInstance();

    log.entry("API::handleAddHoliday()");

    log.debug("API::handleAddHoliday() - received request body:");
    log.debug("%s", request.get_content().data());

    json js = json::parse(request.get_content().data());

    DBPublicHoliday holiday;

    if (js.contains("date")) {
        holiday.date = js["date"].get<std::string>();
    }

    if (js.contains("description")) {
        holiday.description = js["description"].get<std::string>();
    }

    holiday.save();

    DBPublicHoliday savedEntity;
    savedEntity.retrieve(holiday.id);

    clearPublicHolidays();
    DBPublicHoliday::populatePublicHolidays();

    json j = savedEntity.getJson();

    log.exit("API::handleAddHoliday()");

    return httpserver::http_response::string(j.dump());
}
#endif

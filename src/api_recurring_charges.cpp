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

#include "db_v_recurring_charge.h"
#include "db_recurring_charge.h"
#include "cache.h"
#include <cctype>
#include <cstdlib>
#include "pfm_error.h"
#include "logger.h"
#include "cfgmgr.h"
#include "api.h"

using namespace httpserver;

http_response API::handleListRecurringCharges(const http_request & request) {
    Logger & log = Logger::getInstance();
    cfgmgr & cfg = cfgmgr::getInstance();

    log.entry("API::handleListRecurringCharges()");

    log.debug("API::handleListRecurringCharges() - received request body:");
    log.debug("%s", request.get_content().data());

    bool obfuscateDescriptionField = cfg.getValueAsBoolean("server.obfuscate");

    DBResult<DBRecurringChargeView> results;
    results.retrieveAll();

    auto jsonEntities = json::array();

    for (size_t i = 0;i < results.size();i++) {
        DBRecurringChargeView charge = results.at(i);

        if (obfuscateDescriptionField) {
            charge.description = "*****";
        }
        
        json j = charge.getJson();

        jsonEntities.push_back(j);
    }

    json entity;
    entity["charges"] = {jsonEntities};

    log.exit("API::handleListRecurringCharges()");

    return httpserver::http_response::string(entity.dump());
}

#ifdef _COMPILE_TESTING_API_
http_response API::handleAddRecurringCharge(const http_request & request) {
    Logger & log = Logger::getInstance();

    log.entry("API::handleAddRecurringCharge()");

    log.debug("API::handleAddRecurringCharge() - received request body:");
    log.debug("%s", request.get_content().data());

    json js = json::parse(request.get_content().data());

    DBRecurringCharge charge;

    if (js.contains("account")) {
        std::string code = js["account"].get<std::string>();

        DBAccount entity;
        entity.retrieveByCode(code);

        charge.accountId = entity.id;
    }

    if (js.contains("category")) {
        std::string code = js["category"].get<std::string>();

        DBCategory entity;
        entity.retrieveByCode(code);

        charge.categoryId = entity.id;
    }

    if (js.contains("payee")) {
        std::string code = js["payee"].get<std::string>();

        DBPayee entity;
        entity.retrieveByCode(code);

        charge.payeeId = entity.id;
    }

    if (js.contains("date")) {
        charge.date = js["date"].get<std::string>();
    }

    if (js.contains("endDate")) {
        charge.endDate = js["endDate"].get<std::string>();
    }

    if (js.contains("lastPaymentDate")) {
        charge.lastPaymentDate = js["lastPaymentDate"].get<std::string>();
    }

    if (js.contains("description")) {
        charge.description = js["description"].get<std::string>();
    }

    if (js.contains("amount")) {
        charge.amount = js["amount"].get<std::string>();
    }

    if (js.contains("frequency")) {
        charge.frequency = Frequency::parse(js["frequency"].get<std::string>());
    }

    if (js.contains("isTransfer")) {
        std::string isTransfer = js["isTransfer"].get<std::string>();
        charge.isTransfer = (isTransfer == "Y" || isTransfer == "Yes" || isTransfer == "yes");
    }

    if (js.contains("transferTo")) {
        std::string accountCode = js["transferTo"].get<std::string>();
        charge.setTransferToAccount(accountCode);
    }

    charge.save();

    DBRecurringCharge savedEntity;
    savedEntity.retrieve(charge.id);

    json j = savedEntity.getJson();

    log.exit("API::handleAddRecurringCharge()");

    return httpserver::http_response::string(j.dump());
}

http_response API::handleDeleteRecurringCharge(const http_request & request) {
    Logger & log = Logger::getInstance();

    log.entry("API::handleDeleteRecurringCharge()");

    log.debug("API::handleDeleteRecurringCharge() - received request body:");
    log.debug("%s", request.get_content().data());

    json js = json::parse(request.get_content().data());

    if (!js.contains("sequence")) {
        return httpserver::http_response::string("No sequence supplied");
    }

    std::string sequenceStr = js["sequence"].get<std::string>();
    if (sequenceStr.empty()) {
        return httpserver::http_response::string("Non-numeric sequence supplied");
    }

    for (unsigned char c : sequenceStr) {
        if (!std::isdigit(c)) {
            return httpserver::http_response::string("Non-numeric sequence supplied");
        }
    }

    CacheMgr & cache = CacheMgr::getInstance();
    DBRecurringCharge charge = cache.getRecurringCharge(std::atoi(sequenceStr.c_str()));
    charge.remove();

    log.exit("API::handleDeleteRecurringCharge()");

    return httpserver::http_response::string("OK");
}
#endif

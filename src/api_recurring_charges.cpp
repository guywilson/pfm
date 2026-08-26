#include <string>

#include <httpserver.hpp>
#include <nlohmann/json.hpp>

#include "db_v_recurring_charge.h"
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
        
        JRecord record = charge.getRecord();

        json j = json::object();
        object_t o = record.getObject();

        for (const auto& [key, value] : o) {
            j[key] = value;
        }

        jsonEntities.push_back(j);
    }

    json entity;
    entity["charges"] = {jsonEntities};

    log.exit("API::handleListRecurringCharges()");

    return httpserver::http_response::string(entity.dump());
}

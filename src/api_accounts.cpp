#include <string>

#include <httpserver.hpp>
#include <nlohmann/json.hpp>

#include "db_account.h"
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
        
        JRecord record = account.getRecord();

        json j = json::object();
        object_t o = record.getObject();

        for (const auto& [key, value] : o) {
            j[key] = value;
        }

        jsonEntities.push_back(j);
    }

    json entity;
    entity["accounts"] = {jsonEntities};

    log.exit("API::handleListAccounts()");

    return httpserver::http_response::string(entity.dump());
}

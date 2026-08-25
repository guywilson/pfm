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

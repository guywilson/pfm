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
        JRecord record = category.getRecord();

        json j = json::object();
        object_t o = record.getObject();

        for (const auto& [key, value] : o) {
            j[key] = value;
        }

        jsonEntities.push_back(j);
    }

    json entity;
    entity["categories"] = {jsonEntities};

    log.exit("API::handleListCategories()");

    return httpserver::http_response::string(entity.dump());
}

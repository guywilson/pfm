#include <string>

#include <httpserver.hpp>

#include "logger.h"
#include "posixthread.h"
#include "web_api.h"

void * APIListener::run() {
    Logger & log = Logger::getInstance();

    log.entry("APIListener::run()");

    httpserver::webserver ws{httpserver::create_webserver(8080)};

    this->ws = &ws;

    ws.on_get("/api/transaction/list-transactions", [](const httpserver::http_request&) {
        return httpserver::http_response::string("OK");
    });

    log.debug("Starting web API server...");

    ws.start(true);

    log.exit("APIListener::run()");

    return NULL;
}

#include <string>

#include <stdio.h>

#include <httpserver.hpp>
#include <nlohmann/json.hpp>

#include "db_account.h"
#include "db_v_transaction.h"
#include "logger.h"
#include "cfgmgr.h"
#include "command.h"
#include "system.h"
#include "posixthread.h"
#include "web_api.h"
#include "api.h"

static SessionManager session;

void APIListener::validateSession(const httpserver::http_request & request) {
    Logger & log = Logger::getInstance();

    log.entry("APIListener::validateSession");

    std::string_view sessionKey = request.get_header("X-Session-ID");

    if (!session.isValid(sessionKey)) {
        log.error("Got invalid session ID!");
        throw pfm_validation_error("Invalid session supplied");
    }

    log.exit("APIListener::validateSession");
}

void APIListener::registerEndPoints(httpserver::webserver & ws) {
    Logger & log = Logger::getInstance();

    log.entry("APIListener::registerEndPoints()");

    ws.on_get("/api/account/list", [](const httpserver::http_request & request) {
        APIListener::validateSession(request);
        return API::handleListAccounts(request);
    });
    ws.on_post("/api/transaction/find", [](const httpserver::http_request & request) {
        APIListener::validateSession(request);
        return API::handleFindTransactions(request);
    });
    ws.on_post("/api/transaction/list", [](const httpserver::http_request & request) {
        APIListener::validateSession(request);
        return API::handleListTransactions(request);
    });
    ws.on_get("/api/category/list", [](const httpserver::http_request & request) {
        APIListener::validateSession(request);
        return API::handleListCategories(request);
    });
    ws.on_get("/api/payee/list", [](const httpserver::http_request & request) {
        APIListener::validateSession(request);
        return API::handleListPayees(request);
    });
    ws.on_get("/api/charge/list", [](const httpserver::http_request & request) {
        APIListener::validateSession(request);
        return API::handleListRecurringCharges(request);
    });

    log.exit("APIListener::registerEndPoints()");
}

void * APIListener::run() {
    Logger & log = Logger::getInstance();

    log.entry("APIListener::run()");

    cfgmgr & cfg = cfgmgr::getInstance();

    uint16_t port = (uint16_t)cfg.getValueAsInteger("server.port");

    std::string tls_key_path = cfg.getValue("server.key");
    std::string tls_cert_path = cfg.getValue("server.cert");

    httpserver::webserver ws{
        httpserver::create_webserver(port)
            .put_processed_data_to_content()
            .use_ssl()
            .https_mem_key(tls_key_path)
            .https_mem_cert(tls_cert_path)
            .https_priorities(
                "NORMAL:-VERS-TLS-ALL"
                ":+VERS-TLS1.2:+VERS-TLS1.3"
                ":%SAFE_RENEGOTIATION")
    };

    registerEndPoints(ws);

    std::cout << "Session ID: " << session.createSession() << std::endl;
    fflush(stdout);

    log.debug("Starting web API server on port %u...", (unsigned int)port);

    ws.start(true);

    log.exit("APIListener::run()");

    return NULL;
}

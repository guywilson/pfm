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

static SessionManager session;

void APIListener::validateSession(const httpserver::http_request & request) {
    Logger & log = Logger::getInstance();

    log.entry("APIListener::validateSession");

    string_view sessionKey = request.get_header("X-Session-ID");

    if (!session.isValid(sessionKey)) {
        log.error("Got invalid session ID!");
        throw pfm_validation_error("Invalid session supplied");
    }

    log.exit("APIListener::validateSession");
}

httpserver::http_response APIListener::handleFindTransactions(const httpserver::http_request & request) {
    Logger & log = Logger::getInstance();

    log.entry("APIListener::handleFindTransactions()");

    log.debug("APIListener::handleFindTransactions() - received request body:");
    log.debug("%s", request.get_content().data());

    json js = json::parse(request.get_content().data());

    DBCriteria criteria;

    if (js.contains("account")) {
        std::string code = js["account"].get<std::string>();
        criteria = DBTransactionView::FindCriteriaHelper::handleWithTheseAccounts(criteria, {code});
    }
    if (js.contains("category")) {
        std::string code = js["category"].get<std::string>();
        criteria = DBTransactionView::FindCriteriaHelper::handleWithTheseCategories(criteria, {code});
    }
    if (js.contains("payee")) {
        std::string code = js["payee"].get<std::string>();
        criteria = DBTransactionView::FindCriteriaHelper::handleWithTheseCategories(criteria, {code});
    }
    if (js.contains("description")) {
        std::string description = js["description"].get<std::string>();
        criteria = DBTransactionView::FindCriteriaHelper::handleWithThisDescription(criteria, description);
    }
    if (js.contains("reference")) {
        std::string reference = js["reference"].get<std::string>();
        criteria = DBTransactionView::FindCriteriaHelper::handleWithThisReference(criteria, reference);
    }
    if (js.contains("type")) {
        std::string type = js["type"].get<std::string>();
        criteria = DBTransactionView::FindCriteriaHelper::handleWithThisType(criteria, type);
    }
    if (js.contains("date")) {
        std::string date = js["date"].get<std::string>();
        criteria = DBTransactionView::FindCriteriaHelper::handleOnTheseDates(criteria, {date});
    }
    if (js.contains("reconciled")) {
        bool isReconciled = js["reconciled"].get<bool>();
        criteria = DBTransactionView::FindCriteriaHelper::handleIsRecconciled(criteria, isReconciled);
    }
    if (js.contains("recurring")) {
        bool isRecurring = js["recurring"].get<bool>();
        criteria = DBTransactionView::FindCriteriaHelper::handleIsRecurring(criteria, isRecurring);
    }
    if (js.contains("after")) {
        std::string date = js["after"].get<std::string>();
        criteria = DBTransactionView::FindCriteriaHelper::handleGreaterThanThisDate(criteria, date);
    }
    if (js.contains("before")) {
        std::string date = js["before"].get<std::string>();
        criteria = DBTransactionView::FindCriteriaHelper::handleLessThanThisDate(criteria, date);
    }
    if (js.contains("gt")) {
        std::string amount = js["gt"].get<std::string>();
        criteria = DBTransactionView::FindCriteriaHelper::handleGreaterThanThisAmount(criteria, amount);
    }
    if (js.contains("lt")) {
        std::string amount = js["lt"].get<std::string>();
        criteria = DBTransactionView::FindCriteriaHelper::handleLessThanThisAmount(criteria, amount);
    }

    DBTransactionView view;
    DBResult<DBTransactionView> results = view.findTransactionsForCriteria(criteria);

    auto jsonEntities = json::array();

    for (size_t i = 0;i < results.size();i++) {
        DBTransactionView transaction = results.at(i);
        JRecord record = transaction.getRecord();

        json j = json::object();
        object_t o = record.getObject();

        for (const auto& [key, value] : o) {
            j[key] = value;
        }

        jsonEntities.push_back(j);
    }

    json entity;
    entity["transactions"] = {jsonEntities};

    log.exit("APIListener::handleFindTransactions()");

    return httpserver::http_response::string(entity.dump());
}

httpserver::http_response APIListener::handleAddTransaction(const httpserver::http_request & request) {
    Logger & log = Logger::getInstance();

    log.entry("APIListener::handleAddTransaction()");

    log.debug("APIListener::handleAddTransaction() - received request body:");
    log.debug("%s", request.get_content().data());

    json js = json::parse(request.get_content().data());

    DBTransaction transaction;

    if (js.contains("account")) {
        std::string code = js["account"].get<std::string>();

        DBAccount account;
        account.retrieveByCode(code);

        transaction.accountId = account.id;
    }
    if (js.contains("category")) {
        std::string code = js["category"].get<std::string>();

        DBCategory category;
        category.retrieveByCode(code);

        transaction.categoryId = category.id;
    }
    if (js.contains("payee")) {
        std::string code = js["payee"].get<std::string>();

        DBPayee payee;
        payee.retrieveByCode(code);

        transaction.payeeId = payee.id;
    }
    if (js.contains("description")) {
        std::string description = js["description"].get<std::string>();
        transaction.description = description;
    }
    if (js.contains("reference")) {
        std::string reference = js["reference"].get<std::string>();
        transaction.reference = reference;
    }
    if (js.contains("type")) {
        std::string type = js["type"].get<std::string>();
        transaction.type = type;
    }
    if (js.contains("date")) {
        std::string date = js["date"].get<std::string>();
        transaction.date = date;
    }
    if (js.contains("reconciled")) {
        bool isReconciled = js["reconciled"].get<bool>();
        transaction.isReconciled = isReconciled;
    }
    if (js.contains("amount")) {
        std::string amount = js["amount"].get<std::string>();
        transaction.amount = amount;
    }

    transaction.save();

    log.exit("APIListener::handleAddTransaction()");

    return httpserver::http_response::string("OK");
}

void APIListener::registerEndPoints(httpserver::webserver & ws) {
    Logger & log = Logger::getInstance();

    log.entry("APIListener::registerEndPoints()");

    ws.on_post("/api/transaction/find-transactions", [](const httpserver::http_request & request) {
        APIListener::validateSession(request);
        return APIListener::handleFindTransactions(request);
    });
    ws.on_post("/api/transaction/add-transaction", [](const httpserver::http_request & request) {
        APIListener::validateSession(request);
        return APIListener::handleAddTransaction(request);
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

    cout << "Session ID: " << session.createSession() << endl;
    fflush(stdout);

    log.debug("Starting web API server on port %u...", (unsigned int)port);

    ws.start(true);

    log.exit("APIListener::run()");

    return NULL;
}

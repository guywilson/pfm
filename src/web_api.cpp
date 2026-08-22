#include <string>

#include <httpserver.hpp>
#include <nlohmann/json.hpp>

#include "db_account.h"
#include "db_v_transaction.h"
#include "logger.h"
#include "cfgmgr.h"
#include "command.h"
#include "posixthread.h"
#include "web_api.h"

httpserver::http_response APIListener::handleFindTransactions(const httpserver::http_request & request) {
    Logger & log = Logger::getInstance();

    log.entry("APIListener::handleFindTransactions()");

    DBCriteria criteria;

    if (request.get_header("account").length() > 0) {
        std::string code = request.get_header("account").data();
        criteria = DBTransactionView::FindCriteriaHelper::handleWithTheseAccounts(criteria, {code});
    }
    if (request.get_header("category").length() > 0) {
        std::string code = request.get_header("category").data();
        criteria = DBTransactionView::FindCriteriaHelper::handleWithTheseCategories(criteria, {code});
    }
    if (request.get_header("payee").length() > 0) {
        std::string code = request.get_header("payee").data();
        criteria = DBTransactionView::FindCriteriaHelper::handleWithThesePayees(criteria, {code});
    }
    if (request.get_header("description").length() > 0) {
        std::string description = request.get_header("description").data();
        criteria = DBTransactionView::FindCriteriaHelper::handleWithThisDescription(criteria, description);
    }
    if (request.get_header("reference").length() > 0) {
        std::string reference = request.get_header("reference").data();
        criteria = DBTransactionView::FindCriteriaHelper::handleWithThisReference(criteria, reference);
    }
    if (request.get_header("type").length() > 0) {
        std::string type = request.get_header("type").data();
        criteria = DBTransactionView::FindCriteriaHelper::handleWithThisType(criteria, type);
    }
    if (request.get_header("date").length() > 0) {
        std::string date = request.get_header("date").data();
        criteria = DBTransactionView::FindCriteriaHelper::handleOnTheseDates(criteria, {date});
    }
    if (request.get_header("reconciled").length() > 0) {
        bool isReconciled = request.get_header("reconciled").data();
        criteria = DBTransactionView::FindCriteriaHelper::handleIsRecconciled(criteria, isReconciled);
    }
    if (request.get_header("recurring").length() > 0) {
        bool isRecurring = request.get_header("recurring").data();
        criteria = DBTransactionView::FindCriteriaHelper::handleIsRecurring(criteria, isRecurring);
    }
    if (request.get_header("after").length() > 0) {
        std::string date = request.get_header("after").data();
        criteria = DBTransactionView::FindCriteriaHelper::handleGreaterThanThisDate(criteria, date);
    }
    if (request.get_header("before").length() > 0) {
        std::string date = request.get_header("before").data();
        criteria = DBTransactionView::FindCriteriaHelper::handleLessThanThisDate(criteria, date);
    }
    if (request.get_header("gt").length() > 0) {
        std::string amount = request.get_header("gt").data();
        criteria = DBTransactionView::FindCriteriaHelper::handleGreaterThanThisAmount(criteria, amount);
    }
    if (request.get_header("lt").length() > 0) {
        std::string amount = request.get_header("lt").data();
        criteria = DBTransactionView::FindCriteriaHelper::handleLessThanThisAmount(criteria, amount);
    }

    DBTransactionView view;
    DBResult<DBTransactionView> results = view.findTransactionsForCriteria(criteria);

    auto jsonEntities = json::array();

    for (size_t i = 0;i < results.size();i++) {
        DBTransaction transaction = results.at(i);
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

void APIListener::registerEndPoints(httpserver::webserver & ws) {
    Logger & log = Logger::getInstance();

    log.entry("APIListener::registerEndPoints()");

    ws.on_get("/api/transaction/find-transactions", [](const httpserver::http_request & request) {
        return APIListener::handleFindTransactions(request);
    });

    log.exit("APIListener::registerEndPoints()");
}

void * APIListener::run() {
    Logger & log = Logger::getInstance();

    log.entry("APIListener::run()");

    cfgmgr & cfg = cfgmgr::getInstance();
    uint16_t port = (uint16_t)cfg.getValueAsInteger("server.port");

    httpserver::webserver ws{httpserver::create_webserver(port)};

    registerEndPoints(ws);

    log.debug("Starting web API server on port %u...", (unsigned int)port);

    ws.start(true);

    log.exit("APIListener::run()");

    return NULL;
}

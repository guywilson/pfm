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

    ws.on_post("/api/transaction/find-transactions", [](const httpserver::http_request & request) {
        return APIListener::handleFindTransactions(request);
    });

    log.exit("APIListener::registerEndPoints()");
}

void * APIListener::run() {
    Logger & log = Logger::getInstance();

    log.entry("APIListener::run()");

    cfgmgr & cfg = cfgmgr::getInstance();
    uint16_t port = (uint16_t)cfg.getValueAsInteger("server.port");

    httpserver::webserver ws{httpserver::create_webserver(port).put_processed_data_to_content(true)};

    registerEndPoints(ws);

    log.debug("Starting web API server on port %u...", (unsigned int)port);

    ws.start(true);

    log.exit("APIListener::run()");

    return NULL;
}

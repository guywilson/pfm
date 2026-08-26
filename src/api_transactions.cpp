#include <string>

#include <httpserver.hpp>
#include <nlohmann/json.hpp>

#include "db_account.h"
#include "db_v_transaction.h"
#include "pfm_error.h"
#include "logger.h"
#include "cfgmgr.h"
#include "api.h"

using namespace httpserver;

http_response API::handleFindTransactions(const httpserver::http_request & request) {
    Logger & log = Logger::getInstance();
    cfgmgr & cfg = cfgmgr::getInstance();

    log.entry("API::handleFindTransactions()");

    log.debug("API::handleFindTransactions() - received request body:");
    log.debug("%s", request.get_content().data());

    bool obfuscateDescriptionField = cfg.getValueAsBoolean("server.obfuscate");

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

        if (obfuscateDescriptionField) {
            transaction.description = "*****";
        }

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

    log.exit("API::handleFindTransactions()");

    return httpserver::http_response::string(entity.dump());
}

http_response API::handleListTransactions(const httpserver::http_request & request) {
    Logger & log = Logger::getInstance();
    cfgmgr & cfg = cfgmgr::getInstance();

    log.entry("API::handleListTransactions()");

    log.debug("API::handleListTransactions() - received request body:");
    log.debug("%s", request.get_content().data());

    bool obfuscateDescriptionField = cfg.getValueAsBoolean("server.obfuscate");

    json js = json::parse(request.get_content().data());

    pfm_id_t accountId = 0;
    DBCriteria::sql_order order = DBCriteria::descending;
    DBTransactionView::recurring_type type = DBTransactionView::recurring_type::all;
    bool isThisPeriod = false;
    int rowLimit = 20;

    if (js.contains("account")) {
        std::string code = js["account"].get<std::string>();

        DBAccount account;
        account.retrieveByCode(code);

        accountId = account.id;
    }
    if (js.contains("recurring-type")) {
        std::string recurringType = js["recurring-type"].get<std::string>();

        if (recurringType == "non-recurring") {
            type = DBTransactionView::recurring_type::non_recurring;
        }
        else if (recurringType == "recurring-only") {
            type = DBTransactionView::recurring_type::recurring_only;
        }
        else if (recurringType == "all") {
            type = DBTransactionView::recurring_type::all;
        }
        else {
            return http_response::string("Invalid recurring-type supplied").with_status(500);
        }
    }
    if (js.contains("sort-dir")) {
        std::string orderDir = js["sort-dir"].get<std::string>();

        if (orderDir == "descending") {
            order = DBCriteria::sql_order::descending;
        }
        else if (orderDir == "ascending") {
            order = DBCriteria::sql_order::ascending;
        }
        else {
            return http_response::string("Invalid sort-dir supplied").with_status(500);
        }
    }
    if (js.contains("this-period")) {
        isThisPeriod = js["this-period"].get<bool>();
    }
    if (js.contains("row-limit")) {
        rowLimit = js["row-limit"].get<int>();
    }

    DBTransactionView view;
    DBResult<DBTransactionView> results = view.listByAccountID(accountId, type, isThisPeriod, order, rowLimit);

    auto jsonEntities = json::array();

    for (size_t i = 0;i < results.size();i++) {
        DBTransactionView transaction = results.at(i);

        if (obfuscateDescriptionField) {
            transaction.description = "*****";
        }

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

    log.exit("API::handleListTransactions()");

    return httpserver::http_response::string(entity.dump());
}

/*
 * Copyright (C) 2025-2026 Guy Wilson
 *
 * This file is part of PFM.
 *
 * PFM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PFM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PFM. If not, see <https://www.gnu.org/licenses/>.
 */

#include <string>

#include <httpserver.hpp>
#include <nlohmann/json.hpp>

#include "db_account.h"
#include "db_transaction.h"
#include "db_category.h"
#include "db_payee.h"
#include "db_v_transaction.h"
#include "db_v_carried_over.h"
#include "cache.h"
#include "pfm_error.h"
#include "logger.h"
#include "cfgmgr.h"
#include "api.h"

using namespace httpserver;

http_response API::handleFindTransactions(const httpserver::http_request & request) {
    Logger & log = Logger::getInstance();
    cfgmgr & cfg = cfgmgr::getInstance();
    CacheMgr & cache = CacheMgr::getInstance();

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

    cache.clearTransactions();

    auto jsonEntities = json::array();

    for (size_t i = 0;i < results.size();i++) {
        DBTransactionView transaction = results.at(i);

        cache.addTransaction(transaction.sequence, transaction);

        if (obfuscateDescriptionField) {
            transaction.description = "*****";
        }

        json j = transaction.getJson();

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
    CacheMgr & cache = CacheMgr::getInstance();

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

    cache.clearTransactions();

    auto jsonEntities = json::array();

    for (size_t i = 0;i < results.size();i++) {
        DBTransactionView transaction = results.at(i);

        cache.addTransaction(transaction.sequence, transaction);

        if (obfuscateDescriptionField) {
            transaction.description = "*****";
        }

        json j = transaction.getJson();

        jsonEntities.push_back(j);
    }

    json entity;
    entity["transactions"] = {jsonEntities};

    log.exit("API::handleListTransactions()");

    return httpserver::http_response::string(entity.dump());
}

http_response API::handleListCarriedOverLogs(const http_request & request) {
    Logger & log = Logger::getInstance();

    log.entry("API::handleListCarriedOverLogs()");

    log.debug("API::handleListCarriedOverLogs() - received request body:");
    log.debug("%s", request.get_content().data());

    json js = json::parse(request.get_content().data());

    std::string accountCode;
    if (js.contains("account")) {
        accountCode = js["account"].get<std::string>();
    }

    DBCarriedOverView view;
    DBResult<DBCarriedOverView> results = view.retrieveByAccountCode(accountCode);

    auto jsonEntities = json::array();

    for (size_t i = 0;i < results.size();i++) {
        DBCarriedOverView co = results.at(i);

        json j = co.getJson();

        jsonEntities.push_back(j);
    }

    json entity;
    entity["carriedOverLogs"] = {jsonEntities};

    log.exit("API::handleListCarriedOverLogs()");

    return httpserver::http_response::string(entity.dump());
}

#ifdef _COMPILE_TESTING_API_
http_response API::handleAddTransaction(const http_request & request) {
    Logger & log = Logger::getInstance();

    log.entry("API::handleAddTransaction()");

    log.debug("API::handleAddTransaction() - received request body:");
    log.debug("%s", request.get_content().data());

    json js = json::parse(request.get_content().data());

    DBTransaction transaction;

    if (js.contains("account")) {
        std::string accountCode = js["account"].get<std::string>();

        DBAccount account;
        account.retrieveByCode(accountCode);

        transaction.accountId = account.id;
    }

    if (js.contains("category")) {
        std::string categoryCode = js["category"].get<std::string>();

        DBCategory category;
        category.retrieveByCode(categoryCode);

        transaction.categoryId = category.id;
    }

    if (js.contains("payee")) {
        std::string payeeCode = js["payee"].get<std::string>();

        DBPayee payee;
        payee.retrieveByCode(payeeCode);

        transaction.payeeId = payee.id;
    }

    if (js.contains("date")) {
        transaction.date = js["date"].get<std::string>();
    }

    if (js.contains("description")) {
        transaction.description = js["description"].get<std::string>();
    }

    if (js.contains("isReconciled")) {
        transaction.isReconciled = js["isReconciled"].get<bool>();
    }

    if (js.contains("type")) {
        transaction.type = js["type"].get<std::string>();
    }

    if (js.contains("amount")) {
        transaction.amount = js["amount"].get<double>();
    }

    transaction.save();

    DBTransaction savedTransaction;
    savedTransaction.retrieve(transaction.id);

    json j = savedTransaction.getJson();

    log.exit("API::handleAddTransaction()");

    return httpserver::http_response::string(j.dump());
}

http_response API::handleDeleteTransaction(const http_request & request) {
    Logger & log = Logger::getInstance();
    CacheMgr & cache = CacheMgr::getInstance();

    log.entry("API::handleDeleteTransaction()");

    log.debug("API::handleDeleteTransaction() - received request body:");
    log.debug("%s", request.get_content().data());

    json js = json::parse(request.get_content().data());

    if (js.contains("sequence")) {
        std::string sequenceStr = js["sequence"].get<std::string>();

        for (char c : sequenceStr) {
            if (!isdigit(c)) {
                return httpserver::http_response::string("Non-numeric sequence supplied");
            }
        }

        DBTransaction transaction = cache.getTransaction(atoi(sequenceStr.c_str()));
        transaction.remove();
    }

    log.exit("API::handleDeleteTransaction()");

    return httpserver::http_response::string("OK");
}

http_response API::handleReconcileTransaction(const http_request & request) {
    Logger & log = Logger::getInstance();
    CacheMgr & cache = CacheMgr::getInstance();

    log.entry("API::handleReconcileTransaction()");

    log.debug("API::handleReconcileTransaction() - received request body:");
    log.debug("%s", request.get_content().data());

    json js = json::parse(request.get_content().data());

    std::string sequenceStr;
    if (js.contains("sequence")) {
        sequenceStr = js["sequence"].get<std::string>();

        for (char c : sequenceStr) {
            if (!isdigit(c)) {
                return httpserver::http_response::string("Non-numeric sequence supplied");
            }
        }
    }
    else {
        return httpserver::http_response::string("No sequence supplied");
    }

    DBTransaction transaction = cache.getTransaction(atoi(sequenceStr.c_str()));
    transaction.isReconciled = true;

    transaction.save();

    DBTransaction savedTransaction;
    savedTransaction.retrieve(transaction.id);

    json j = savedTransaction.getJson();

    log.exit("API::handleReconcileTransaction()");

    return httpserver::http_response::string(j.dump());
}

http_response API::handleTransferTransaction(const http_request & request) {
    Logger & log = Logger::getInstance();

    log.entry("API::handleTransferTransaction()");

    log.debug("API::handleTransferTransaction() - received request body:");
    log.debug("%s", request.get_content().data());

    json js = json::parse(request.get_content().data());

    DBTransaction transaction;
    DBAccount accountTo;

    if (js.contains("accountTo")) {
        std::string accountCode = js["accountTo"].get<std::string>();

        accountTo.retrieveByCode(accountCode);
    }

    if (js.contains("accountFrom")) {
        std::string accountCode = js["accountFrom"].get<std::string>();

        DBAccount accountFrom;
        accountFrom.retrieveByCode(accountCode);

        transaction.accountId = accountFrom.id;
    }

    if (js.contains("category")) {
        std::string categoryCode = js["category"].get<std::string>();

        DBCategory category;
        category.retrieveByCode(categoryCode);

        transaction.categoryId = category.id;
    }

    if (js.contains("date")) {
        transaction.date = js["date"].get<std::string>();
    }

    if (js.contains("description")) {
        transaction.description = js["description"].get<std::string>();
    }

    if (js.contains("isReconciled")) {
        std::string isReconciled = js["isReconciled"].get<std::string>();
        transaction.isReconciled = (isReconciled == "Y" || isReconciled == "Yes" || isReconciled == "yes");
    }

    if (js.contains("amount")) {
        transaction.amount = js["amount"].get<std::string>();
    }

    DBTransaction::createTransferPairFromSource(transaction, accountTo);

    log.exit("API::handleTransferTransaction()");

    return httpserver::http_response::string("OK");
}
#endif

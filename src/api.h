#pragma once

#include <httpserver.hpp>

using namespace httpserver;

/*
** The idea behind exposing an API is to allow an AI agent to query PFM in order
** to compare against a bank transaction export to find missing/additional transactions
** when things don't match up.
**
** Tempting though it is to surface endpoints to create, update and delete entites,
** this API is read-only for obvious security reasons.
*/
class API {
    public:
        static http_response handleListAccounts(const http_request & request);

        static http_response handleFindTransactions(const http_request & request);
        static http_response handleListTransactions(const http_request & request);

        static http_response handleListCategories(const http_request & request);
        static http_response handleListPayees(const http_request & request);

        static http_response handleListRecurringCharges(const http_request & request);
};

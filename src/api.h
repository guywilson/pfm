#pragma once

#include <httpserver.hpp>

using namespace httpserver;

/*
** Expose read-only endpoints with a REST API. All APIs require the X-Session-ID header to
** be populated with the session key displayed when the API server is started (with the
** start comand). The session key expires after 1 hour, restarting the server will
** invalidate any previous session keys and create a new one.
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

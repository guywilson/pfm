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

#pragma once

#include <httpserver.hpp>

// #define _COMPILE_TESTING_API_

using namespace httpserver;

/*
** Expose read-only endpoints with a REST API. All APIs require the X-Session-ID header to
** be populated with the session key displayed when the API server is started (with the
** start comand). The session key expires after 1 hour, restarting the server will
** invalidate any previous session keys and create a new one.
**
** Tempting though it is to surface endpoints to create, update and delete entites,
** this API is read-only by default for obvious security reasons.
**
** In order to run full end-to-end testing, the API can be compiled with _COMPILE_TESTING_API_
** which introduces certain add/delete APIs...
*/
class API {
    public:
        static http_response handleListAccounts(const http_request & request);

        static http_response handleFindTransactions(const http_request & request);
        static http_response handleListTransactions(const http_request & request);

        static http_response handleListCategories(const http_request & request);

        static http_response handleListPayees(const http_request & request);

        static http_response handleListRecurringCharges(const http_request & request);

        static http_response handleListCarriedOverLogs(const http_request & request);

#ifdef _COMPILE_TESTING_API_
        static http_response handleAddAccount(const http_request & request);
        static http_response handleDeleteAccount(const http_request & request);

        static http_response handleAddHoliday(const http_request & request);
        static http_response handleAddCategory(const http_request & request);
        static http_response handleAddPayee(const http_request & request);

        static http_response handleAddTransaction(const http_request & request);
        static http_response handleDeleteTransaction(const http_request & request);
        static http_response handleReconcileTransaction(const http_request & request);
        static http_response handleTransferTransaction(const http_request & request);

        static http_response handleAddRecurringCharge(const http_request & request);
        static http_response handleDeleteRecurringCharge(const http_request & request);
#endif
};

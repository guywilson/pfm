#pragma once

#include <httpserver.hpp>

#include "posixthread.h"

class APIListener : public PosixThread {
    private:
        void registerEndPoints(httpserver::webserver & ws);

        static httpserver::http_response handleFindTransactions(const httpserver::http_request & request);

    public:
        APIListener() : PosixThread() {}

        void * run() override;
};

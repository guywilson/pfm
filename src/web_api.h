#pragma once

#include <httpserver.hpp>

#include "posixthread.h"

class APIListener : public PosixThread {
    private:
        static void validateSession(const httpserver::http_request & request);
        
        void registerEndPoints(httpserver::webserver & ws);

    public:
        APIListener() : PosixThread() {}

        void * run() override;
};

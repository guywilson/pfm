#pragma once

#include <httpserver.hpp>

#include "posixthread.h"

class APIListener : public PosixThread {
    public:
        httpserver::webserver * ws;

        APIListener() : PosixThread() {}

        void * run() override;
};

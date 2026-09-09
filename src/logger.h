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

#include <string>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>


/*
** Supported log levels...
*/
#define LOG_LEVEL_INFO          0x01
#define LOG_LEVEL_STATUS        0x02
#define LOG_LEVEL_DEBUG         0x04
#define LOG_LEVEL_SQL           0x08
#define LOG_LEVEL_ENTRY         0x10
#define LOG_LEVEL_EXIT          0x20
#define LOG_LEVEL_ENTRYEXIT     (LOG_LEVEL_ENTRY | LOG_LEVEL_EXIT)
#define LOG_LEVEL_ERROR         0x40
#define LOG_LEVEL_FATAL         0x80

#define LOG_LEVEL_ALL           (LOG_LEVEL_INFO | LOG_LEVEL_STATUS | LOG_LEVEL_DEBUG | LOG_LEVEL_SQL | LOG_LEVEL_ENTRYEXIT | LOG_LEVEL_ERROR | LOG_LEVEL_FATAL)

class Logger {
    public:
        static Logger & getInstance() {
            static Logger instance;
            return instance;
        }

    private:
        Logger() {}

        FILE * lfp;
        int loggingLevel;
        pthread_mutex_t mutex;

        static char * trim(const char * src);

        int logMessage(int logLevel, const char * fmt, va_list args);

    public:
        ~Logger() {}

        static int getLogLevelFromString(const std::string & logLevel);

        void init(const std::string & filename, int logLevel);
        void init(int logLevel);
        
        void close();

        void setLogLevel(int logLevel);
        void addLogLevel(int logLevel);
        void clearLogLevel(int logLevel);
        bool isLogLevel(int logLevel);

        void newline();
        int info(const char * fmt, ...);
        int status(const char * fmt, ...);
        int debug(const char * fmt, ...);
        int sql(const char * fmt, ...);
        int entry(const char * fmt, ...);
        int exit(const char * fmt, ...);
        int error(const char * fmt, ...);
        int fatal(const char * fmt, ...);
};

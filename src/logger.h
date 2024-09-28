#include <string>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

using namespace std;

#ifndef _INCL_LOGGER
#define _INCL_LOGGER

#define MAX_LOG_LENGTH          250

/*
** Supported log levels...
*/
#define LOG_LEVEL_INFO          0x01
#define LOG_LEVEL_STATUS        0x02
#define LOG_LEVEL_DEBUG         0x04
#define LOG_LEVEL_ENTRY         0x08
#define LOG_LEVEL_EXIT          0x10
#define LOG_LEVEL_ENTRYEXIT     (LOG_LEVEL_ENTRY | LOG_LEVEL_EXIT)
#define LOG_LEVEL_ERROR         0x40
#define LOG_LEVEL_FATAL         0x80

#define LOG_LEVEL_ALL           (LOG_LEVEL_INFO | LOG_LEVEL_STATUS | LOG_LEVEL_DEBUG | LOG_LEVEL_ENTRYEXIT | LOG_LEVEL_ERROR | LOG_LEVEL_FATAL)

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
        ~Logger();

        static int getLogLevelFromString(string & logLevel);

        void initLogger(string & filename, int logLevel);
        void initLogger(int logLevel);
        
        void closeLogger();

        void setLogLevel(int logLevel);
        void addLogLevel(int logLevel);
        void clearLogLevel(int logLevel);
        bool isLogLevel(int logLevel);

        void newline();
        int logInfo(const char * fmt, ...);
        int logStatus(const char * fmt, ...);
        int logDebug(const char * fmt, ...);
        int logEntry(const char * fmt, ...);
        int logExit(const char * fmt, ...);
        int logError(const char * fmt, ...);
        int logFatal(const char * fmt, ...);
};

#endif
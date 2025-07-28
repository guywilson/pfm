#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>

#include "strdate.h"
#include "logger.h"

using namespace std;

char * Logger::trim(const char * src) {
    if (src == NULL) {
        return NULL;
    }

    int originalLength = strlen(src);
    
    int i = 0;

    while (isspace(src[i++]));
    int startPos = i - 1;

    i = originalLength - 1;

    while (isspace(src[i--]));
    int endPos = i + 2;

    int newLength = endPos - startPos;

    char * trimmedString = (char *)malloc(newLength + 1);

    int j = 0;
    
    for (i = startPos;i < endPos;i++) {
        trimmedString[j++] = src[i];
    }

    trimmedString[j] = 0;

    return trimmedString;
}

void Logger::initLogger(string & filename, int logLevel) {
    this->loggingLevel = logLevel;

    if (filename.length() > 0) {
        this->lfp = fopen(filename.c_str(), "wt");

        if (this->lfp == NULL) {
	        syslog(LOG_INFO, "Failed to open log file %s", filename.c_str());
            this->lfp = stdout;
        }
    }
    else {
        this->lfp = stdout;
    }
}

void Logger::initLogger(int logLevel) {
    this->loggingLevel = logLevel;
    this->lfp = stdout;
}

void Logger::closeLogger() {
    if (lfp != stdout) {
        fclose(lfp);
    }
}

void Logger::setLogLevel(int logLevel) {
    this->loggingLevel = logLevel;
}

void Logger::addLogLevel(int logLevel) {
    this->loggingLevel |= logLevel;
}

void Logger::clearLogLevel(int logLevel) {
    this->loggingLevel &= ~logLevel;
}

bool Logger::isLogLevel(int logLevel) {
    return ((this->loggingLevel & logLevel) == logLevel ? true : false);
}

int Logger::getLogLevelFromString(string & logLevel) {
    int             level = 0;

    char * pszLogLevel = strdup(logLevel.c_str());
    char * reference = pszLogLevel;

    char * pszToken = trim(strtok_r(pszLogLevel, "|", &reference));

    while (pszToken != NULL) {
        if (strncmp(pszToken, "LOG_LEVEL_INFO", 14) == 0) {
            level |= LOG_LEVEL_INFO;
        }
        else if (strncmp(pszToken, "LOG_LEVEL_STATUS", 16) == 0) {
            level |= LOG_LEVEL_STATUS;
        }
        else if (strncmp(pszToken, "LOG_LEVEL_DEBUG", 15) == 0) {
            level |= LOG_LEVEL_DEBUG;
        }
        else if (strncmp(pszToken, "LOG_LEVEL_ERROR", 15) == 0) {
            level |= LOG_LEVEL_ERROR;
        }
        else if (strncmp(pszToken, "LOG_LEVEL_FATAL", 15) == 0) {
            level |= LOG_LEVEL_FATAL;
        }

        free(pszToken);

        pszToken = trim(strtok_r(NULL, "|", &reference));
    }

    free(pszLogLevel);

    return level;
}

int Logger::logMessage(int logLevel, const char * fmt, va_list args) {
    int         bytesWritten = 0;

    if (this->loggingLevel & logLevel) {
        string buffer = "[" + StrDate::getTimestampToMicrosecond() + "] ";

        switch (logLevel) {
            case LOG_LEVEL_DEBUG:
                buffer += "[DEBUG]";
                break;

            case LOG_LEVEL_STATUS:
                buffer += "[STATUS]";
                break;

            case LOG_LEVEL_INFO:
                buffer += "[INFO]";
                break;

            case LOG_LEVEL_ENTRY:
                buffer += "[ENTRY]";
                break;

            case LOG_LEVEL_EXIT:
                buffer += "[EXIT]";
                break;

            case LOG_LEVEL_ERROR:
                buffer += "[ERROR]";
                break;

            case LOG_LEVEL_FATAL:
                buffer += "[FATAL]";
                break;
        }

        buffer += fmt;
        buffer += "\n";

	    pthread_mutex_lock(&mutex);

        bytesWritten = vfprintf(this->lfp, buffer.c_str(), args);
        fflush(this->lfp);

	    pthread_mutex_unlock(&mutex);
    }

    return bytesWritten;
}

void Logger::newline() {
    fprintf(this->lfp, "\n");
}

int Logger::logInfo(const char * fmt, ...) {
    va_list     args;
    int         bytesWritten;

    va_start (args, fmt);
    
    bytesWritten = logMessage(LOG_LEVEL_INFO, fmt, args);
    
    va_end(args);
    
    return bytesWritten;
}

int Logger::logStatus(const char * fmt, ...) {
    va_list     args;
    int         bytesWritten;

    va_start (args, fmt);
    
    bytesWritten = logMessage(LOG_LEVEL_STATUS, fmt, args);
    
    va_end(args);
    
    return bytesWritten;
}

int Logger::logDebug(const char * fmt, ...) {
    va_list     args;
    int         bytesWritten;

    va_start (args, fmt);
    
    bytesWritten = logMessage(LOG_LEVEL_DEBUG, fmt, args);
    
    va_end(args);
    
    return bytesWritten;
}

int Logger::logEntry(const char * fmt, ...) {
    va_list     args;
    int         bytesWritten;

    va_start (args, fmt);
    
    bytesWritten = logMessage(LOG_LEVEL_ENTRY, fmt, args);
    
    va_end(args);
    
    return bytesWritten;
}

int Logger::logExit(const char * fmt, ...) {
    va_list     args;
    int         bytesWritten;

    va_start (args, fmt);
    
    bytesWritten = logMessage(LOG_LEVEL_EXIT, fmt, args);
    
    va_end(args);
    
    return bytesWritten;
}

int Logger::logError(const char * fmt, ...) {
    va_list     args;
    int         bytesWritten;

    va_start (args, fmt);
    
    bytesWritten = logMessage(LOG_LEVEL_ERROR, fmt, args);
    
    va_end(args);
    
    return bytesWritten;
}

int Logger::logFatal(const char * fmt, ...) {
    va_list     args;
    int         bytesWritten;

    va_start (args, fmt);
    
    bytesWritten = logMessage(LOG_LEVEL_FATAL, fmt, args);
    
    va_end(args);
    
    return bytesWritten;
}

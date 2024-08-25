#include <exception>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

using namespace std;

#ifndef __PFM_ERROR
#define __PFM_ERROR

#define MESSAGE_BUFFER_LEN                  4096

class pfm_error : public exception
{
    private:
        string          message;

    public:
        const char * getTitle() {
            return "PFM error: ";
        }

        pfm_error() {
            this->message.assign(getTitle());
        }

        pfm_error(const char * msg) : pfm_error() {
            this->message.append(msg);
        }

        pfm_error(const char * msg, const char * file, int line) : pfm_error() {
            char lineNumBuf[8];

            snprintf(lineNumBuf, 8, ":%d", line);

            this->message.append(msg);
            this->message.append(" at ");
            this->message.append(file);
            this->message.append(lineNumBuf);
        }

        virtual const char * what() const noexcept {
            return this->message.c_str();
        }

        static char * buildMsg(const char * fmt, ...) {
            va_list     args;
            char *      buffer;

            va_start(args, fmt);
            
            buffer = (char *)malloc(MESSAGE_BUFFER_LEN);
            
            vsnprintf(buffer, MESSAGE_BUFFER_LEN, fmt, args);
            
            va_end(args);

            return buffer;
        }
};

class pfm_fatal : public pfm_error {
    public:
        const char * getTitle() {
            return "Fatal error: ";
        }

        pfm_fatal() : pfm_error() {}
        pfm_fatal(const char * msg) : pfm_error(msg) {}
        pfm_fatal(const char * msg, const char * file, int line) : pfm_error(msg, file, line) {}
};

class pfm_validation_error : public pfm_error {
    public:
        const char * getTitle() {
            return "Validation error: ";
        }

        pfm_validation_error() : pfm_error() {}
        pfm_validation_error(const char * msg) : pfm_error(msg) {}
        pfm_validation_error(const char * msg, const char * file, int line) : pfm_error(msg, file, line) {}
};

#endif

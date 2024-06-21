#include <exception>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

using namespace std;

#ifndef _CALC_ERROR
#define _CALC_ERROR

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

        static const char * buildMsg(const char * fmt, ...) {
            va_list     args;
            char *      buffer;
            size_t      length;

            va_start(args, fmt);
            
            length = strlen(fmt) + 80;

            buffer = (char *)malloc(length);
            
            vsnprintf(buffer, length, fmt, args);
            
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
#endif

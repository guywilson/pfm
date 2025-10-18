#include <string>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include <sqlcipher/sqlite3.h>

#include "pfm_error.h"
#include "logger.h"

using namespace std;

#ifndef __INCL_PFM_ID
#define __INCL_PFM_ID

class pfm_id_t {
    private:
        string _value;
        bool _isNull;

        void inline checkForNumericChar(char c) {
            if (isprint(c)) {
                if (!isdigit(c)) {
                    throw pfm_validation_error("The ID value must be an integer");
                }
            }
        }

        void checkForNumericString(const string & s) {
            for (char c : s) {
                checkForNumericChar(c);
            }
        }

        void checkForNumericString(const char * s) {
            for (int i = 0;i < strlen(s);i++) {
                char c = s[i];

                checkForNumericChar(c);
            }
        }

    public:
        void set(sqlite3_int64 id) {
            char buffer[40];

            snprintf(buffer, sizeof(buffer), "%" PRId64, id);

            _value.assign(buffer);
            _isNull = false;
        }

        void set(const string & s) {
            if (s.compare("NULL") == 0) {
                clear();
                return;
            }

            checkForNumericString(s);

            _value = s;
            _isNull = false;
        }

        void set(const char * s) {
            if (strncmp(s, "NULL", 4) == 0) {
                clear();
                return;
            }
            
            checkForNumericString(s);

            _value = s;
            _isNull = false;
        }

        void clear() {
            _value = "NULL";
            _isNull = true;
        }

        string getValue() const {
            if (isNull()) {
                return "NULL";
            }

            return _value;
        }
        
        const char * c_str() const {
            return _value.c_str();
        }

        sqlite3_int64 intValue() {
            sqlite3_int64 id = (sqlite3_int64)strtoll(_value.c_str(), NULL, 10);
            return id;
        }

        pfm_id_t() {
            clear();
        }

        pfm_id_t(sqlite3_int64 id) {
            set(id);
        }

        pfm_id_t(const string & s) {
            set(s);
        }

        bool isNull() const {
            return _isNull;
        }

        pfm_id_t & operator=(const pfm_id_t & rhs) {
            if (this == &rhs) {
                return *this;
            }

            if (rhs.isNull()) {
                this->clear();
                return *this;
            }

            this->set(rhs.getValue());

            return *this;
        }

        pfm_id_t & operator=(const string & rhs) {
            this->set(rhs);
            return *this;
        }

        pfm_id_t & operator=(const char * rhs) {
            this->set(rhs);
            return *this;
        }

        pfm_id_t & operator=(const sqlite3_int64 rhs) {
            this->set(rhs);
            return *this;
        }

        bool operator==(const pfm_id_t & rhs) {
            if (rhs.isNull()) {
                return true;
            }

            return (this->_value.compare(rhs.getValue()) == 0);
        }

        bool operator==(const string & rhs) {
            return (this->_value.compare(rhs) == 0);
        }

        bool operator==(const sqlite3_int64 rhs) {
            pfm_id_t id = rhs;
            return (this->_value.compare(id.getValue()) == 0);
        }

        bool operator!=(const pfm_id_t & rhs) {
            return (this->_value.compare(rhs.getValue()) != 0);
        }

        bool operator!=(const string & rhs) {
            return (this->_value.compare(rhs) != 0);
        }

        bool operator!=(const sqlite3_int64 rhs) {
            pfm_id_t id = rhs;
            return (this->_value.compare(id.getValue()) != 0);
        }
};

#endif

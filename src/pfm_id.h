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
#include <stdint.h>
#include <inttypes.h>

#include <sqlcipher/sqlite3.h>

#include "pfm_error.h"
#include "logger.h"

#ifdef PFM_TEST_SUITE_ENABLED
#include <iostream>
#endif


class pfm_id_t {
    private:
        std::string _value;
        bool _isNull;

        void inline checkForNumericChar(char c) {
            if (isprint(c)) {
                if (!isdigit(c)) {
                    throw pfm_validation_error("The ID value must be an integer");
                }
            }
        }

        void checkForNumericString(const std::string & s) {
            for (char c : s) {
                checkForNumericChar(c);
            }
        }

        void checkForNumericString(const char * s) {
            for (int i = 0;i < (int)strlen(s);i++) {
                char c = s[i];

                checkForNumericChar(c);
            }
        }

    public:
        void set(sqlite3_int64 id) {
            char buffer[40];

            snprintf(buffer, sizeof(buffer), "%lld", (long long)id);

            _value.assign(buffer);
            _isNull = false;
        }

        void set(const std::string & s) {
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

        std::string getValue() const {
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

        pfm_id_t(const std::string & s) {
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

        pfm_id_t & operator=(const std::string & rhs) {
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
                return this->isNull();
            }

            return (this->_value.compare(rhs.getValue()) == 0);
        }

        bool operator==(const std::string & rhs) {
            return (this->_value.compare(rhs) == 0);
        }

        bool operator==(const sqlite3_int64 rhs) {
            pfm_id_t id = rhs;
            return (this->_value.compare(id.getValue()) == 0);
        }

        bool operator!=(const pfm_id_t & rhs) {
            return (this->_value.compare(rhs.getValue()) != 0);
        }

        bool operator!=(const std::string & rhs) {
            return (this->_value.compare(rhs) != 0);
        }

        bool operator!=(const sqlite3_int64 rhs) {
            pfm_id_t id = rhs;
            return (this->_value.compare(id.getValue()) != 0);
        }
};

#ifdef PFM_TEST_SUITE_ENABLED
class PfmIdTest {
    private:
        static void testDefaultValue() {
            pfm_id_t id;

            if (!id.isNull() || id.getValue() != "NULL" || std::string(id.c_str()) != "NULL" || id.intValue() != 0) {
                throw pfm_error("testDefaultValue(): Test failed");
            }
            else {
                std::cout << "testDefaultValue(): Test passed" << std::endl;
            }
        }

        static void testSetInteger() {
            pfm_id_t id;
            id.set(sqlite3_int64(9223372036854775807LL));

            if (id.isNull() || id.getValue() != "9223372036854775807" || id.intValue() != 9223372036854775807LL) {
                throw pfm_error("testSetInteger(): Test failed");
            }
            else {
                std::cout << "testSetInteger(): Test passed" << std::endl;
            }
        }

        static void testSetZero() {
            pfm_id_t id;
            id.set(sqlite3_int64(0));

            if (id.isNull() || id.getValue() != "0" || id.intValue() != 0) {
                throw pfm_error("testSetZero(): Test failed");
            }
            else {
                std::cout << "testSetZero(): Test passed" << std::endl;
            }
        }

        static void testSetString() {
            pfm_id_t id;
            id.set(std::string("12345678901"));

            if (id.isNull() || id.getValue() != "12345678901" || id.intValue() != 12345678901LL) {
                throw pfm_error("testSetString(): Test failed");
            }
            else {
                std::cout << "testSetString(): Test passed" << std::endl;
            }
        }

        static void testSetChar() {
            pfm_id_t id;
            id.set("000123");

            if (id.isNull() || id.getValue() != "000123" || std::string(id.c_str()) != "000123" || id.intValue() != 123) {
                throw pfm_error("testSetChar(): Test failed");
            }
            else {
                std::cout << "testSetChar(): Test passed" << std::endl;
            }
        }

        static void testGetters() {
            const pfm_id_t id(std::string("456"));
            pfm_id_t numeric(sqlite3_int64(456));

            if (id.isNull() || id.getValue() != "456" || std::string(id.c_str()) != "456" || numeric.intValue() != 456) {
                throw pfm_error("testGetters(): Test failed");
            }
            else {
                std::cout << "testGetters(): Test passed" << std::endl;
            }
        }

        static void testClear() {
            pfm_id_t id(sqlite3_int64(123));
            id.clear();

            if (!id.isNull() || id.getValue() != "NULL" || std::string(id.c_str()) != "NULL" || id.intValue() != 0) {
                throw pfm_error("testClear(): Test failed");
            }
            else {
                std::cout << "testClear(): Test passed" << std::endl;
            }
        }

        static void testSetNullString() {
            pfm_id_t id(sqlite3_int64(123));
            id.set(std::string("NULL"));

            if (!id.isNull() || id.getValue() != "NULL" || std::string(id.c_str()) != "NULL") {
                throw pfm_error("testSetNullString(): Test failed");
            }
            else {
                std::cout << "testSetNullString(): Test passed" << std::endl;
            }
        }

        static void testSetNullChar() {
            pfm_id_t id(sqlite3_int64(123));
            id.set("NULL");

            if (!id.isNull() || id.getValue() != "NULL" || std::string(id.c_str()) != "NULL") {
                throw pfm_error("testSetNullChar(): Test failed");
            }
            else {
                std::cout << "testSetNullChar(): Test passed" << std::endl;
            }
        }

        static void testAssignmentId() {
            const pfm_id_t source(sqlite3_int64(123));
            pfm_id_t id;
            pfm_id_t & result = (id = source);

            if (&result != &id || id.isNull() || id.getValue() != "123" || source.getValue() != "123") {
                throw pfm_error("testAssignmentId(): Test failed");
            }
            else {
                std::cout << "testAssignmentId(): Test passed" << std::endl;
            }
        }

        static void testAssignmentNullId() {
            const pfm_id_t source;
            pfm_id_t id(sqlite3_int64(123));
            pfm_id_t & result = (id = source);

            if (&result != &id || !id.isNull() || id.getValue() != "NULL") {
                throw pfm_error("testAssignmentNullId(): Test failed");
            }
            else {
                std::cout << "testAssignmentNullId(): Test passed" << std::endl;
            }
        }

        static void testSelfAssignment() {
            pfm_id_t id(sqlite3_int64(123));
            const pfm_id_t & alias = id;
            pfm_id_t & result = (id = alias);

            if (&result != &id || id.isNull() || id.getValue() != "123") {
                throw pfm_error("testSelfAssignment(): Test failed");
            }
            else {
                std::cout << "testSelfAssignment(): Test passed" << std::endl;
            }
        }

        static void testNullSelfAssignment() {
            pfm_id_t id;
            const pfm_id_t & alias = id;
            pfm_id_t & result = (id = alias);

            if (&result != &id || !id.isNull() || id.getValue() != "NULL") {
                throw pfm_error("testNullSelfAssignment(): Test failed");
            }
            else {
                std::cout << "testNullSelfAssignment(): Test passed" << std::endl;
            }
        }

        static void testAssignmentString() {
            pfm_id_t id;
            pfm_id_t & result = (id = std::string("456"));

            if (&result != &id || id.isNull() || id.getValue() != "456" || id.intValue() != 456) {
                throw pfm_error("testAssignmentString(): Test failed");
            }
            else {
                std::cout << "testAssignmentString(): Test passed" << std::endl;
            }
        }

        static void testAssignmentChar() {
            pfm_id_t id;
            pfm_id_t & result = (id = "456");

            if (&result != &id || id.isNull() || id.getValue() != "456" || id.intValue() != 456) {
                throw pfm_error("testAssignmentChar(): Test failed");
            }
            else {
                std::cout << "testAssignmentChar(): Test passed" << std::endl;
            }
        }

        static void testAssignmentInteger() {
            pfm_id_t id;
            pfm_id_t & result = (id = sqlite3_int64(456));

            if (&result != &id || id.isNull() || id.getValue() != "456" || id.intValue() != 456) {
                throw pfm_error("testAssignmentInteger(): Test failed");
            }
            else {
                std::cout << "testAssignmentInteger(): Test passed" << std::endl;
            }
        }

        static void testAssignmentNullString() {
            pfm_id_t id(sqlite3_int64(123));
            pfm_id_t & result = (id = std::string("NULL"));

            if (&result != &id || !id.isNull() || id.getValue() != "NULL") {
                throw pfm_error("testAssignmentNullString(): Test failed");
            }
            else {
                std::cout << "testAssignmentNullString(): Test passed" << std::endl;
            }
        }

        static void testAssignmentNullChar() {
            pfm_id_t id(sqlite3_int64(123));
            pfm_id_t & result = (id = "NULL");

            if (&result != &id || !id.isNull() || id.getValue() != "NULL") {
                throw pfm_error("testAssignmentNullChar(): Test failed");
            }
            else {
                std::cout << "testAssignmentNullChar(): Test passed" << std::endl;
            }
        }

        static void testEqualityId() {
            pfm_id_t id(sqlite3_int64(123));

            if (!(id == pfm_id_t(sqlite3_int64(123))) || id == pfm_id_t(sqlite3_int64(456))) {
                throw pfm_error("testEqualityId(): Test failed");
            }
            else {
                std::cout << "testEqualityId(): Test passed" << std::endl;
            }
        }

        static void testInequalityId() {
            pfm_id_t id(sqlite3_int64(123));

            if (id != pfm_id_t(sqlite3_int64(123)) || !(id != pfm_id_t(sqlite3_int64(456)))) {
                throw pfm_error("testInequalityId(): Test failed");
            }
            else {
                std::cout << "testInequalityId(): Test passed" << std::endl;
            }
        }

        static void testEqualityString() {
            pfm_id_t id(sqlite3_int64(123));

            if (!(id == std::string("123")) || id == std::string("456")) {
                throw pfm_error("testEqualityString(): Test failed");
            }
            else {
                std::cout << "testEqualityString(): Test passed" << std::endl;
            }
        }

        static void testInequalityString() {
            pfm_id_t id(sqlite3_int64(123));

            if (id != std::string("123") || !(id != std::string("456"))) {
                throw pfm_error("testInequalityString(): Test failed");
            }
            else {
                std::cout << "testInequalityString(): Test passed" << std::endl;
            }
        }

        static void testEqualityInteger() {
            pfm_id_t id(sqlite3_int64(123));

            if (!(id == sqlite3_int64(123)) || id == sqlite3_int64(456)) {
                throw pfm_error("testEqualityInteger(): Test failed");
            }
            else {
                std::cout << "testEqualityInteger(): Test passed" << std::endl;
            }
        }

        static void testInequalityInteger() {
            pfm_id_t id(sqlite3_int64(123));

            if (id != sqlite3_int64(123) || !(id != sqlite3_int64(456))) {
                throw pfm_error("testInequalityInteger(): Test failed");
            }
            else {
                std::cout << "testInequalityInteger(): Test passed" << std::endl;
            }
        }

        static void testNullEquality() {
            pfm_id_t nullId;
            pfm_id_t otherNullId;
            pfm_id_t id(sqlite3_int64(123));

            if (!(nullId == otherNullId) || id == nullId || nullId == id || !(nullId == std::string("NULL")) || nullId == sqlite3_int64(0)) {
                throw pfm_error("testNullEquality(): Test failed");
            }
            else {
                std::cout << "testNullEquality(): Test passed" << std::endl;
            }
        }

        static void testNullInequality() {
            pfm_id_t nullId;
            pfm_id_t otherNullId;
            pfm_id_t id(sqlite3_int64(123));

            if (nullId != otherNullId || !(id != nullId) || !(nullId != id) || nullId != std::string("NULL") || !(nullId != sqlite3_int64(0))) {
                throw pfm_error("testNullInequality(): Test failed");
            }
            else {
                std::cout << "testNullInequality(): Test passed" << std::endl;
            }
        }

        static void testInvalidString() {
            pfm_id_t id(sqlite3_int64(123));
            bool rejected = false;
            try {
                id.set(std::string("12x3"));
            }
            catch (pfm_validation_error &) {
                rejected = true;
            }

            if (!rejected || id.isNull() || id.getValue() != "123") {
                throw pfm_error("testInvalidString(): Test failed");
            }
            else {
                std::cout << "testInvalidString(): Test passed" << std::endl;
            }
        }

        static void testInvalidChar() {
            pfm_id_t id(sqlite3_int64(123));
            bool rejected = false;
            try {
                id.set("12x3");
            }
            catch (pfm_validation_error &) {
                rejected = true;
            }

            if (!rejected || id.isNull() || id.getValue() != "123") {
                throw pfm_error("testInvalidChar(): Test failed");
            }
            else {
                std::cout << "testInvalidChar(): Test passed" << std::endl;
            }
        }

    public:
        static void run() {
            int numTestsPassed = 0;
            int numTestsFailed = 0;
            void (*tests[])() = {
                testDefaultValue,
                testSetInteger,
                testSetZero,
                testSetString,
                testSetChar,
                testGetters,
                testClear,
                testSetNullString,
                testSetNullChar,
                testAssignmentId,
                testAssignmentNullId,
                testSelfAssignment,
                testNullSelfAssignment,
                testAssignmentString,
                testAssignmentChar,
                testAssignmentInteger,
                testAssignmentNullString,
                testAssignmentNullChar,
                testEqualityId,
                testInequalityId,
                testEqualityString,
                testInequalityString,
                testEqualityInteger,
                testInequalityInteger,
                testNullEquality,
                testNullInequality,
                testInvalidString,
                testInvalidChar
            };

            for (auto test : tests) {
                try {
                    test();
                    numTestsPassed++;
                }
                catch (pfm_error & e) {
                    std::cout << e.what() << std::endl;
                    numTestsFailed++;
                }
            }

            std::cout << "Tests passed: " << numTestsPassed << ", tests failed: " << numTestsFailed << std::endl;
        }
};
#endif

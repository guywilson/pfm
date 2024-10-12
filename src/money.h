#include <iostream>
#include <string>
#include <stdint.h>

#include "logger.h"
#include "pfm_error.h"

using namespace std;

#ifndef __INCL_MONEY
#define __INCL_MONEY

#define DECIMAL_POINT_NOT_FOUND_VALUE               -1
#define AMOUNT_BUFFER_LENGTH                        64

typedef int32_t money_t;

class MoneyTest;

class Money {
#ifdef PFM_TEST_SUITE_ENABLED
    friend class MoneyTest;
#endif
    private:
        money_t representedValue;

        Logger & log = Logger::getInstance();
        
        int findDecimalPointPos(const char * amount);
        void copyToDecimalPoint(char * targetBuffer, const char * amount, int targetBufferLen);
        void copyAfterDecimalPoint(char * targetBuffer, const char * amount, int tragetBufferLen);
        money_t getWholeValueFromString(const char * amount);
        money_t getDecimalValueFromString(const char * amount);
        money_t makeRepresentedValue(money_t whole, money_t decimal);

        void _setValue(const char * amount);
        void _setValue(const string & amount);
        void _setValue(double amount);
        void _setValue(money_t amount);

    protected:
        Money(money_t amount);

        money_t _getValue() const;

    public:
        Money();
        Money(string & amount);
        Money(const char * amount);
        Money(double amount);

        Money(const Money & src);

        double getDoubleValue();
        string getRawStringValue() const;
        string getFormattedStringValue() const;

        Money & operator=(const Money & rhs);
        Money & operator=(const string & rhs);
        Money & operator=(const char * rhs);
        Money & operator=(const double rhs);

        const Money operator+(const Money & rhs);
        Money & operator+=(const Money & rhs);

        const Money operator-(const Money & rhs);
        Money & operator-=(const Money & rhs);

        const Money operator*(const int rhs);
        Money & operator*=(const int rhs);
        
        const Money operator/(const int rhs);
        Money & operator/=(const int rhs);

        bool operator==(const Money & rhs);
        bool operator!=(const Money & rhs);
        bool operator<(const Money & src);
        bool operator<=(const Money & src);
        bool operator>(const Money & src);
        bool operator>=(const Money & src);
};

#ifdef PFM_TEST_SUITE_ENABLED
class MoneyTest {
    private:
        static void testCreateString() {
            string amount = "137.47";
            Money m(amount);

            if (m._getValue() != 13747L) {
                throw pfm_error("testCreateString(): Test failed");
            }
            else {
                cout << "testCreateString(): Test passed" << endl;
            }
        }

        static void testCreateChar1() {
            const char * amount = "137.47";
            Money m(amount);

            if (m._getValue() != 13747L) {
                throw pfm_error("testCreateChar1(): Test failed");
            }
            else {
                cout << "testCreateChar1(): Test passed" << endl;
            }
        }

        static void testCreateChar2() {
            const char * amount = "137";
            Money m(amount);

            if (m._getValue() != 13700L) {
                throw pfm_error("testCreateChar2(): Test failed");
            }
            else {
                cout << "testCreateChar2(): Test passed" << endl;
            }
        }

        static void testCreateChar3() {
            const char * amount = "-25.25";
            Money m(amount);

            if (m._getValue() != -2525L) {
                throw pfm_error("testCreateChar3(): Test failed");
            }
            else {
                cout << "testCreateChar3(): Test passed" << endl;
            }
        }

        static void testCreateChar4() {
            const char * amount = "27.30";
            Money m(amount);

            if (m._getValue() != 2730L) {
                throw pfm_error("testCreateChar4(): Test failed");
            }
            else {
                cout << "testCreateChar4(): Test passed" << endl;
            }
        }

        static void testCreateDouble1() {
            double amount = 137.47f;
            Money m(amount);

            if (m._getValue() != 13747L) {
                throw pfm_error("testCreateDouble1(): Test failed");
            }
            else {
                cout << "testCreateDouble1(): Test passed" << endl;
            }
        }

        static void testCreateDouble2() {
            double amount = 137.0f;
            Money m(amount);

            if (m._getValue() != 13700L) {
                throw pfm_error("testCreateDouble2(): Test failed");
            }
            else {
                cout << "testCreateDouble2(): Test passed" << endl;
            }
        }

        static void testCreateDouble3() {
            double amount = -25.25f;
            Money m(amount);

            if (m._getValue() != -2525L) {
                throw pfm_error("testCreateDouble3(): Test failed");
            }
            else {
                cout << "testCreateDouble3(): Test passed" << endl;
            }
        }

        static void testCopyConstructor() {
            const char * amount = "107.86";
            Money m1(amount);
            Money m2(m1);

            if (m2._getValue() != 10786L) {
                throw pfm_error("testCopyConstructor(): Test failed");
            }
            else {
                cout << "testCopyConstructor(): Test passed" << endl;
            }
        }

        static void testGetDoubleValue1() {
            const char * amount = "137.47";
            Money m(amount);

            double value = m.getDoubleValue();

            if (value != (double)137.47) {
                throw pfm_error("testGetDoubleValue1(): Test failed");
            }
            else {
                cout << "testGetDoubleValue1(): Test passed" << endl;
            }
        }

        static void testGetDoubleValue2() {
            const char * amount = "137.00";
            Money m(amount);

            double value = m.getDoubleValue();

            if (value != (double)137.0) {
                throw pfm_error("testGetDoubleValue2(): Test failed");
            }
            else {
                cout << "testGetDoubleValue2(): Test passed" << endl;
            }
        }

        static void testGetRawStringValue1() {
            const char * amount = "137.47";
            Money m(amount);

            if (m.getRawStringValue() != amount) {
                throw pfm_error("testGetRawStringValue1(): Test failed");
            }
            else {
                cout << "testGetRawStringValue1(): Test passed" << endl;
            }
        }

        static void testGetRawStringValue2() {
            const char * amount = "137.00";
            Money m(amount);

            if (m.getRawStringValue() != amount) {
                throw pfm_error("testGetRawStringValue2(): Test failed");
            }
            else {
                cout << "testGetRawStringValue2(): Test passed" << endl;
            }
        }

        static void testGetRawStringValue3() {
            const char * amount = "-25.25";
            Money m(amount);

            if (m.getRawStringValue() != amount) {
                throw pfm_error("testGetRawStringValue3(): Test failed");
            }
            else {
                cout << "testGetRawStringValue3(): Test passed" << endl;
            }
        }

        static void testGetRawStringValue4() {
            const char * amount = "-137.47";
            Money m(amount);

            if (m.getRawStringValue() != amount) {
                throw pfm_error("testGetRawStringValue4(): Test failed");
            }
            else {
                cout << "testGetRawStringValue4(): Test passed" << endl;
            }
        }

        static void testAssignment1() {
            const char * amount = "137.47";
            Money m1(amount);
            Money m2;

            m2 = m1;

            if (m2._getValue() != 13747L) {
                throw pfm_error("testAssignment1(): Test failed");
            }
            else {
                cout << "testAssignment1(): Test passed" << endl;
            }
        }

        static void testAssignment2() {
            string amount = "137.47";
            Money m2;

            m2 = amount;

            if (m2._getValue() != 13747L) {
                throw pfm_error("testAssignment2(): Test failed");
            }
            else {
                cout << "testAssignment2(): Test passed" << endl;
            }
        }

        static void testAssignment3() {
            double amount = 137.47f;
            Money m2;

            m2 = amount;

            if (m2._getValue() != 13747L) {
                throw pfm_error("testAssignment3(): Test failed");
            }
            else {
                cout << "testAssignment3(): Test passed" << endl;
            }
        }

        static void testOperatorPlus() {
            Money m1("107.86");
            Money m2("27.30");
            Money m3 = m1 + m2;

            if (m3._getValue() != 13516L) {
                throw pfm_error("testOperatorPlus(): Test failed");
            }
            else {
                cout << "testOperatorPlus(): Test passed" << endl;
            }
        }

        static void testOperatorPlusEquals() {
            Money m1("107.86");
            Money m2("27.30");
            m2 += m1;

            if (m2._getValue() != 13516L) {
                throw pfm_error("testOperatorPlusEquals(): Test failed");
            }
            else {
                cout << "testOperatorPlusEquals(): Test passed" << endl;
            }
        }

        static void testOperatorMinus() {
            Money m1("107.86");
            Money m2("27.30");
            Money m3 = m1 - m2;

            if (m3._getValue() != 8056L) {
                throw pfm_error("testOperatorMinus(): Test failed");
            }
            else {
                cout << "testOperatorMinus(): Test passed" << endl;
            }
        }

        static void testOperatorMinusEquals() {
            Money m1("107.86");
            Money m2("27.30");
            m1 -= m2;

            if (m1._getValue() != 8056L) {
                throw pfm_error("testOperatorMinusEquals(): Test failed");
            }
            else {
                cout << "testOperatorMinusEquals(): Test passed" << endl;
            }
        }

        static void testOperatorTimes() {
            Money m1("107.86");
            Money m3 = m1 * 3;

            if (m3._getValue() != 32358L) {
                throw pfm_error("testOperatorTimes(): Test failed");
            }
            else {
                cout << "testOperatorTimes(): Test passed" << endl;
            }
        }

        static void testOperatorTimesEquals() {
            Money m1("107.86");
            m1 *= 3;

            if (m1._getValue() != 32358L) {
                throw pfm_error("testOperatorTimesEquals(): Test failed");
            }
            else {
                cout << "testOperatorTimesEquals(): Test passed" << endl;
            }
        }

        static void testOperatorDivide() {
            Money m1("107.86");
            Money m3 = m1 / 2;

            if (m3._getValue() != 5393L) {
                throw pfm_error("testOperatorDivide(): Test failed");
            }
            else {
                cout << "testOperatorDivide(): Test passed" << endl;
            }
        }

        static void testOperatorDivideEquals() {
            Money m1("107.86");
            m1 /= 2;

            if (m1._getValue() != 5393L) {
                throw pfm_error("testOperatorDivideEquals(): Test failed");
            }
            else {
                cout << "testOperatorDivideEquals(): Test passed" << endl;
            }
        }

        static void testOperatorEquality1() {
            Money m1("107.86");
            Money m2("107.86");

            if (m1 != m2) {
                throw pfm_error("testOperatorEquality1(): Test failed");
            }
            else {
                cout << "testOperatorEquality1(): Test passed" << endl;
            }
        }

        static void testOperatorEquality2() {
            Money m1("107.86");
            Money m2("107.85");

            if (m1 == m2) {
                throw pfm_error("testOperatorEquality2(): Test failed");
            }
            else {
                cout << "testOperatorEquality2(): Test passed" << endl;
            }
        }

        static void testOperatorInequality1() {
            Money m1("107.86");
            Money m2("107.85");

            if (m1 == m2) {
                throw pfm_error("testOperatorInequality1(): Test failed");
            }
            else {
                cout << "testOperatorInequality1(): Test passed" << endl;
            }
        }

        static void testOperatorInequality2() {
            Money m1("107.86");
            Money m2("107.86");

            if (m1 != m2) {
                throw pfm_error("testOperatorInequality2(): Test failed");
            }
            else {
                cout << "testOperatorInequality2(): Test passed" << endl;
            }
        }

        static void testOperatorLessThan1() {
            Money m1("107.86");
            Money m2("105.86");

            if (!(m2 < m1)) {
                throw pfm_error("testOperatorLessThan1(): Test failed");
            }
            else {
                cout << "testOperatorLessThan1(): Test passed" << endl;
            }
        }

        static void testOperatorLessThan2() {
            Money m1("107.86");
            Money m2("109.81");

            if ((m2 < m1)) {
                throw pfm_error("testOperatorLessThan2(): Test failed");
            }
            else {
                cout << "testOperatorLessThan2(): Test passed" << endl;
            }
        }

        static void testOperatorLessThanOrEquals() {
            Money m1("107.86");
            Money m2("107.86");

            if (!(m2 <= m1)) {
                throw pfm_error("testOperatorLessThanOrEquals(): Test failed");
            }
            else {
                cout << "testOperatorLessThanOrEquals(): Test passed" << endl;
            }
        }

        static void testOperatorGreaterThan1() {
            Money m1("105.86");
            Money m2("107.86");

            if (!(m2 > m1)) {
                throw pfm_error("testOperatorGreaterThan1(): Test failed");
            }
            else {
                cout << "testOperatorGreaterThan1(): Test passed" << endl;
            }
        }

        static void testOperatorGreaterThan2() {
            Money m1("109.81");
            Money m2("107.86");

            if ((m2 > m1)) {
                throw pfm_error("testOperatorGreaterThan2(): Test failed");
            }
            else {
                cout << "testOperatorGreaterThan2(): Test passed" << endl;
            }
        }

        static void testOperatorGreaterThanOrEquals() {
            Money m1("107.86");
            Money m2("107.86");

            if (!(m2 >= m1)) {
                throw pfm_error("testOperatorGreaterThanOrEquals(): Test failed");
            }
            else {
                cout << "testOperatorGreaterThanOrEquals(): Test passed" << endl;
            }
        }

    public:
        static void run() {
            int numTestsPassed = 0;
            int numTestsFailed = 0;

            try {
                testCreateString();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testCreateChar1();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testCreateChar2();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testCreateChar3();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testCreateChar4();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testCreateDouble1();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testCreateDouble2();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testCreateDouble3();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testCopyConstructor();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testGetDoubleValue1();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testGetDoubleValue2();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testGetRawStringValue1();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testGetRawStringValue2();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testGetRawStringValue3();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testGetRawStringValue4();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testAssignment1();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testAssignment2();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testAssignment3();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testOperatorPlus();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testOperatorPlusEquals();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testOperatorMinus();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testOperatorMinusEquals();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testOperatorTimes();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testOperatorTimesEquals();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testOperatorDivide();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testOperatorDivideEquals();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testOperatorEquality1();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testOperatorEquality2();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testOperatorInequality1();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testOperatorInequality2();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testOperatorLessThan1();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testOperatorLessThan2();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testOperatorLessThanOrEquals();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testOperatorGreaterThan1();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testOperatorGreaterThan2();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testOperatorGreaterThanOrEquals();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            cout << "Tests passed: " << numTestsPassed << ", tests failed: " << numTestsFailed << endl;
        }
};
#endif
#endif

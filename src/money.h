#include <iostream>
#include <string>
#include <stdint.h>

#include "pfm_error.h"

using namespace std;

#ifndef __INCL_MONEY
#define __INCL_MONEY

#define DECIMAL_POINT_NOT_FOUND_VALUE               -1

typedef int32_t money_t;

class MoneyTest;

class Money {
#ifdef PFM_TEST_SUITE_ENABLED
    friend class MoneyTest;
#endif
    private:
        money_t representedValue;

        int findDecimalPointPos(const char * amount);
        void _setValue(money_t m);

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

        Money operator+(const Money & rhs);
        Money operator+=(const Money & rhs);
        Money operator-(const Money & rhs);
        Money operator-=(const Money & rhs);
        Money operator*(const Money & rhs);
        Money operator*=(const Money & rhs);
        Money operator/(const Money & rhs);
        Money operator/=(const Money & rhs);

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
            Money m = Money(amount);

            if (m._getValue() != 13747L) {
                throw pfm_error("testCreateString(): Test failed");
            }
            else {
                cout << "testCreateString(): Test passed" << endl;
            }
        }

        static void testCreateChar1() {
            const char * amount = "137.47";
            Money m = Money(amount);

            if (m._getValue() != 13747L) {
                throw pfm_error("testCreateChar1(): Test failed");
            }
            else {
                cout << "testCreateChar1(): Test passed" << endl;
            }
        }

        static void testCreateChar2() {
            const char * amount = "137";
            Money m = Money(amount);

            if (m._getValue() != 13700L) {
                throw pfm_error("testCreateChar2(): Test failed");
            }
            else {
                cout << "testCreateChar2(): Test passed" << endl;
            }
        }

        static void testCreateChar3() {
            const char * amount = "-25.25";
            Money m = Money(amount);

            if (m._getValue() != -2525L) {
                throw pfm_error("testCreateChar3(): Test failed");
            }
            else {
                cout << "testCreateChar3(): Test passed" << endl;
            }
        }

        static void testCreateDouble1() {
            double amount = 137.47f;
            Money m = Money(amount);

            if (m._getValue() != 13747L) {
                throw pfm_error("testCreateDouble1(): Test failed");
            }
            else {
                cout << "testCreateDouble1(): Test passed" << endl;
            }
        }

        static void testCreateDouble2() {
            double amount = 137.0f;
            Money m = Money(amount);

            if (m._getValue() != 13700L) {
                throw pfm_error("testCreateDouble2(): Test failed");
            }
            else {
                cout << "testCreateDouble2(): Test passed" << endl;
            }
        }

        static void testCreateDouble3() {
            double amount = -25.25f;
            Money m = Money(amount);

            if (m._getValue() != -2525L) {
                throw pfm_error("testCreateDouble3(): Test failed");
            }
            else {
                cout << "testCreateDouble3(): Test passed" << endl;
            }
        }

        static void testGetDoubleValue1() {
            const char * amount = "137.47";
            Money m = Money(amount);

            if (m.getDoubleValue() != 137.47f) {
                throw pfm_error("testGetDoubleValue1: Test failed");
            }
            else {
                cout << "testGetDoubleValue1(): Test passed" << endl;
            }
        }

        static void testGetDoubleValue2() {
            const char * amount = "137.00";
            Money m = Money(amount);

            if (m.getDoubleValue() != 137.0f) {
                throw pfm_error("testGetDoubleValue2: Test failed");
            }
            else {
                cout << "testGetDoubleValue2(): Test passed" << endl;
            }
        }

    public:
        void run() {

        }
};
#endif
#endif

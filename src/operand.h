#include <string>

#include <ctype.h>
#include <stdint.h>

#include <gmp.h>
#include <mpfr.h>

#include "token.h"

using namespace std;

#ifndef __INCL_OPERAND
#define __INCL_OPERAND

#define MPFR_BASE_PRECISION           1024L

#define INTERMEDIATE_PRECISION          256

#define BASE_10                          10
#define DECIMAL                     BASE_10

#define OUTPUT_MAX_STRING_LENGTH       4096

static inline bool isDigit(char ch) {
    return isdigit(ch);
}

static inline bool isOperandChar(char ch) {
    return (isDigit(ch) || ch == '-' || ch == '.');
}

class Operand : public Token {
    private:
        void initialiseValue() {
            mpfr_init2(value, MPFR_BASE_PRECISION);
        }

    public:
        mpfr_t value;

        Operand() {
            initialiseValue();
        }

        Operand(const string & token) : Token(token) {
            initialiseValue();
            mpfr_strtofr(value, token.c_str(), NULL, DECIMAL, MPFR_RNDA);
        }

        Operand(mpfr_t src) {
            initialiseValue();
            mpfr_set(value, src, MPFR_RNDA);
        }

        void clear() {
            mpfr_clear(value);
        }

        static bool isOperand(const string & token) {
            for (int i = 0;i < token.length();i++) {
                char ch = token[i];

                if (!isOperandChar(ch)) {
                    return false;
                }
            }

            return true;
        }

        string toString(long precision = 2) {
            char szOutputString[OUTPUT_MAX_STRING_LENGTH];
            char szFormatString[32];
            string output;

            snprintf(szFormatString, 32, "%%.%ldRf", precision);
            mpfr_snprintf(szOutputString, OUTPUT_MAX_STRING_LENGTH, szFormatString, value);
            output.assign(szOutputString);

            return output;
        }

        virtual string evaluate() override {
            return toString(INTERMEDIATE_PRECISION);
        }

        static const string CLASS_NAME() {
            return "Operand";
        }

        virtual const string className() override {
            return Operand::CLASS_NAME();
        }

        const Operand deg() {
            Operand result;

            mpfr_t  pi;
            mpfr_t  one_eighty;

            mpfr_init2(pi, MPFR_BASE_PRECISION);
            mpfr_init2(one_eighty, MPFR_BASE_PRECISION);

            mpfr_const_pi(pi, MPFR_RNDA);
            mpfr_set_ui(one_eighty, 180U, MPFR_RNDA);
            
            mpfr_div(result.value, one_eighty, pi, MPFR_RNDA);
            mpfr_mul(result.value, result.value, this->value, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand operator+(const Operand & rhs) {
            Operand result;
            mpfr_add(result.value, this->value, rhs.value, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand operator-(const Operand & rhs) {
            Operand result;
            mpfr_sub(result.value, this->value, rhs.value, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand operator*(const Operand & rhs) {
            Operand result;
            mpfr_mul(result.value, this->value, rhs.value, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand operator/(const Operand & rhs) {
            Operand result;
            mpfr_div(result.value, this->value, rhs.value, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand operator%(const Operand & rhs) {
            Operand result;
            mpfr_remainder(result.value, this->value, rhs.value, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }
};

#endif

#pragma once

#include <string>

#include <ctype.h>
#include <stdint.h>

#include "money.h"
#include "token.h"

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
            value = 0.0;
        }

    public:
        Money value;

        Operand() {
            initialiseValue();
        }

        Operand(const std::string & token) : Token(token) {
            initialiseValue();
            value = token;
        }

        Operand(Money src) {
            initialiseValue();
            value = src;
        }

        void clear() {
            value = 0.0;
        }

        static bool isOperand(const std::string & token) {
            for (size_t i = 0;i < token.length();i++) {
                char ch = token[i];

                if (!isOperandChar(ch)) {
                    return false;
                }
            }

            return true;
        }

        std::string toString() {
            return value.rawStringValue();
        }

        virtual std::string evaluate() override {
            return toString();
        }

        static const std::string CLASS_NAME() {
            return "Operand";
        }

        virtual const std::string className() override {
            return Operand::CLASS_NAME();
        }

        const Operand operator+(const Operand & rhs) {
            Operand result;
            result.value = this->value + rhs.value;
            result.setToken(result.toString());
            return result;
        }

        const Operand operator-(const Operand & rhs) {
            Operand result;
            result.value = this->value - rhs.value;
            result.setToken(result.toString());
            return result;
        }

        const Operand operator*(const Operand & rhs) {
            Operand result;
            result.value = this->value * rhs.value;
            result.setToken(result.toString());
            return result;
        }

        const Operand operator/(const Operand & rhs) {
            Operand result;
            result.value = this->value / rhs.value;
            result.setToken(result.toString());
            return result;
        }

        const Operand operator%(const Operand & rhs) {
            Operand result;
            result.value = this->value % rhs.value;
            result.setToken(result.toString());
            return result;
        }
};

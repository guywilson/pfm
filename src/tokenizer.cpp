#include <iostream>
#include <string>
#include <string.h>
#include <ctype.h>

#include "pfm_error.h"
#include "token.h"
#include "operand.h"
#include "operator.h"
#include "brace.h"
#include "container.h"
#include "tokenizer.h"

using namespace std;

static inline bool isCharDigit(char ch) {
    return (bool)isdigit((int)ch);
}

static inline bool isDelimiter(char ch) {
    static const char * delimiters = "+-*/^:%&|~<>()[]{}";

    for (int i = 0;i < (int)strlen(delimiters);i++) {
        if (ch == delimiters[i]) {
            return true;
        }
    }

    return false;
}

static inline bool isSpaceDelimiter(char ch) {
    return (bool)isspace((int)ch);
}

static inline bool isTokenWhiteSpace(string & token) {
    return isSpaceDelimiter(token[0]);
}

static bool isNegativeOperand(string & expression, int index) {
    /*
    ** If this is the '-' character and if the next char is a digit (0-9)
    ** and the previous char is not a ')' or a digit then this must be a -ve number,
    ** not the '-' operator...
    */
    char ch = expression[index];
    char next = expression[index + 1];
    char previous = expression[index - 1];

    if (ch == '-' && isdigit(next)) {
        if (index > 0) {
            if (!Brace::isRightBrace(previous) && !isCharDigit(previous)) {
                return true;
            }
            else {
                return false;
            }
        }
        else if (index == 0) {
            // We're at the beginning of the expression, must be
            // a -ve operand
            return true;
        }
    }

    return false;
}

int Tokenizer::findNextTokenPos() {
    char ch;
    int tokenLen = 0;

    for (int i = startIndex;i < (int)expression.length();i++) {
        ch = expression[i];

        if (isDelimiter(ch) || isSpaceDelimiter(ch)) {
            /*
            ** Do we have a token on it's own, or is it a delimiter...
            */
            if (tokenLen > 0) {
                // The token is the delimiter to another token...
                return i;
            }
            else {
                if (ch == '-') {
                    if (isNegativeOperand(expression, i)) {
                        continue;
                    }
                    else {
                        return i + 1;
                    }
                }
                else {
                    // The token is the token we want to return...
                    return i + 1;
                }
            }
        }

        tokenLen++;

        /*
        ** If we haven't returned yet and we're at the end of
        ** the expression, we must have an operand at the end
        ** of the expression...
        */
        if (i == (expression.length() - 1)) {
            return i + 1;
        }
    }

    return -1;
}

int Tokenizer::getTokenLength() {
    return (endIndex - startIndex);
}

Tokenizer::Tokenizer(const string & expression) {
    this->expression = expression;
    this->startIndex = 0;
    this->endIndex = 0;
}

TokenArray Tokenizer::tokenize() {
    TokenArray tokens;

    int pos = findNextTokenPos();

    while (pos > 0) {
        endIndex = pos;

        string token = expression.substr(startIndex, getTokenLength());
        
        startIndex = endIndex;

        if (!isTokenWhiteSpace(token)) {
            if (Operator::isOperator(token)) {
                Operator * t = new Operator(token);
                tokens.add(t);
            }
            else if (Operand::isOperand(token)) {
                Operand * t = new Operand(token);
                tokens.add(t);
            }
            else if (Brace::isBrace(token)) {
                Brace * t = new Brace(token);
                tokens.add(t);
            }
            else {
                throw calc_error(calc_error::buildMsg("Invalid token '%s'", token.c_str()));
            }
        }

        pos = findNextTokenPos();
    }

    return tokens;
}

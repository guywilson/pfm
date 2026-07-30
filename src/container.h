#include <string>
#include <vector>
#include <stack>
#include <queue>

#include <stdint.h>

#include "pfm_error.h"
#include "token.h"
#include "operand.h"
#include "operator.h"

using namespace std;

#ifndef __INCL_CONTAINER
#define __INCL_CONTAINER

class BaseContainer {
    public:
        BaseContainer() {}

        virtual bool isEmpty() = 0;
        virtual size_t size() = 0;
};

class TokenArray : public BaseContainer {
    private:
        vector<Token *> tokens;

    public:
        TokenArray() : BaseContainer() {}

        void add(Token * token) {
            tokens.push_back(token);
        }

        Token * at(uint32_t i) {
            if (tokens.size() == 0) {
                throw calc_error("Cannot get token from array as it is empty");
            }
            if (i > (tokens.size() - 1)) {
                throw calc_error("Cannot get token from array, index out of range");
            }
            
            return tokens.at(i);
        }

        bool isEmpty() override {
            return (tokens.empty());
        }

        size_t size() override {
            return tokens.size();
        }
};

class TokenStack : public BaseContainer {
    private:
        stack<Token *> tokens;

    public:
        TokenStack() : BaseContainer() {}

        void push(Token * token) {
            tokens.push(token);
        }

        bool isEmpty() override {
            return (tokens.empty());
        }

        size_t size() override {
            return tokens.size();
        }

        Token * peek() {
            return tokens.top();
        }

        Token * pop() {
            if (tokens.size() > 0) {
                Token * token = tokens.top();
                tokens.pop();

                return token;
            }
            else {
                throw calc_error("Cannot pop() token, stack is empty");
            }
        }

        Operand * poperand() {
            Token * token = pop();

            Operand * operand;
            if (token->className() == Operand::CLASS_NAME()) {
                operand = dynamic_cast<Operand *>(token);
            }
            else {
                throw calc_error("The requested operand token is not an operand");
            }

            return operand;
        }

        Operator * poperator() {
            Token * token = pop();

            Operator * op;
            if (token->className() == Operator::CLASS_NAME()) {
                op = dynamic_cast<Operator *>(token);
            }
            else {
                throw calc_error("The requested operator token is not an operator");
            }

            return op;
        }
};

class TokenQueue : public BaseContainer {
    private:
        queue<Token *> tokens;

    public:
        TokenQueue() : BaseContainer() {}

        bool isEmpty() override {
            return (tokens.empty());
        }

        size_t size() override {
            return tokens.size();
        }

        void put(Token * token) {
            tokens.push(token);
        }

        Token * get() {
            if (tokens.size() > 0) {
                Token * token = tokens.front();
                tokens.pop();

                return token;
            }
            else {
                throw calc_error("Cannot get() token, queue is empty");
            }
        }
};

#endif

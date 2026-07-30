#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <stack>

#include "pfm_error.h"
#include "token.h"
#include "operator.h"
#include "brace.h"
#include "container.h"
#include "tokenizer.h"
#include "expression.h"

using namespace std;

static inline bool isOperand(Token * t) {
    return (t->className() == Operand::CLASS_NAME());
}

static inline bool isOperator(Token * t) {
    return (t->className() == Operator::CLASS_NAME());
}

static inline bool isBrace(Token * t) {
    return (t->className() == Brace::CLASS_NAME());
}

TokenQueue Expression::getRPNQueue(TokenArray & tokens) {
    TokenQueue tokenQueue;
    TokenStack operatorStack;

    for (size_t i = 0;i < tokens.size();i++) {
        Token * token = tokens.at(i);

        if (isOperand(token)) {
            tokenQueue.put(token);
        }
        else if (isOperator(token)) {
            Operator * o1 = dynamic_cast<Operator *>(token);

            while (!operatorStack.isEmpty()) {
                Token * topToken = operatorStack.peek();

                if (!isOperator(topToken)) {
                    break;
                }

                Operator * o2 = dynamic_cast<Operator *>(topToken);

                if ((o1->getAssociativity() == Operator::aLeft && o1->getPrescedence() <= o2->getPrescedence()) ||
                    (o1->getAssociativity() == Operator::aRight && o1->getPrescedence() < o2->getPrescedence()))
                {
                    tokenQueue.put(operatorStack.pop());
                }
                else {
                    break;
                }
            }

            operatorStack.push(token);
        }
        else if (isBrace(token)) {
            Brace * brace = dynamic_cast<Brace *>(token);
            
            if (brace->isLeftBrace()) {
                operatorStack.push(brace);
            }
            else {
                /*
                If the token is a right parenthesis (i.e. ")"):
                    Until the token at the top of the stack is a left parenthesis, pop
                    operators off the stack onto the output queue.
                    Pop the left parenthesis from the stack, but not onto the output queue.
                    If the token at the top of the stack is a function token, pop it onto
                    the output queue.
                    If the stack runs out without finding a left parenthesis, then there
                    are mismatched parentheses.
                */
                bool foundLeftParenthesis = false;

                while (!operatorStack.isEmpty()) {
                    Token * stackToken = operatorStack.pop();

                    if (stackToken->className() == "Brace") {
                        Brace * b = dynamic_cast<Brace *>(stackToken);

                        if (b->isLeftBrace()) {
                            foundLeftParenthesis = true;
                            break;
                        }
                    }
                    else {
                        tokenQueue.put(stackToken);
                    }
                }

                if (!foundLeftParenthesis) {
                    throw calc_error("getRPNQueue(): Error unmatched parenthesis");
                }
            }
        }
        else {
            throw calc_error("getRPNQueue(): Invalid token");
        }
    }

    /*
        While there are still operator tokens in the stack:
        If the operator token on the top of the stack is a parenthesis,
        then there are mismatched parentheses.
        Pop the operator onto the output queue.
    */
    while (!operatorStack.isEmpty()) {
        Token * stackToken = operatorStack.pop();

        if (isBrace(stackToken)) {
            /*
            ** If we've got here, we must have unmatched parenthesis...
            */
            throw calc_error("getRPNQueue(): Error unmatched parenthesis");
        }
        else {
            tokenQueue.put(stackToken);
        }
    }

    return tokenQueue;
}

string Expression::evaluate(const string & expression) {
    Tokenizer tokenizer(expression);

    TokenArray tokens = tokenizer.tokenize();
    TokenQueue tokenQueue = getRPNQueue(tokens);

    TokenStack tokenStack;

    while (!tokenQueue.isEmpty()) {
        Token * t = tokenQueue.get();

        if (isOperand(t)) {
            tokenStack.push(t);
        }
        else if (isOperator(t)) {
            Operator * op = dynamic_cast<Operator *>(t);

            Operand * o2 = tokenStack.poperand();
            Operand * o1 = tokenStack.poperand();

            op->setOperands(*o1, *o2);
            string r = op->evaluate();

            o1->clear();
            delete o1;

            o2->clear();
            delete o2;

            Operand * result = new Operand(r);
            tokenStack.push(result);
        }
    }

    /*
    ** If there is one and only one item left on the stack,
    ** it is the result of the calculation. Otherwise, we
    ** have too many tokens and therefore an error...
    */
    string answer;

    if (tokenStack.size() == 1) {
        Operand * result = tokenStack.poperand();

        answer = result->toString(precision);

        result->clear();
        delete result;
    }
    else {
        throw calc_error("Invalid items on stack", __FILE__, __LINE__);
    }

    return answer;
}

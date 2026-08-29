#include <string>

#include <gmp.h>
#include <mpfr.h>

#include "pfm_error.h"
#include "token.h"
#include "operand.h"


#ifndef __INCL_OPERATOR
#define __INCL_OPERATOR

static inline bool isTokenPlus(const std::string & token) {
    return (token.compare("+") == 0);
}

static inline bool isTokenMinus(const std::string & token) {
    return (token.compare("-") == 0);
}

static inline bool isTokenMultiply(const std::string & token) {
    return (token.compare("*") == 0);
}

static inline bool isTokenDivide(const std::string & token) {
    return (token.compare("/") == 0);
}

static inline bool isTokenMod(const std::string & token) {
    return (token.compare("%") == 0);
}

static inline bool isTokenOperator(const std::string & token) {
    return (isTokenPlus(token) ||
            isTokenMinus(token) ||
            isTokenMultiply(token) ||
            isTokenDivide(token) ||
            isTokenMod(token));
}

class Operator : public Token {
    public:
        enum associativity {
            aLeft,
            aRight
        };

    private:
        enum op {
            operator_plus,
            operator_minus,
            operator_multiply,
            operator_divide,
            operator_mod,
            operator_unkown
        };

        Operand lhs;
        Operand rhs;
        op opType;

    protected:
        associativity opAssociativity;
        int opPrescedence;

    public:
        Operator() : Token() {
            opType = operator_unkown;
        }

        Operator(const std::string & token) : Token(token) {
            if (isTokenPlus(token)) {
                opType = operator_plus;
                opPrescedence = 2;
                opAssociativity = aLeft;
            }
            else if (isTokenMinus(token)) {
                opType = operator_minus;
                opPrescedence = 2;
                opAssociativity = aLeft;
            }
            else if (isTokenMultiply(token)) {
                opType = operator_multiply;
                opPrescedence = 3;
                opAssociativity = aLeft;
            }
            else if (isTokenDivide(token)) {
                opType = operator_divide;
                opPrescedence = 3;
                opAssociativity = aLeft;
            }
            else if (isTokenMod(token)) {
                opType = operator_mod;
                opPrescedence = 3;
                opAssociativity = aLeft;
            }
            else {
                opType = operator_unkown;
                opPrescedence = 0;
                opAssociativity = aLeft;
            }
        }

        static bool isOperator(const std::string & token) {
            return isTokenOperator(token);
        }

        static const std::string CLASS_NAME() {
            return "Operator";
        }

        virtual const std::string className() override {
            return Operator::CLASS_NAME();
        }

        int getPrescedence() {
            return opPrescedence;
        }

        associativity getAssociativity() {
            return opAssociativity;
        }

        void setLHOperand(Operand & l) {
            lhs = l;
        }

        void setRHOperand(Operand & r) {
            rhs = r;
        }

        void setOperands(Operand & lhs, Operand & rhs) {
            setLHOperand(lhs);
            setRHOperand(rhs);
        }

        std::string evaluate() override {
            Operand result;

            switch (opType) {
                case operator_plus:
                    result = lhs + rhs;
                    break;

                case operator_minus:
                    result = lhs - rhs;
                    break;

                case operator_multiply:
                    result = lhs * rhs;
                    break;

                case operator_divide:
                    result = lhs / rhs;
                    break;

                case operator_mod:
                    result = lhs % rhs;
                    break;

                default:
                    throw calc_error("Invalid operator");
            }

            return result.toString(INTERMEDIATE_PRECISION);
        }
};

#endif

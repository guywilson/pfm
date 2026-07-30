#include <string>
#include <vector>
#include <deque>
#include <iomanip>

#include "container.h"
#include "token.h"

using namespace std;

#ifndef __INCL_EXPRESSION
#define __INCL_EXPRESSION

// A custom facet to handle thousands separators
struct ThousandsSeparator : numpunct<char> {
    protected:
        char do_thousands_sep() const override {
            return ','; // Define the thousands separator as a comma
        }

        string do_grouping() const override {
            return "\3"; // Group digits in blocks of 3
        }
};

class Expression {
    private:
        long precision;

        TokenQueue getRPNQueue(TokenArray & tokens);

    public:
        Expression() {
            this->precision = 2;
        }

        string evaluate(const string & expression);
};

#endif

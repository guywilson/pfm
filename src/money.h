#include <iostream>
#include <string>

#include "db_currency.h"

using namespace std;

#ifndef __INCL_MONEY
#define __INCL_MONEY

class Money {
    private:
        int wholePart;
        int decimalPart;

        string currencySymbol;

    public:
        Money() {}
};

#endif

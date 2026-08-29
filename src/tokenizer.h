#include <string>
#include <vector>
#include <string.h>
#include <stdint.h>

#include "container.h"
#include "token.h"


#ifndef __INCL_TOKENIZER
#define __INCL_TOKENIZER

class Tokenizer {
    private:
        uint32_t startIndex;
        uint32_t endIndex;

        std::string expression;
        
        int findNextTokenPos();
        int getTokenLength();

    public:
        Tokenizer(const std::string & expression);

        TokenArray tokenize();
};

#endif

#include <string>

using namespace std;

#ifndef __INCL_TOKEN
#define __INCL_TOKEN

class Token {
    private:
        string token;

    protected:
        void setToken(const string & token) {
            this->token = token;
        }
        
    public:
        Token() {
            this->token = "";
        }

        Token(const string & token) {
            this->token = token;
        }

        virtual ~Token() {}

        virtual string evaluate() {
            return this->token;
        }

        virtual const string className() {
            return "Token";
        }
};

#endif

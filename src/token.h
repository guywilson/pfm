#include <string>


#ifndef __INCL_TOKEN
#define __INCL_TOKEN

class Token {
    private:
        std::string token;

    protected:
        void setToken(const std::string & token) {
            this->token = token;
        }
        
    public:
        Token() {
            this->token = "";
        }

        Token(const std::string & token) {
            this->token = token;
        }

        virtual ~Token() {}

        virtual std::string evaluate() {
            return this->token;
        }

        virtual const std::string className() {
            return "Token";
        }
};

#endif

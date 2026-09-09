/*
 * Copyright (C) 2025-2026 Guy Wilson
 *
 * This file is part of PFM.
 *
 * PFM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PFM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PFM. If not, see <https://www.gnu.org/licenses/>.
 */

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

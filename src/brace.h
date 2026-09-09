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

#include "token.h"


#ifndef __INCL_BRACE
#define __INCL_BRACE

class Brace : public Token {
    private:
        bool isLeft;

    public:
        Brace(const std::string & token) : Token(token) {
            if (Brace::isLeftBrace(token[0])) {
                isLeft = true;
            }
            else {
                isLeft = false;
            }
        }

        static const std::string CLASS_NAME() {
            return "Brace";
        }

        virtual const std::string className() override {
            return Brace::CLASS_NAME();
        }

        static bool isLeftBrace(char ch) {
            return (ch == '(' || ch == '[' || ch == '{');
        }

        static bool isRightBrace(char ch) {
            return (ch == ')' || ch == ']' || ch == '}');
        }

        static bool isBrace(char ch) {
            return (isLeftBrace(ch) || isRightBrace(ch));
        }

        static bool isBrace(const std::string & token) {
            return isBrace(token[0]);
        }

        bool isLeftBrace() {
            return isLeft;
        }
};

#endif

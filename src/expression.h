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

#pragma once

#include <string>
#include <vector>
#include <deque>
#include <iomanip>

#include "container.h"
#include "token.h"

// A custom facet to handle thousands separators
struct ThousandsSeparator : std::numpunct<char> {
    protected:
        char do_thousands_sep() const override {
            return ','; // Define the thousands separator as a comma
        }

        std::string do_grouping() const override {
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

        std::string evaluate(const std::string & expression);
};

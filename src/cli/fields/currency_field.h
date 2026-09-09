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

#include "text_field.h"
#include "../field.h"
#include "../../money.h"
#include "../../rlcustom.h"

class CLICurrencyField : public CLITextField {
    public:
        CLICurrencyField() : CLITextField() {
            setLengthLimit(AMOUNT_FIELD_STRING_LEN);
        }

        CLICurrencyField(const std::string & label) : CLITextField(label) {
            setLengthLimit(AMOUNT_FIELD_STRING_LEN);
        }

        void setDefaultValue(double value) {
            char szValue[AMOUNT_FIELD_STRING_LEN];

            snprintf(szValue, AMOUNT_FIELD_STRING_LEN, "%.2f", value);
            CLITextField::setDefaultValue(szValue);
        }

        void setDefaultValue(const std::string & value) {
            CLITextField::setDefaultValue(value);
        }

        void show() override {
            rl_utils::setLineLength(maxLength);

            std::string line = readLine();
            _setValue(line);
        }
};

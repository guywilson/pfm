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
#include <cstdlib>
#include <readline/readline.h>
#include <readline/history.h>

#include "../field.h"
#include "../constants.h"
#include "../../rlcustom.h"

class CLITextField : public CLIField {
    private:
        std::string defaultValue;

    protected:
        int maxLength = FIELD_STRING_LEN;

        std::string readLine(const std::string & prompt) {
            std::string text;

            char * line = readline(prompt.c_str());

            if (line != NULL && line[0] != 0) {
                text = line;
                free(line);
            }
            else if (defaultValue.length() > 0) {
                text = defaultValue;
            }

            return text;
        }

        std::string readLine() {
            std::string s = _getLabel();
            return readLine(s);
        }

    public:
        CLITextField() : CLIField() {}
        CLITextField(const std::string & label) : CLIField(label) {}

        void setLengthLimit(int numChars) {
            maxLength = numChars;
        }

        void setDefaultValue(const std::string & text) {
            defaultValue = text;
        }

        std::string getValue() override {
            std::string value = _getValue();

            if (value.length() == 0) {
                if (defaultValue.length() > 0) {
                    return defaultValue;
                }
            }

            return value;
        }

        void show() override {
            rl_utils::setLineLength(maxLength);
            std::string line = readLine();
            _setValue(line);
        }
};

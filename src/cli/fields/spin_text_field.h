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

#include "text_field.h"
#include "../field.h"
#include "../constants.h"
#include "../../rlcustom.h"

class CLISpinTextField : public CLITextField {
    private:
        std::vector<std::string> items;

    protected:
        void populate() {
            clear();

            if (items.size() > 0) {
                for (int i = 0;i < (int)items.size();i++) {
                    add_history(items[i].c_str());
                }
            }
        }

        void clear() {
            clear_history();
        }

    public:
        CLISpinTextField() : CLITextField() {
            setLengthLimit(CODE_FIELD_MAX_LENGTH);
        }

        CLISpinTextField(const std::string & label) : CLITextField(label) {
            setLengthLimit(CODE_FIELD_MAX_LENGTH);
        }

        void addItem(const std::string & item) {
            items.push_back(item);
        }

        void show() override {
            populate();

            rl_utils::setLineLength(maxLength);
            std::string line = readLine();
            _setValue(line);

            clear();
        }
};

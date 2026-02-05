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
        vector<std::string> items;

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

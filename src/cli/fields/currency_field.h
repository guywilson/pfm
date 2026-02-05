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

        void setDefaultValue(const string & value) {
            CLITextField::setDefaultValue(value);
        }

        void show() override {
            rl_utils::setLineLength(maxLength);

            string line = readLine();
            _setValue(line);
        }
};

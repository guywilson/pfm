#pragma once
#include <string>

#include "widget.h"
#include "validation.h"

class CLIField : public CLIWidget {
    private:
        std::string value;
        std::string label;

    protected:
        std::string _getLabel() {
            return label;
        }

        std::string _getValue() {
            return value;
        }

        void _setValue(const std::string value) {
            cli::validation::validateField(value);
            this->value = value;
        } 

    public:
        CLIField() : CLIWidget() {}

        CLIField(const string & label) {
            this->label = label;
        }

        void setLabel(const string & label) {
            this->label = label;
        }

        virtual string getValue() {
            return _getValue();
        }

        double getDoubleValue() {
            return strtod(getValue().c_str(), NULL);
        }

        int32_t getIntegerValue() {
            return (int32_t)strtol(getValue().c_str(), NULL, 10);
        }

        uint32_t getUnsignedIntegerValue() {
            return (uint32_t)strtoul(getValue().c_str(), NULL, 10);
        }

        long getLongValue() {
            return strtol(getValue().c_str(), NULL, 10);
        }

        unsigned long getUnsignedLongValue() {
            return strtoul(getValue().c_str(), NULL, 10);
        }

        int64_t getInt64Value() {
            return (int64_t)strtoll(getValue().c_str(), NULL, 10);
        }

        void show() override {}
};

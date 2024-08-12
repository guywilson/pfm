#include <string>
#include <ctype.h>

#include "cli_widget.h"
#include "strdate.h"
#include "db_category.h"
#include "db_payee.h"

using namespace std;

#ifndef __INCL_CUSTOM_WIDGETS
#define __INCL_CUSTOM_WIDGETS

#define CODE_FIELD_MAX_LENGTH                    5
#define DATE_FIELD_LENGTH                       10

class CategorySpinField : public CLISpinTextField {
    public:
        CategorySpinField() : CLISpinTextField() {}
        CategorySpinField(string & label) : CLISpinTextField(label) {}
        CategorySpinField(const char * label) : CLISpinTextField(label) {}

        void show() override {
            setLengthLimit(CODE_FIELD_MAX_LENGTH);

            DBCategory category;
            DBCategoryResult result = category.retrieveAll();

            for (int i = 0;i < result.getNumRows();i++) {
                DBCategory c = result.getResultAt(i);
                addItem(c.code);
            }

            populate();

            string line = readLine();
            _setValue(line);

            clear();
        }

        DBCategory getCategory() {
            string code = getValue();

            DBCategory category;
            category.retrieveByCode(code);

            return category;
        }
};

class PayeeSpinField : public CLISpinTextField {
    public:
        PayeeSpinField() : CLISpinTextField() {}
        PayeeSpinField(string & label) : CLISpinTextField(label) {}
        PayeeSpinField(const char * label) : CLISpinTextField(label) {}

        void show() override {
            DBPayee payee;
            DBPayeeResult result = payee.retrieveAll();

            for (int i = 0;i < result.getNumRows();i++) {
                DBPayee p = result.getResultAt(i);
                addItem(p.code);
            }

            populate();

            string line = readLine();
            _setValue(line);

            clear();
        }

        DBPayee getPayee() {
            string code = getValue();

            DBPayee payee;
            payee.retrieveByCode(code);

            return payee;
        }
};

class DateField : public CLITextField {
    private:
        const int maxAttemps = 5;

    public:
        DateField() : CLITextField() {}
        DateField(string & label) : CLITextField(label) {}
        DateField(const char * label) : CLITextField(label) {}

        void show() override {
            setLengthLimit(DATE_FIELD_LENGTH);

            bool isDateValid = false;
            int attempts = 0;
            string line;

            while (!isDateValid && attempts < maxAttemps) {
                line = readLine();
                isDateValid = StrDate::validateDate(line);

                attempts++;
            }

            if (isDateValid) {
                _setValue(line);
            }
            else {
                throw pfm_validation_error(
                                pfm_error::buildMsg(
                                    "Invalid date '%s', must be of the form 'yyyy-mm-dd'",
                                    line.c_str()
                                ),
                                __FILE__,
                                __LINE__);
            }
        }
};

class FrequencyField : public CLITextField {
    private:
        const int maxAttemps = 5;

        bool validate(string & frequency) {
            if (!isLengthValid(frequency) || !isFirstCharValid(frequency) || !isLastCharValid(frequency)) {
                return false;
            }

            return true;
        }

        bool isLengthValid(string & frequency) {
            return (frequency.length() >= 2);
        }

        bool isFirstCharValid(string & frequency) {
            return (isdigit(frequency.at(0)));
        }

        bool isLastCharValid(string & frequency) {
            if (frequency.back() != 'w' && 
                frequency.back() != 'm' && 
                frequency.back() != 'y' &&
                frequency.back() != 'd')
            {
                return false;
            }
            else {
                return true;
            }
        }

    public:
        FrequencyField() : CLITextField() {}
        FrequencyField(string & label) : CLITextField(label) {}
        FrequencyField(const char * label) : CLITextField(label) {}

        void show() override {
            setLengthLimit(4);

            bool isFrequencyValid = false;
            int attempts = 0;
            string line;

            while (!isFrequencyValid && attempts < maxAttemps) {
                line = readLine();
                isFrequencyValid = validate(line);

                attempts++;
            }

            if (isFrequencyValid) {
                _setValue(line);
            }
            else {
                throw pfm_validation_error(
                                pfm_error::buildMsg(
                                    "Invalid frequency '%s', must be of the form 'x[d|w|m|y]'",
                                    line.c_str()
                                ),
                                __FILE__,
                                __LINE__);
            }
        }
};

#endif

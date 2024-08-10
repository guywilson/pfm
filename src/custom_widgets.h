#include <string>
#include <ctype.h>

#include "cli_widget.h"
#include "strdate.h"
#include "db_category.h"
#include "db_payee.h"

using namespace std;

#ifndef __INCL_CUSTOM_WIDGETS
#define __INCL_CUSTOM_WIDGETS

#define FIELD_STRING_LEN                        64
#define MAX_PROMPT_LENGTH                      128
#define AMOUNT_FIELD_STRING_LEN                 16
#define CODE_FIELD_MAX_LENGTH                    5
#define DATE_FIELD_LENGTH                       10

class CategorySpinField : public CLISpinTextField {
    public:
        CategorySpinField() : CLISpinTextField() {}
        CategorySpinField(string & label) : CLISpinTextField(label) {}
        CategorySpinField(const char * label) : CLISpinTextField(label) {}

        void show() override {
            DBCategory category;
            DBCategoryResult result = category.retrieveAll();

            for (int i = 0;i < result.getNumRows();i++) {
                DBCategory c = result.getResultAt(i);
                addItem(c.code);
            }

            populate();
            readLine();
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
            readLine();
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

            while (!isDateValid && attempts < maxAttemps) {
                readLine();
                isDateValid = StrDate::validateDate(getValue());

                attempts++;
            }

            if (!isDateValid) {
                throw pfm_validation_error(
                                pfm_error::buildMsg(
                                    "Invalid date '%s', must be of the form 'yyyy-mm-dd'",
                                    getValue().c_str()
                                ),
                                __FILE__,
                                __LINE__);
            }
        }
};

class FrequencyField : public CLITextField {
    private:
        const int maxAttemps = 5;

        bool validate() {
            string frequency = getValue();

            if (!isLengthValid() || !isFirstCharValid() || !isLastCharValid()) {
                return false;
            }

            return true;
        }

        bool isLengthValid() {
            return (getValue().length() >= 2);
        }

        bool isFirstCharValid() {
            return (isdigit(getValue().at(0)));
        }

        bool isLastCharValid() {
            string frequency = getValue();

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
            bool isFrequencyValid = false;
            int attempts = 0;

            while (!isFrequencyValid && attempts < maxAttemps) {
                readLine();
                isFrequencyValid = validate();

                attempts++;
            }

            if (!isFrequencyValid) {
                throw pfm_validation_error(
                                pfm_error::buildMsg(
                                    "Invalid frequency '%s', must be of the form 'x[d|w|m|y]'",
                                    getValue().c_str()
                                ),
                                __FILE__,
                                __LINE__);
            }
        }
};

#endif

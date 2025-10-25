#include <string>
#include <ctype.h>

#include "cli_widget.h"
#include "strdate.h"
#include "db_category.h"
#include "db_payee.h"

using namespace std;

#ifndef __INCL_CUSTOM_WIDGETS
#define __INCL_CUSTOM_WIDGETS

#define DATE_FIELD_LENGTH                       10

class CategorySpinField : public CLISpinTextField {
    private:
        DBCategory category;

    public:
        CategorySpinField() : CLISpinTextField() {}
        CategorySpinField(string & label) : CLISpinTextField(label) {}
        CategorySpinField(const char * label) : CLISpinTextField(label) {}

        void show() override {
            rl_utils::setLineLength(maxLength);

            DBResult<DBCategory> result;
            result.retrieveAll();

            for (int i = 0;i < result.size();i++) {
                DBCategory category = result.at(i);
                addItem(category.code);
            }

            populate();

            string code = readLine();
            _setValue(code);

            category.retrieveByCode(code);

            if (category.id.isNull()) {
                category.code = code;

                rl_utils::setLineLength(FIELD_STRING_LEN);
                category.description = readLine("Category description: ");

                if (category.code.length() > 0 && category.description.length() > 0) {
                    category.save();
                }
            }

            clear();
        }

        DBCategory getCategory() {
            return category;
        }
};

class PayeeSpinField : public CLISpinTextField {
    private:
        DBPayee payee;

    public:
        PayeeSpinField() : CLISpinTextField() {}
        PayeeSpinField(string & label) : CLISpinTextField(label) {}
        PayeeSpinField(const char * label) : CLISpinTextField(label) {}

        void show() override {
            DBResult<DBPayee> result;
            result.retrieveAll();

            for (int i = 0;i < result.size();i++) {
                DBPayee payee = result.at(i);
                addItem(payee.code);
            }

            populate();

            string code = readLine();
            _setValue(code);

            payee.retrieveByCode(code);

            if (payee.id.isNull()) {
                payee.code = code;

                rl_utils::setLineLength(FIELD_STRING_LEN);
                payee.name = readLine("Payee name: ");

                if (payee.code.length() > 0 && payee.name.length() > 0) {
                    payee.save();
                }
            }

            clear();
        }

        DBPayee getPayee() {
            return payee;
        }
};

class AccountSpinField : public CLISpinTextField {
    private:
        DBAccount account;

    public:
        AccountSpinField() : CLISpinTextField() {}
        AccountSpinField(string & label) : CLISpinTextField(label) {}
        AccountSpinField(const char * label) : CLISpinTextField(label) {}

        void show() override {
            DBResult<DBAccount> result;
            result.retrieveAll();

            for (int i = 0;i < result.size();i++) {
                DBAccount account = result.at(i);
                addItem(account.code);
            }

            populate();

            string code = readLine();

            if (code.length() > 0) {
                _setValue(code);

                account.retrieveByCode(code);
            }

            clear();
        }

        DBAccount getAccount() {
            return account;
        }
};

class DateField : public CLITextField {
    private:
        const int maxAttemps = 5;

        void buildHistoryDateItem(int year, int month) {
            char historyDateBuffer[DATE_STAMP_BUFFER_LEN];

            snprintf(historyDateBuffer, DATE_STAMP_BUFFER_LEN, "%d-%02d-", year, month);
            add_history(historyDateBuffer);
        }

        void populateDateHistory() {
            clear_history();

            StrDate today;
            int year = today.year();
            int month = today.month() + 1;

            int monthCounter = 0;

            while (monthCounter < 12) {
                if (month > 12) {
                    month = 1;
                }

                buildHistoryDateItem(year, month++);

                monthCounter++;
            }
        }

    public:
        DateField() : CLITextField() {
            setLengthLimit(DATE_FIELD_LENGTH);
        }

        DateField(string & label) : CLITextField(label) {
            setLengthLimit(DATE_FIELD_LENGTH);
        }

        DateField(const char * label) : CLITextField(label) {
            setLengthLimit(DATE_FIELD_LENGTH);
        }

        void show() override {
            populateDateHistory();

            rl_utils::setLineLength(maxLength);
            
            bool isDateValid = false;
            int attempts = 0;
            string line;

            while (!isDateValid && attempts < maxAttemps) {
                line = readLine();
                isDateValid = StrDate::isDateValid(line);

                attempts++;
            }

            if (isDateValid) {
                _setValue(line);
            }
            else {
                /*
                ** Set the date, will throw an error as the
                ** date is invalid...
                */
                StrDate errorDate(line);
            }

            clear_history();
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

#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <unordered_map>
#include <readline/readline.h>
#include <readline/history.h>

using namespace std;

#ifndef __CLI_WIDGET
#define __CLI_WIDGET

#define FIELD_STRING_LEN                        64
#define MAX_PROMPT_LENGTH                      128
#define AMOUNT_FIELD_STRING_LEN                 16

class CLIWidget {
    public:
        virtual void show() = 0;
};

class CLIField : public CLIWidget {
    private:
        string value;
        string label;

    protected:
        void readLine() {
            char * line = NULL;

            line = readline(label.c_str());

            if (line != NULL && line[0] != 0) {
                value = line;
                free(line);
            }
            else {
                value = "";
            }
        }

        string _getValue() {
            return value;
        }

    public:
        CLIField() : CLIWidget() {}

        CLIField(string & label) {
            this->label = label;
        }

        CLIField(const char * label) {
            this->label = label;
        }

        void setLabel(string & label) {
            this->label = label;
        }

        void setLabel(const char * label) {
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

        void show() {
            readLine();
        }
};

class CLITextField : public CLIField {
    private:
        int maxLength = FIELD_STRING_LEN;
        string defaultValue;

    public:
        CLITextField() : CLIField() {}
        CLITextField(string & label) : CLIField(label) {}
        CLITextField(const char * label) : CLIField(label) {}

        void setLengthLimit(int numChars) {
            maxLength = numChars;
        }

        void setDefaultValue(string & text) {
            defaultValue = text;
        }

        void setDefaultValue(const char * text) {
            defaultValue = text;
        }

        string getValue() override {
            string value = _getValue();

            if (value.length() == 0) {
                if (defaultValue.length() > 0) {
                    return defaultValue;
                }
            }

            return value;
        }
};

class CLICurrencyField : public CLITextField {
    public:
        CLICurrencyField() : CLITextField() {}
        CLICurrencyField(string & label) : CLITextField(label) {}
        CLICurrencyField(const char * label) : CLITextField(label) {}

        void setDefaultValue(double value) {
            char szValue[AMOUNT_FIELD_STRING_LEN];

            snprintf(szValue, AMOUNT_FIELD_STRING_LEN, "%.2f", value);
            CLITextField::setDefaultValue(szValue);
        }

        void show() override {
            setLengthLimit(AMOUNT_FIELD_STRING_LEN);

            readLine();
        }
};

class CLISpinTextField : public CLITextField {
    private:
        vector<string> items;

    protected:
        void populate() {
            clear();

            if (items.size() > 0) {
                for (int i = 0;i < items.size();i++) {
                    add_history(items[i].c_str());
                }
            }
        }

        void clear() {
            clear_history();
        }

    public:
        CLISpinTextField() : CLITextField() {}
        CLISpinTextField(string & label) : CLITextField(label) {}
        CLISpinTextField(const char * label) : CLITextField(label) {}

        void addItem(string & item) {
            items.push_back(item);
        }

        void show() override {
            populate();
            readLine();
            clear();
        }
};

class CLIView : public CLIWidget {
    private:
        string title;

    protected:
        void printTitle() {
            cout << "*** " << title << " ***" << endl;
        }

    public:
        CLIView() : CLIWidget() {}

        CLIView(string & title) : CLIWidget() {
            setTitle(title);
        }

        CLIView(const char * szTitle) : CLIWidget() {
            setTitle(szTitle);
        }

        void setTitle(string & title) {
            this->title = title;
        }

        void setTitle(const char * szTitle) {
            this->title = szTitle;
        }

        void show(const char * szTitle) {
            string t = szTitle;
            show(t);
        }

        void show(string & title) {
            printTitle();
        }

        void show() override {
            show(this->title);
        }
};

#endif

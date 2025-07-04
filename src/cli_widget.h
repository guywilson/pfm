#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <unordered_map>
#include <readline/readline.h>
#include <readline/history.h>

#include "money.h"
#include "pfm_error.h"
#include "rlcustom.h"

using namespace std;

#ifndef __CLI_WIDGET
#define __CLI_WIDGET

#define FIELD_STRING_LEN                        64
#define MAX_PROMPT_LENGTH                      128
#define AMOUNT_FIELD_STRING_LEN                 16
#define TITLE_BUFFER_LEN                       128

#define CLI_CANCEL_KEY                         'x'
#define SINGLE_QUOTE_CHAR                       39


#if defined(__APPLE__) || defined(__unix__)
std::ostream& bold_on(std::ostream& os) {
    return os << "\e[1m";
}

std::ostream& bold_off(std::ostream& os) {
    return os << "\e[0m";
}
#else
std::ostream& bold_on(std::ostream& os) {
    return os << "";
}

std::ostream& bold_off(std::ostream& os) {
    return os << "";
}
#endif

class CLIWidget {
    public:
        CLIWidget() {}
        virtual ~CLIWidget() {};

        virtual void show() = 0;
};

class CLIField : public CLIWidget {
    private:
        string value;
        string label;

        string makeUpperCase(string s) {
            for (int i = 0;i < (int)s.length();i++) {
                s[i] = (char)toupper(s.at(i));
            }

            return s;
        }

        bool doesExistInString(string & src, const char * checkStr) {
            string upper = makeUpperCase(src);

            if (upper.find(checkStr) == string::npos) {
                return false;
            }

            return true;
        }

        bool containsInsert(string & fieldValue) {
            return doesExistInString(fieldValue, "INSERT INTO ");
        }

        bool containsUpdate(string & fieldValue) {
            return doesExistInString(fieldValue, " UPDATE ");
        }

        bool containsDelete(string & fieldValue) {
            return doesExistInString(fieldValue, "DELETE FROM ");
        }

        bool containsJoin(string & fieldValue) {
            return doesExistInString(fieldValue, " JOIN ");
        }

        bool containsOr(string & fieldValue) {
            return doesExistInString(fieldValue, " OR ");
        }

        bool containsDrop(string & fieldValue) {
            return doesExistInString(fieldValue, " DROP ");
        }

        bool containsAlter(string & fieldValue) {
            return doesExistInString(fieldValue, " ALTER ");
        }

    protected:
        void validateField(string & fieldValue) {
            if (containsInsert(fieldValue) || 
                containsUpdate(fieldValue) || 
                containsDelete(fieldValue) || 
                containsJoin(fieldValue) || 
                containsOr(fieldValue) ||
                containsDrop(fieldValue) ||
                containsAlter(fieldValue))
            {
                throw pfm_error("Invalid field value, SQL keywords are not permitted");
            }
        }

        string _getLabel() {
            return label;
        }

        string _getValue() {
            return value;
        }

        void _setValue(string value) {
            validateField(value);
            this->value = value;
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

        void show() override {}
};

class CLITextField : public CLIField {
    private:
        int maxLength = FIELD_STRING_LEN;
        string defaultValue;

        uint64_t findSingleQuotePos(string & s, int startingPos = 0) {
            uint64_t pos = s.find(SINGLE_QUOTE_CHAR, startingPos);

            if (pos != string::npos) {
                if (s.at(pos + 1) != SINGLE_QUOTE_CHAR) {
                    return pos;
                }
            }

            return string::npos;
        }

        int delimitSingleQuotes(string & s) {
            uint64_t searchPos = 0;
            int numQuotesFound = 0;

            searchPos = findSingleQuotePos(s);

            while (searchPos != string::npos) {
                numQuotesFound++;

                s.insert(searchPos, 1, SINGLE_QUOTE_CHAR);

                searchPos = findSingleQuotePos(s, searchPos + 2);
            }

            return numQuotesFound;
        }

    protected:
        string readLine(const char * prompt) {
            string text;

            char * line = readline(prompt);

            if (line != NULL && line[0] != 0) {
                text = line;
                free(line);
            }
            else if (defaultValue.length() > 0) {
                text = defaultValue;
            }

            return text;
        }

        string readLine(string & prompt) {
            return readLine(prompt.c_str());
        }

        string readLine() {
            string s = _getLabel();
            return readLine(s);
        }

    public:
        CLITextField() : CLIField() {}
        CLITextField(string & label) : CLIField(label) {}
        CLITextField(const char * label) : CLIField(label) {}

        void setLengthLimit(int numChars) {
            rl_utils::setLineLength(numChars);
            maxLength = numChars;
        }

        void setDefaultValue(const string & text) {
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

        void show() override {
            string line = readLine();
            delimitSingleQuotes(line);
            _setValue(line);
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

        void setDefaultValue(string value) {
            CLITextField::setDefaultValue(value);
        }

        void show() override {
            setLengthLimit(AMOUNT_FIELD_STRING_LEN);

            string line = readLine();
            _setValue(line);
        }
};

class CLISpinTextField : public CLITextField {
    private:
        vector<string> items;

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
        CLISpinTextField() : CLITextField() {}
        CLISpinTextField(string & label) : CLITextField(label) {}
        CLISpinTextField(const char * label) : CLITextField(label) {}

        void addItem(string & item) {
            items.push_back(item);
        }

        void show() override {
            populate();

            string line = readLine();
            _setValue(line);

            clear();
        }
};

class CLIView : public CLIWidget {
    private:
        string title;

    protected:
        void printTitle() {
            cout << "*** " << title << " ***" << endl << endl;
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

class CLIListColumn : public CLIField {
    public:
        typedef enum {
            leftAligned,
            rightAligned
        }
        alignment;

    private:
        int width;
        alignment align;

        int getNumPaddingChars(string & value) {
            return (getWidth() - value.length());
        }

        int printPadding(string & value) {
            int numPaddingChars = getNumPaddingChars(value);

            for (int i = 0;i < numPaddingChars;i++) {
                cout << " ";
            }

            return numPaddingChars;
        }

    public:
        CLIListColumn() : CLIField() {}
        CLIListColumn(const char * name, int width, CLIListColumn::alignment align) : CLIField(name) {
            this->width = width;
            this->align = align;

            if ((width + 2) < (int)strlen(name)) {
                throw pfm_error("Column name is too long for specified width");
            }
        }

        CLIListColumn(string & name, int width, CLIListColumn::alignment align) : CLIField(name) {
            this->width = width;
            this->align = align;

            if ((width + 2) < (int)name.length()) {
                throw pfm_error("Column name is too long for specified width");
            }
        }
        
        int getColumnWidth() {
            string columnName = getName();

            return (columnName.length() + getNumPaddingChars(columnName) + 3);
        }

        void printColumnHeader() {
            string name = getName();

            cout << name;
            printPadding(name);
            cout << " | ";
        }

        void printCell(string value) {
            int width = getWidth();
            string v = value;

            if (value[0] == '#') {
                width++;
                v = v.substr(1);
            }

            if (width < (int)v.length()) {
                v = v.substr(0, width - 3);
                v = v.append("...");
            }

            switch (getAlignment()) {
                case CLIListColumn::leftAligned:
                    cout << left << setw(width) << v;
                    break;

                case CLIListColumn::rightAligned:
                    cout << right << setw(width) << v;
                    break;
            }

            cout << " | ";
        }

        string getName() {
            return _getLabel();
        }

        alignment getAlignment() {
            return align;
        }

        int getWidth() {
            return width;
        }

        void show() override {}
};

class CLIListRow : public CLIWidget {
    private:
        vector<CLIListColumn> columnDefintions;
        vector<string> columnValues;

    public:
        CLIListRow() : CLIWidget() {}

        CLIListRow(const CLIListRow & row) {
            for (int i = 0;i < row.getNumColumns();i++) {
                addColumn(row.getColumnAt(i));
            }

            for (int i = 0;i < row.getNumValues();i++) {
                addCellValue(row.getValueAt(i));
            }
        }

        void printTopBorder() {
            cout << "+";

            for (int i = 0;i < getNumColumns();i++) {
                CLIListColumn column = getColumnAt(i);

                for (int j = 0;j < column.getColumnWidth() - 1;j++) {
                    cout << '-';
                }

                cout << '+';
            }

            cout << endl;
        }

        void printBottomBorder() {
            printTopBorder();
        }

        int getNumColumns() const {
            return columnDefintions.size();
        }

        int getNumValues() const {
            return columnValues.size();
        }

        CLIListColumn getColumnAt(int i) const {
            if (i < getNumColumns()) {
                return columnDefintions[i];
            }
            else {
                throw pfm_error("CLIListRow::getColumnAt() - Index out of range");
            }
        }

        string getValueAt(int i) const {
            if (i < getNumValues()) {
                return columnValues[i];
            }
            else {
                throw pfm_error("CLIListRow::getValueAt() - Index out of range");
            }
        }

        void addColumn(const CLIListColumn & column) {
            columnDefintions.push_back(column);
        }

        void addCellValue(const string & value) {
            columnValues.push_back(value);
        }

        void addCellValue(const char * szValue) {
            string value = szValue;
            columnValues.push_back(value);
        }

        void addCellValue(Money & val) {
            /*
            ** Handle 2-byte characters, e.g. currency symbols,
            ** width is adjusted in printCell() for Money cells.
            ** Changes specified in P2675 should fix this:
            ** https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2675r1.pdf
            */
            string value = "#" + val.getFormattedStringValue();
            columnValues.push_back(value);
        }

        void addCellValue(double val) {
            char buffer[32];

            snprintf(buffer, 32, "%.2f", val);
            string value = buffer;
            columnValues.push_back(value);
        }

        void addCellValue(int32_t val) {
            char buffer[32];

            snprintf(buffer, 32, "%d", val);
            string value = buffer;
            columnValues.push_back(value);
        }

        void addCellValue(uint32_t val) {
            char buffer[32];

            snprintf(buffer, 32, "%u", val);
            string value = buffer;
            columnValues.push_back(value);
        }

        void addCellValue(int64_t val) {
            char buffer[32];

            snprintf(buffer, 32, "%lld", val);
            string value = buffer;
            columnValues.push_back(value);
        }

        void addCellValue(uint64_t val) {
            char buffer[32];

            snprintf(buffer, 32, "%llu", val);
            string value = buffer;
            columnValues.push_back(value);
        }

        void showHeaderRow() {
            printTopBorder();

            cout << "| ";

            for (int i = 0;i < getNumColumns();i++) {
                CLIListColumn column = getColumnAt(i);
                column.printColumnHeader();
            }

            cout << endl;

            printTopBorder();
        }

        void show() override {
            cout << "| ";

            for (int i = 0;i < getNumColumns();i++) {
                CLIListColumn column = getColumnAt(i);
                string value = getValueAt(i);

                column.printCell(value);
            }

            cout << endl;
        }
};

class CLIListView : public CLIView {
    private:
        CLIListRow headerRow;
        vector<CLIListRow> dataRows;
    
    public:
        CLIListView() : CLIView() {}
        CLIListView(string & title) : CLIView(title) {}
        CLIListView(const char * szTitle) : CLIView(szTitle) {}

        void addHeaderRow(CLIListRow & header) {
            headerRow = header;
        }

        void addRow(CLIListRow & row) {
            dataRows.push_back(row);
        }

        void showBottomBorder() {
            headerRow.printBottomBorder();
        }

        void showNoExtraCR() {
            printTitle();

            headerRow.showHeaderRow();

            for (int i = 0;i < (int)dataRows.size();i++) {
                dataRows[i].show();
            }

            if (!dataRows.empty()) {
                showBottomBorder();
            }
        }

        void show() override {
            showNoExtraCR();
            cout << endl;
        }
};

class CLIFindView : public CLIView {
    public:
        CLIFindView() : CLIView() {}
        CLIFindView(string & title) : CLIView(title) {}
        CLIFindView(const char * szTitle) : CLIView(szTitle) {}

        void show() override {
            CLIView::show();
        }

        virtual string getCriteria() {
            return "";
        }
};

#endif

#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <unordered_map>
#include <readline/readline.h>
#include <readline/history.h>
#include <inttypes.h>

#include "money.h"
#include "strdate.h"
#include "cfgmgr.h"
#include "pfm_error.h"
#include "rlcustom.h"
#include "bold_modifier.h"

using namespace std;

#ifndef __CLI_WIDGET
#define __CLI_WIDGET

#define FIELD_STRING_LEN                        64
#define MAX_PROMPT_LENGTH                      128
#define AMOUNT_FIELD_STRING_LEN                 16
#define TITLE_BUFFER_LEN                       128
#define CODE_FIELD_MAX_LENGTH                    5
#define CRITERIA_FIELD_MAX_LEN                 256

#define LIST_VIEW_SEQUENCE_WIDTH                2
#define LIST_VIEW_CODE_WIDTH                    CODE_FIELD_MAX_LENGTH
#define LIST_VIEW_AMOUNT_WIDTH                  (AMOUNT_FIELD_STRING_LEN - 3)
#define LIST_VIEW_DESCRIPTION_WIDTH             20
#define LIST_VIEW_REFERENCE_WIDTH               9
#define LIST_VIEW_TYPE_WIDTH                    2
#define LIST_VIEW_RECONCILED_WIDTH              1
#define LIST_VIEW_RECURRING_CHARGE_WIDTH        1
#define LIST_VIEW_TRANSFER_WIDTH                1
#define LIST_VIEW_FREQUENCY_WIDTH               3

#define CLI_CANCEL_KEY                         'x'

static int countMultiByteChars(const string & str) {
    int i = 0;
    for (unsigned char c : str) {
        if (c & 0x80) {
            i++;
        }
    }

    return i;
}

static int calculateFieldWidth(const string & value, int baseWidth) {
    /*
    ** Handle 2-byte characters, e.g. currency symbols,
    ** width is adjusted here for such strings.
    ** Changes specified in P2675 should fix this:
    ** https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2675r1.pdf
    */
    int width = baseWidth;
    int numMultiByteChars = countMultiByteChars(value);

    if (numMultiByteChars > 0) {
        if (numMultiByteChars >= 2) {
            numMultiByteChars--;
        }

        width += numMultiByteChars;
    }

    return width;
}

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

class CLITextField : public CLIField {
    private:
        string defaultValue;

    protected:
        int maxLength = FIELD_STRING_LEN;

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
            rl_utils::setLineLength(maxLength);
            string line = readLine();
            _setValue(line);
        }
};

class CLICurrencyField : public CLITextField {
    public:
        CLICurrencyField() : CLITextField() {
            setLengthLimit(AMOUNT_FIELD_STRING_LEN);
        }

        CLICurrencyField(string & label) : CLITextField(label) {
            setLengthLimit(AMOUNT_FIELD_STRING_LEN);
        }

        CLICurrencyField(const char * label) : CLITextField(label) {
            setLengthLimit(AMOUNT_FIELD_STRING_LEN);
        }

        void setDefaultValue(double value) {
            char szValue[AMOUNT_FIELD_STRING_LEN];

            snprintf(szValue, AMOUNT_FIELD_STRING_LEN, "%.2f", value);
            CLITextField::setDefaultValue(szValue);
        }

        void setDefaultValue(string value) {
            CLITextField::setDefaultValue(value);
        }

        void show() override {
            rl_utils::setLineLength(maxLength);

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
        CLISpinTextField() : CLITextField() {
            setLengthLimit(CODE_FIELD_MAX_LENGTH);
        }

        CLISpinTextField(string & label) : CLITextField(label) {
            setLengthLimit(CODE_FIELD_MAX_LENGTH);
        }

        CLISpinTextField(const char * label) : CLITextField(label) {
            setLengthLimit(CODE_FIELD_MAX_LENGTH);
        }

        void addItem(string & item) {
            items.push_back(item);
        }

        void show() override {
            populate();

            rl_utils::setLineLength(maxLength);
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
        enum column_alignment {
            leftAligned,
            rightAligned
        };

    private:
        int width;
        column_alignment align;

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
        CLIListColumn(const string & name, int width, CLIListColumn::column_alignment align) : CLIField(name) {
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

        void printCell(string & value) {
            int width = getWidth();
            string v = value;

            width = calculateFieldWidth(value, width);

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

        inline column_alignment getAlignment() {
            return align;
        }

        int getWidth() {
            return width;
        }

        void show() override {}
};

class CLIListRow : public CLIWidget {
    private:
        vector<string> cells;
        size_t numColumns;

        static const int CONVERT_BUFFER_SIZE = 32;
        char convertBuffer[CONVERT_BUFFER_SIZE];

        inline string convertBufferString() {
            return string(convertBuffer);
        }

    public:
        CLIListRow(const size_t columns) {
            numColumns = columns;
            cells.reserve(columns);
        }

        void addCell(const string & value) {
            cells.push_back(value);

            if (cells.size() > numColumns) {
                throw pfm_error(pfm_error::buildMsg("Adding cell %zu to row, expected %zu columns", cells.size(), numColumns));
            }
        }

        void addCell(Money & value) {
            addCell(value.localeFormattedStringValue());
        }

        void addCell(StrDate & value) {
            addCell(value.shortDate());
        }

        void addCell(double val) {
            snprintf(convertBuffer, CONVERT_BUFFER_SIZE, "%.2f", val);
            addCell(convertBufferString());
        }

        void addCell(int32_t val) {
            snprintf(convertBuffer, CONVERT_BUFFER_SIZE, "%d", (int)val);
            addCell(convertBufferString());
        }

        void addCell(uint32_t val) {
            snprintf(convertBuffer, CONVERT_BUFFER_SIZE, "%u", (unsigned int)val);
            addCell(convertBufferString());
        }

        void addCell(int64_t val) {
            snprintf(convertBuffer, CONVERT_BUFFER_SIZE, "%" PRId64, val);
            addCell(convertBufferString());
        }

        void addCell(uint64_t val) {
            snprintf(convertBuffer, CONVERT_BUFFER_SIZE, "%" PRIu64, val);
            addCell(convertBufferString());
        }

        void addCell(bool val) {
            string value = (val ? "Y" : "N");
            addCell(value);
        }

        void show() override {

        }

        void show(const vector<CLIListColumn> & columns) {
            cout << "| ";

            for (size_t i = 0;i < numColumns;i++) {
                CLIListColumn column = columns[i];
                column.printCell(cells[i]);
            }

            cout << endl;
        }
};

class CLIListView : public CLIView {
    private:
        vector<CLIListColumn> columns;
        vector<CLIListRow> rows;
        vector<int> columnWidths;

        void printTopBorder() {
            cout << "+";

            for (auto & c : columns) {
                for (int i = 0;i < c.getColumnWidth() - 1;i++) {
                    cout << '-';
                }

                cout << '+';
            }

            cout << endl;
        }

        void printBottomBorder() {
            printTopBorder();
        }

    protected:
        void reserveRows(size_t numRows) {
            rows.reserve(numRows);
        } 
        
        void showHeader() {
            printTopBorder();

            cout << "| ";
            for (auto & c : columns) {
                c.printColumnHeader();
            }
            cout << endl;

            printTopBorder();
        }

    public:
        void setColumns(const vector<CLIListColumn> & cols) {
            columns.reserve(cols.size());
            columnWidths.reserve(cols.size());

            for (auto c : cols) {
                columns.push_back(c);
                columnWidths.push_back(c.getColumnWidth());
            }
        }

        virtual uint16_t getMinimumWidth() {
            return 0;
        }

        inline int getTotalWidth() {
            int totalWidth = 0;

            for (int w : columnWidths) {
                totalWidth += w;
            }

            return totalWidth + 2;
        }

        inline size_t getNumColumns() {
            return columns.size();
        }

        void addRow(CLIListRow & row) {
            rows.push_back(row);
        }

        void showNoExtraCR() {
            printTitle();
            showHeader();

            for (auto & r : rows) {
                r.show(columns);
            }

            if (!rows.empty()) {
                printBottomBorder();
            }
        }

        void show() override {
            showNoExtraCR();
            cout << endl;
        }

        void showTotal(const string & label, const Money & total) {
            int numPaddingSpaces = 0;

            for (int i = 0;i < columns.size();i++) {
                CLIListColumn c = columns[i];

                if (c.getName() == "Amount" || c.getName() == "Total") {
                    break;
                }

                numPaddingSpaces += columnWidths[i];
            }

            numPaddingSpaces -= (int)label.length();

            for (int i = 0;i < numPaddingSpaces;i++) {
                cout << ' ';
            }

            string totalStr = total.localeFormattedStringValue();
            int fieldWidth = calculateFieldWidth(totalStr, LIST_VIEW_AMOUNT_WIDTH);

            cout << label << "| " << bold_on << right << setw(fieldWidth) << totalStr << bold_off << " |" << endl << endl;
        }
};

#endif

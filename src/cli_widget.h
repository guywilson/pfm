#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <unordered_map>
#include <readline/readline.h>
#include <readline/history.h>

#include "pfm_error.h"

using namespace std;

#ifndef __CLI_WIDGET
#define __CLI_WIDGET

#define FIELD_STRING_LEN                        64
#define MAX_PROMPT_LENGTH                      128
#define AMOUNT_FIELD_STRING_LEN                 16
#define TITLE_BUFFER_LEN                       128

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

    protected:
        string _getLabel() {
            return label;
        }

        string _getValue() {
            return value;
        }

        void _setValue(string value) {
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

    protected:
        string readLine() {
            string text;

            char * line = readline(_getLabel().c_str());

            if (line != NULL && line[0] != 0) {
                text = line;
                free(line);
            }
            else if (defaultValue.length() > 0) {
                text = defaultValue;
            }

            return text;
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
            string line = readLine();
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

    public:
        CLIListColumn() : CLIField() {}
        CLIListColumn(const char * name, int width, CLIListColumn::alignment align) : CLIField(name) {
            this->width = width;
            this->align = align;

            if ((width + 2) < strlen(name)) {
                throw pfm_error("Column name is too long for specified width");
            }
        }

        CLIListColumn(string & name, int width, CLIListColumn::alignment align) : CLIField(name) {
            this->width = width;
            this->align = align;

            if ((width + 2) < name.length()) {
                throw pfm_error("Column name is too long for specified width");
            }
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
        int tableWidth;

        int getNumPaddingChars(CLIListColumn & column, string & value) {
            return (column.getWidth() - value.length());
        }

        int printPadding(CLIListColumn & column, string & value) {
            int numPaddingChars = getNumPaddingChars(column, value);

            for (int i = 0;i < numPaddingChars;i++) {
                cout << " ";
            }

            return numPaddingChars;
        }
        
        int getColumnWidth(CLIListColumn & column) {
            string columnName = column.getName();

            return (columnName.length() + getNumPaddingChars(column, columnName) + 4);
        }

        void printColumnHeader(CLIListColumn & column) {
            string name = column.getName();

            cout << name;
            printPadding(column, name);
            cout << " | ";
        }

        void printCell(CLIListColumn & column, string value) {
            switch (column.getAlignment()) {
                case CLIListColumn::leftAligned:
                    cout << left << setw(column.getWidth()) << value;
                    break;

                case CLIListColumn::rightAligned:
                    cout << right << setw(column.getWidth()) << value;
                    break;
            }

            cout << " | ";
        }

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
            for (int i = 0;i < tableWidth;i++) {
                cout << "-";
            }

            cout << endl;
        }

        void printBottomBorder() {
            for (int i = 0;i < tableWidth;i++) {
                cout << "-";
            }

            cout << endl;
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
            tableWidth = 0;

            cout << "| ";

            for (int i = 0;i < getNumColumns();i++) {
                CLIListColumn column = getColumnAt(i);

                printColumnHeader(column);

                tableWidth += getColumnWidth(column);
            }

            cout << endl;

            tableWidth -= 2;

            printTopBorder();
        }

        void show() override {
            cout << "| ";

            for (int i = 0;i < getNumColumns();i++) {
                CLIListColumn column = getColumnAt(i);
                string value = getValueAt(i);

                printCell(column, value);
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

        void show() override {
            printTitle();

            headerRow.showHeaderRow();

            for (int i = 0;i < dataRows.size();i++) {
                dataRows[i].show();
            }

            cout << endl;
        }
};

#endif

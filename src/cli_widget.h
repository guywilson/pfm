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
        void readLine() {
            char * line = NULL;

            line = readline(_getLabel().c_str());

            if (line != NULL && line[0] != 0) {
                _setValue(line);
                free(line);
            }
            else {
                _setValue("");
            }
        }

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

        int getNumPaddingChars(CLIListColumn & column, string & value) {
            return column.getWidth() - value.length();
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

        void showHeaderRow() {
            int tableWidth = 0;

            cout << "| ";

            for (int i = 0;i < columnDefintions.size();i++) {
                tableWidth += 2;

                CLIListColumn column = columnDefintions[i];

                string columnName = column.getName();
                cout << columnName;

                tableWidth += columnName.length();

                int numPaddingChars = getNumPaddingChars(column, columnName);;

                for (int j = 0;j < numPaddingChars;j++) {
                    cout << " ";
                    tableWidth++;
                }

                cout << " | ";

                tableWidth += 2;
            }

            cout << endl;

            tableWidth -= 2;

            for (int i = 0;i < tableWidth;i++) {
                cout << "-";
            }

            cout << endl;
        }

        void show() override {
            cout << "| ";

            for (int i = 0;i < getNumColumns();i++) {
                CLIListColumn column = getColumnAt(i);
                string value = getValueAt(i);

                switch (column.getAlignment()) {
                    case CLIListColumn::leftAligned:
                        cout << left << setw(column.getWidth()) << value;
                        break;

                    case CLIListColumn::rightAligned:
                        cout << right << setw(column.getWidth()) << value;
                        break;
                }

                int numPaddingChars = getNumPaddingChars(column, columnValues[i]);

                for (int j = 0;j < numPaddingChars;j++) {
                    cout << " ";
                }

                cout << " | ";
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

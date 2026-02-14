#pragma once

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <string.h>
#include <vector>
#include <unordered_map>
#include <inttypes.h>

#include "field.h"
#include "view.h"
#include "text.h"
#include "constants.h"

#include "../pfm_error.h"
#include "../money.h"
#include "../strdate.h"
#include "../custom_modifiers.h"
#include "../terminal.h"

class CLITableCell : public CLIField {
    private:
        size_t width;

        size_t getNumPaddingChars(const std::string & value) {
            return (getWidth() - value.length());
        }

        size_t printPadding(const std::string & value) {
            size_t numPaddingChars = getNumPaddingChars(value);

            for (size_t i = 0;i < numPaddingChars;i++) {
                std::cout << " ";
            }

            return numPaddingChars;
        }

    public:
        CLITableCell() : CLIField() {}
        CLITableCell(const std::string & label, const std::string & value, size_t valueWidth) : CLIField(label) {
            _setValue(value);
            this->width = valueWidth;
        }

        string getCellContents() {
            string value = getValue();
            size_t w = getWidth();
            w = cli::text::calculateFieldWidth(value, w);

            if (value.length() > w) {
                value = value.substr(0, w - 3);
                value = value.append("...");
            }

            std::stringstream ss;

            ss << 
                set_style(TextStyle::Bold, Colour::Yellow) << 
                getLabel() << 
                set_style(TextStyle::Reset) << 
                ": " <<
                std::left << 
                std::setw(w) << 
                value;

            return ss.str();
        }

        inline string getLabel() {
            return _getLabel();
        }

        inline size_t getWidth() {
            return width;
        }

        inline size_t getTotalWidth() {
            return getWidth() + _getLabel().length() + 2;
        }

        inline void setWidth(size_t width) {
            this->width = width;
        }

        void show() override {}
};

class CLITable : public CLIView {
    private:
        std::vector<CLITableCell> cells;
        size_t numColumns;
        size_t numRows;
        string title;

    public:
        CLITable(const std::string & title, size_t numRows) {
            this->numColumns = 2;
            this->numRows = numRows;
            this->title = title;

            cells.resize(numColumns * numRows);
        }

        void addCell(const CLITableCell & cell, size_t column, size_t row) {
            if (column > (numColumns - 1) || row > (numRows - 1)) {
                throw pfm_error(
                    pfm_error::buildMsg(
                        "CLITable::addCell() - specified cell location out of range. " \
                        "Table dimensions are %zu x %zu, requested (%zu, %zu)", 
                        numColumns, 
                        numRows, 
                        column, 
                        row),
                    __FILE__,
                    __LINE__);
            }

            size_t index = row * numColumns + column;
            cells[index] = cell;
        }

        void printRow(const vector<CLITableCell> & columns) {
            for (size_t i = 0;i < columns.size();i++) {
                CLITableCell cell = columns[i];

                string contents = cell.getCellContents();

                cout << contents;

                bool isLastColumn = (i == (numColumns - 1));

                if (!isLastColumn) {
                    for (size_t j = (contents.length() - 1);j < ((TERMINAL_MIN_WIDTH / numColumns) + 10);j++) {
                        cout << " ";
                    }
                }
            }

            cout << endl;
        }

        void show() override {
            size_t i = 0;

            cout << "*** " << this->title << " ***" << endl << endl;
            
            while (i < cells.size()) {
                vector<CLITableCell> columns;
                columns.reserve(numColumns);

                for (size_t c = 0;c < numColumns;c++) {
                    columns.push_back(cells[i++]);
                }

                printRow(columns);
            }
        }
};

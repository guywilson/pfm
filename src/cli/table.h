#pragma once

#include <iostream>
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

class CLITableCell : public CLIField {
    public:
        enum cell_alignment {
            leftAligned,
            rightAligned
        };

    private:
        int width;
        cell_alignment align;
        string value;

        int getNumPaddingChars(const std::string & value) {
            return (getWidth() - value.length());
        }

        int printPadding(const std::string & value) {
            int numPaddingChars = getNumPaddingChars(value);

            for (int i = 0;i < numPaddingChars;i++) {
                std::cout << " ";
            }

            return numPaddingChars;
        }

    public:
        CLITableCell() : CLIField() {}
        CLITableCell(const std::string & label, const std::string & value, CLITableCell::cell_alignment align) : CLIField(label) {
            this->align = align;
            this->value = value;
        }

        int getColumnWidth() {
            string columnName = getName();

            return (columnName.length() + getNumPaddingChars(columnName) + 3);
        }

        void printColumnHeader() {
            std::string name = getName();

            std::cout << bold_on << name << bold_off;
            printPadding(name);
            std::cout << " | ";
        }

        void printCell(string & value) {
            int width = getWidth();
            std::string v = value;

            width = cli::text::calculateFieldWidth(value, width);

            if (width < (int)v.length()) {
                v = v.substr(0, width - 3);
                v = v.append("...");
            }

            switch (getAlignment()) {
                case CLIListColumn::leftAligned:
                    std::cout << std::left << std::setw(width) << v;
                    break;

                case CLIListColumn::rightAligned:
                    std::cout << std::right << std::setw(width) << v;
                    break;
            }

            std::cout << " | ";
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

class CLITable : public CLIView {
    private:
        std::vector<CLITableCell> cells;
        int numColumns

        void printTopBorder() {
            std::cout << "+";

            for (auto & c : columns) {
                for (int i = 0;i < c.getColumnWidth() - 1;i++) {
                    cout << '-';
                }

                std::cout << '+';
            }

            std::cout << std::endl;
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

            std::cout << "| ";
            for (auto & c : columns) {
                c.printColumnHeader();
            }
            std::cout << std::endl;

            printTopBorder();
        }

    public:
        void setColumns(const std::vector<CLIListColumn> & cols) {
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
            std::cout << std::endl;
        }

        void showTotal(const std::string & label, const Money & total) {
            int numPaddingSpaces = 0;

            for (size_t i = 0;i < columns.size();i++) {
                CLIListColumn c = columns[i];

                if (c.getName() == "Amount" || c.getName() == "Total") {
                    break;
                }

                numPaddingSpaces += columnWidths[i];
            }

            numPaddingSpaces -= (int)label.length();

            for (int i = 0;i < numPaddingSpaces;i++) {
                std::cout << ' ';
            }

            std::string totalStr = total.localeFormattedStringValue();
            int fieldWidth = cli::text::calculateFieldWidth(totalStr, LIST_VIEW_AMOUNT_WIDTH);

            std::cout << label << "| " << bold_on << std::right << std::setw(fieldWidth) << totalStr << bold_off << " |" << std::endl << std::endl;
        }
};

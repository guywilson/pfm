#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
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

class CLIDetailListCell : public CLIField {
    public:
        enum column_alignment {
            leftAligned,
            rightAligned
        };

    private:
        std::string text;
        size_t width;

        static const int CONVERT_BUFFER_SIZE = 32;

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
        CLIDetailListCell() : CLIField() {}
        CLIDetailListCell(const std::string & value, size_t width, uint8_t styleFlags, const Colour & colour, CLIDetailListCell::column_alignment align) : CLIField() {
            this->width = width;
            std::string v = value;

            width = cli::text::calculateFieldWidth(value, width);

            if (width < v.length()) {
                v = v.substr(0, width - 3);
                v = v.append("...");
            }

            std::stringstream ss;

            switch (align) {
                case CLIDetailListCell::leftAligned:
                    ss << set_style(styleFlags, colour) << std::left << std::setw(width) << v << set_style(TextStyle::Reset);
                    break;

                case CLIDetailListCell::rightAligned:
                    ss << set_style(styleFlags, colour) << std::right << std::setw(width) << v << set_style(TextStyle::Reset);
                    break;
            }

            ss << " ";

            this->text = ss.str();
        }
        CLIDetailListCell(const std::string & value, size_t width, uint8_t styleFlags, const Colour & colour) : CLIDetailListCell(value, width, styleFlags, colour, CLIDetailListCell::leftAligned) {}

        static std::string formatValue(Money & value) {
            return value.localeFormattedStringValue();
        }

        static std::string formatValue(StrDate & value) {
            return value.shortDate();
        }

        static std::string formatValue(double val) {
            char convertBuffer[CONVERT_BUFFER_SIZE];
            snprintf(convertBuffer, CONVERT_BUFFER_SIZE, "%.2f", val);
            return std::string(convertBuffer);
        }

        static std::string formatValue(int32_t val) {
            char convertBuffer[CONVERT_BUFFER_SIZE];
            snprintf(convertBuffer, CONVERT_BUFFER_SIZE, "%d", (int)val);
            return std::string(convertBuffer);
        }


        static std::string formatValue(uint32_t val) {
            char convertBuffer[CONVERT_BUFFER_SIZE];
            snprintf(convertBuffer, CONVERT_BUFFER_SIZE, "%u", (int)val);
            return std::string(convertBuffer);
        }

        static std::string formatValue(int64_t val) {
            char convertBuffer[CONVERT_BUFFER_SIZE];
            snprintf(convertBuffer, CONVERT_BUFFER_SIZE, "%" PRId64, val);
            return std::string(convertBuffer);
        }

        static std::string formatValue(uint64_t val) {
            char convertBuffer[CONVERT_BUFFER_SIZE];
            snprintf(convertBuffer, CONVERT_BUFFER_SIZE, "%" PRIu64, val);
            return std::string(convertBuffer);
        }

        static std::string formatValue(bool val) {
            return (val ? "Yes" : "No");
        }

        inline size_t getWidth() const {
            return this->width;
        }

        void show() override {
            std::cout << text;
        }
};

class CLIDetailListRow : public CLIWidget {
    private:
        std::vector<CLIDetailListCell> primaryCells;
        std::vector<CLIDetailListCell> secondaryCells;
        std::vector<int> primaryCellWidths;
        std::vector<int> secondaryCellWidths;

    protected:
        inline int getTotalWidth() {
            int primaryWidth = 1;

            for (int w : primaryCellWidths) {
                primaryWidth += (w + 1);
            }

            int secondaryWidth = 1;

            for (int w : secondaryCellWidths) {
                secondaryWidth += (w + 1);
            }

            return (primaryWidth > secondaryWidth ? primaryWidth : secondaryWidth);
        }

    public:
        void addPrimaryCell(const CLIDetailListCell & cell) {
            primaryCells.push_back(cell);
            primaryCellWidths.push_back(cell.getWidth());
        }

        void addSecondaryCell(const CLIDetailListCell & cell) {
            secondaryCells.push_back(cell);
            secondaryCellWidths.push_back(cell.getWidth());
        }

        void printTopBorder() {
            std::cout << "+";

            for (int i = 0;i < getTotalWidth();i++) {
                std::cout << "-";
            }

            std::cout << "+" << std::endl;
        }

        void printBottomBorder() {
            printTopBorder();
        }

        void show() override {
            printTopBorder();

            std::cout << "| ";
            for (auto & c : primaryCells) {
                c.show();
            }
            std::cout << "|" << std::endl;

            std::cout << "| ";
            for (auto & c : secondaryCells) {
                c.show();
            }
            std::cout << "|" << std::endl;
        }
};

class CLIDetailListView : public CLIView {
    private:
        std::vector<CLIDetailListRow> rows;

    public:
        void addRow(const CLIDetailListRow & row) {
            rows.push_back(row);
        }

        void show() override {
            printTitle();

            if (!rows.empty()) {
                for (auto & row : rows) {
                    row.show();
                }

                rows[0].printBottomBorder();
            }
        }

        void showTotal(const std::string & label, const Money & total, size_t padding) {
            std::string totalStr = total.localeFormattedStringValue();
            int fieldWidth = cli::text::calculateFieldWidth(totalStr, LIST_VIEW_AMOUNT_WIDTH);

            for (size_t i = 0;i < padding;i++) {
                std::cout << " ";
            }

            std::cout << set_style(TextStyle::Bold | TextStyle::Underline, Colour::Cyan) << label << set_style(TextStyle::Reset) << "| " << set_style(TextStyle::Bold) << std::right << std::setw(fieldWidth) << totalStr << set_style(TextStyle::Reset) << " |" << std::endl << std::endl;
        }
};

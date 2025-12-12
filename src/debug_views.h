#include <iostream>
#include <string>
#include <string.h>
#include <vector>

#include "pfm_error.h"
#include "cli_widget.h"
#include "terminal.h"
#include "custom_widgets.h"
#include "db_v_carried_over.h"

using namespace std;

#ifndef __DEBUG_VIEWS
#define __DEBUG_VIEWS

class CarriedOverListView : public CLIListView {
    public:
        CarriedOverListView() : CLIListView() {}

        void addResults(DBResult<DBCarriedOverView> & result) {
            char szTitle[TITLE_BUFFER_LEN];

            snprintf(szTitle, TITLE_BUFFER_LEN, "Carried Over Logs (%d)", result.size());
            setTitle(szTitle);

            CLIListRow headerRow;

            CLIListColumn column1 = CLIListColumn("Account", 7, CLIListColumn::leftAligned);
            headerRow.addColumn(column1);

            CLIListColumn column2 = CLIListColumn("Date", DATE_FIELD_LENGTH, CLIListColumn::leftAligned);
            headerRow.addColumn(column2);

            CLIListColumn column3 = CLIListColumn("Description", 25, CLIListColumn::leftAligned);
            headerRow.addColumn(column3);

            CLIListColumn column4 = CLIListColumn("Balance", 16, CLIListColumn::rightAligned);
            headerRow.addColumn(column4);

            addHeaderRow(headerRow);

            for (int i = 0;i < result.size();i++) {
                DBCarriedOverView co = result.at(i);

                CLIListRow row(headerRow);

                row.addCellValue(co.accountCode);
                row.addCellValue(co.date.shortDate());
                row.addCellValue(co.description);
                row.addCellValue(co.balance);

                addRow(row);
            }
        }
};

class GenericListView : public CLIListView {
    public:
        GenericListView() : CLIListView() {}

        void addRows(vector<DBRow> & rows) {
            Terminal & terminal = Terminal::getInstance();

            int remainingWidth = terminal.getWidth();

            char szTitle[TITLE_BUFFER_LEN];
            snprintf(szTitle, TITLE_BUFFER_LEN, "Results (%zu rows)", rows.size());
            setTitle(szTitle);

            CLIListRow headerRow;

            DBRow r = rows[0];

            int numColumns = 0;

            for (size_t columnIndex = 0;columnIndex < r.getNumColumns();columnIndex++) {
                DBColumn c = r.getColumnAt(columnIndex);

                string name = c.getName();
                int width = 0;

                if (name == "id") {
                    width = 5;
                }
                else if (name == "amount" || name == "balance" || name == "balance_limit") {
                    width = AMOUNT_FIELD_STRING_LEN;
                }
                else if (name == "name" || name == "description") {
                    width = 22;
                }
                else if (name == "code") {
                    width = CODE_FIELD_MAX_LENGTH;
                }
                else if (name == "date") {
                    width = DATE_FIELD_LENGTH;
                }
                else if (name == "created" || name == "updated") {
                    width = TIMESTAMP_FIELD_LENGTH;
                }
                else {
                    width = (int)name.length();
                }

                CLIListColumn column = CLIListColumn(name.c_str(), width, CLIListColumn::leftAligned);
                remainingWidth -= column.getColumnWidth();

                if (remainingWidth < 0) {
                    break;
                }
                else {
                    headerRow.addColumn(column);
                    numColumns++;
                }
            }

            addHeaderRow(headerRow);

            for (size_t i = 0;i < rows.size();i++) {
                DBRow r = rows[i];

                CLIListRow row(headerRow);

                for (int columnIndex = 0;columnIndex < numColumns;columnIndex++) {
                    DBColumn c = r.getColumnAt(columnIndex);

                    if (c.getName() == "date") {
                        StrDate date = c.getValue();
                        row.addCellValue(date.shortDate());
                    }
                    else if (c.getName() == "amount" || c.getName() == "balance" || c.getName() == "balance_limit") {
                        Money amount = c.doubleValue();
                        row.addCellValue(amount.localeFormattedStringValue());
                    }
                    else {
                        row.addCellValue(c.getValue());
                    }
                }

                addRow(row);
            }
        }
};

#endif

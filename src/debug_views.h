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
        CarriedOverListView() : CLIListView() {
            if (Terminal::getWidth() < getMinimumWidth()) {
                throw pfm_error(
                    pfm_error::buildMsg(
                        "Terminal is not wide enough for CarriedOverListView. Terminal width %u, minimum width %u", 
                        (unsigned int)Terminal::getWidth(), 
                        (unsigned int)getMinimumWidth()));
            }
        }

        inline uint16_t getMinimumWidth() override {
            return (
                7 + 
                DATE_FIELD_LENGTH + 
                25 + 
                16);
        }

        void addResults(DBResult<DBCarriedOverView> & result) {
            char szTitle[TITLE_BUFFER_LEN];

            snprintf(szTitle, TITLE_BUFFER_LEN, "Carried Over Logs (%zu)", result.size());
            setTitle(szTitle);

            setColumns({
                CLIListColumn("Account", 7, CLIListColumn::leftAligned),
                CLIListColumn("Date", DATE_FIELD_LENGTH, CLIListColumn::leftAligned),
                CLIListColumn("Description", 25, CLIListColumn::leftAligned),
                CLIListColumn("Balance", 16, CLIListColumn::rightAligned)
            });

            for (size_t i = 0;i < result.size();i++) {
                DBCarriedOverView co = result.at(i);

                CLIListRow row(getNumColumns());

                row.addCell(co.accountCode);
                row.addCell(co.date);
                row.addCell(co.description);
                row.addCell(co.balance);

                addRow(row);
            }
        }
};

class GenericListView : public CLIListView {
    public:
        GenericListView() : CLIListView() {
            if (Terminal::getWidth() < getMinimumWidth()) {
                throw pfm_error(
                    pfm_error::buildMsg(
                        "Terminal is not wide enough for GenericListView. Terminal width %u, minimum width %u", 
                        (unsigned int)Terminal::getWidth(), 
                        (unsigned int)getMinimumWidth()));
            }
        }

        inline uint16_t getMinimumWidth() override {
            return 0;
        }

        void addRows(vector<DBRow> & rows) {
            char szTitle[TITLE_BUFFER_LEN];
            snprintf(szTitle, TITLE_BUFFER_LEN, "Results (%zu rows)", rows.size());
            setTitle(szTitle);

            if (rows.size() > 0) {
                int remainingWidth = Terminal::getWidth();

                vector<CLIListColumn> columns;

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

                    int columnWidth = width + 3;

                    if (remainingWidth < columnWidth) {
                        break;
                    }
                    else {
                        remainingWidth -= columnWidth;
                        columns.push_back(CLIListColumn(name.c_str(), width, CLIListColumn::leftAligned));
                        numColumns++;
                    }
                }

                setColumns(columns);

                for (size_t i = 0;i < rows.size();i++) {
                    DBRow r = rows[i];

                    CLIListRow row(getNumColumns());

                    for (int columnIndex = 0;columnIndex < numColumns;columnIndex++) {
                        DBColumn c = r.getColumnAt(columnIndex);

                        if (c.getName() == "date") {
                            StrDate date = c.getValue();
                            row.addCell(date);
                        }
                        else if (c.getName() == "amount" || c.getName() == "balance" || c.getName() == "balance_limit") {
                            Money amount = c.doubleValue();
                            row.addCell(amount);
                        }
                        else {
                            row.addCell(c.getValue());
                        }
                    }

                    addRow(row);
                }
            }
        }
};

#endif

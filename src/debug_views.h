#include <iostream>
#include <string>
#include <string.h>
#include <vector>

#include "pfm_error.h"
#include "cli_widget.h"
#include "custom_widgets.h"
#include "db_v_carried_over.h"

using namespace std;

#ifndef __CARRIED_OVER_VIEW
#define __CARRIED_OVER_VIEW

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

#endif

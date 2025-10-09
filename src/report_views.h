#include <iostream>
#include <string>
#include <string.h>
#include <vector>

#include <readline/readline.h>
#include <readline/history.h>

#include "pfm_error.h"
#include "cli_widget.h"
#include "custom_widgets.h"
#include "db_transaction.h"
#include "db_v_transaction.h"
#include "db_transaction_report.h"
#include "cfgmgr.h"

using namespace std;

#ifndef __REPORT_VIEW
#define __REPORT_VIEW

#define CRITERIA_BUFFER_LENGTH                      64
#define REPORT_PROMPT_LENGTH                       256

class AddReportView : public CLIView {
    private:
        CLITextField descriptionField = CLITextField("Description: ");
        CLITextField sqlField = CLITextField("SQL WHERE: ");

    public:
        AddReportView() : AddReportView("Add report") {}

        AddReportView(const char * title) : CLIView(title) {
        }

        void show() override {
            CLIView::show();

            descriptionField.show();
            sqlField.show();
        }

        DBTransactionReport getReport() {
            DBTransactionReport report;

            report.description = descriptionField.getValue();
            report.sqlWhereClause = sqlField.getValue();

            return report;
        }
};

class ReportListView : public CLIListView {
    private:
        void showResultsTable(DBResult<DBTransactionReport> & result) {
            CLIListRow headerRow;

            CLIListColumn column1 = CLIListColumn("Seq", 3, CLIListColumn::rightAligned);
            headerRow.addColumn(column1);

            CLIListColumn column2 = CLIListColumn("Description.", 24, CLIListColumn::leftAligned);
            headerRow.addColumn(column2);

            CLIListColumn column3 = CLIListColumn("Where Clause", 73, CLIListColumn::leftAligned);
            headerRow.addColumn(column3);

            addHeaderRow(headerRow);

            for (int i = 0;i < result.size();i++) {
                DBTransactionReport report = result[i];

                CLIListRow row(headerRow);

                row.addCellValue(report.sequence);
                row.addCellValue(report.description);
                row.addCellValue(report.sqlWhereClause);

                addRow(row);
            }
        }

    public:
        ReportListView() : CLIListView() {
        }

        void addResults(DBResult<DBTransactionReport> & result) {
            char szTitle[TITLE_BUFFER_LEN];

            snprintf(szTitle, TITLE_BUFFER_LEN, "Report list (%d)", result.size());
            setTitle(szTitle);

            showResultsTable(result);
        }

        void show() override {
            CLIListView::show();
        }
};

class UpdateReportView : public CLIView {
    private:
        pfm_id_t reportId;
        string sql;

        CLITextField descriptionField;
        CLITextField sqlField;

    public:
        UpdateReportView() : UpdateReportView("Update report") {}

        UpdateReportView(const char * title) : CLIView(title) {
            sqlField.setLengthLimit(REPORT_PROMPT_LENGTH);
        }

        void setReport(DBTransactionReport & report) {
            char szPrompt[REPORT_PROMPT_LENGTH];

            report.retrieve();

            reportId = report.id;

            snprintf(szPrompt, REPORT_PROMPT_LENGTH, "Description ['%s']: ", report.description.c_str());
            descriptionField.setLabel(szPrompt);
            descriptionField.setDefaultValue(report.description);

            snprintf(szPrompt, REPORT_PROMPT_LENGTH, "SQL ['%s']: ", report.sqlWhereClause.c_str());
            sqlField.setLabel(szPrompt);
            sqlField.setDefaultValue(report.sqlWhereClause);

            sql = report.sqlWhereClause;
        }

        void show() override {
            CLIView::show();

            descriptionField.show();

            clear_history();
            add_history(sql.c_str());
            
            sqlField.show();
        }

        DBTransactionReport getReport() {
            DBTransactionReport report;

            report.id = reportId;

            report.description = descriptionField.getValue();
            report.sqlWhereClause = sqlField.getValue();

            return report;
        }
};

class SaveReportView : public CLIView {
    private:
        CLITextField descriptionField = CLITextField("Description: ");

    public:
        SaveReportView() : SaveReportView("Save report") {}

        SaveReportView(const char * title) : CLIView(title) {
        }

        void show() override {
            CLIView::show();

            descriptionField.show();
        }

        DBTransactionReport getReport() {
            DBTransactionReport report;

            report.description = descriptionField.getValue();

            return report;
        }
};

class ShowReportView : public CLIView {
    private:
        DBTransactionReport rp;

    public:
        ShowReportView() : ShowReportView("Show report") {}

        ShowReportView(const char * title) : CLIView(title) {}

        void setReport(DBTransactionReport & report) {
            char szPrompt[REPORT_PROMPT_LENGTH];

            rp = report;
            rp.retrieve();
        }

        void show() override {
            CLIView::show();

            printf("Description: '%s'\n", rp.description.c_str());
            printf("SQL: '%s'\n", rp.sqlWhereClause.c_str());
        }
};

#endif

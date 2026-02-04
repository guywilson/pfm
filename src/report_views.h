#include <iostream>
#include <string>
#include <string.h>
#include <vector>

#include <readline/readline.h>
#include <readline/history.h>

#include "pfm_error.h"
#include "cli_widget.h"
#include "terminal.h"
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
            reserveRows(result.size());
            
            setColumns({
                CLIListColumn("Sq", LIST_VIEW_SEQUENCE_WIDTH, CLIListColumn::rightAligned),
                CLIListColumn("Description.", 30, CLIListColumn::leftAligned),
            });

            for (size_t i = 0;i < result.size();i++) {
                DBTransactionReport report = result[i];

                CLIListRow row(getNumColumns());

                row.addCell(report.sequence);
                row.addCell(report.description);

                addRow(row);
            }
        }

    public:
        ReportListView() : CLIListView() {
            if (Terminal::getWidth() < getMinimumWidth()) {
                throw pfm_error(
                    pfm_error::buildMsg(
                        "Terminal is not wide enough for ReportListView. Terminal width %u, minimum width %u", 
                        (unsigned int)Terminal::getWidth(), 
                        (unsigned int)getMinimumWidth()));
            }
        }

        inline uint16_t getMinimumWidth() override {
            return (
                3 + 
                24 + 
                73);
        }

        void addResults(DBResult<DBTransactionReport> & result) {
            char szTitle[TITLE_BUFFER_LEN];

            snprintf(szTitle, TITLE_BUFFER_LEN, "Report list (%zu)", result.size());
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

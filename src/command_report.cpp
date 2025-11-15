#include <string>
#include <string.h>
#include <vector>

#include "command.h"
#include "pfm_error.h"
#include "db.h"
#include "strdate.h"
#include "cfgmgr.h"
#include "cache.h"
#include "jfile.h"

#include "db_transaction_report.h"
#include "report_views.h"

using namespace std;

void Command::addReport() {
    AddReportView view;
    view.show();

    DBTransactionReport report = view.getReport();
    report.save();
}

void Command::copyReport(DBTransactionReport & report) {
    report.id.clear();
    report.save();
}

void Command::listReports() {
    DBResult<DBTransactionReport> result;
    result.retrieveAll();

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    cacheMgr.clearReports();

    for (int i = 0;i < result.size();i++) {
        DBTransactionReport report = result[i];
        cacheMgr.addReport(report.sequence, report);
    }

    ReportListView view;
    view.addResults(result);
    view.show();
}

DBTransactionReport Command::getReport(int sequence) {
    CacheMgr & cacheMgr = CacheMgr::getInstance();

    DBTransactionReport report = cacheMgr.getReport(sequence);

    return report;
}

void Command::updateReport(DBTransactionReport & report) {
    UpdateReportView view;
    view.setReport(report);
    view.show();

    DBTransactionReport updatedReport = view.getReport();
    updatedReport.save();
}

void Command::deleteReport(DBTransactionReport & report) {
    report.remove();
    report.clear();
}

void Command::runReport(DBTransactionReport & report) {
    findTransactions(report.sqlWhereClause);
}

void Command::saveReport(const string & description) {
    CacheMgr & cacheMgr = CacheMgr::getInstance();

    DBTransactionReport report;

    if (description.length() == 0) {
        SaveReportView view;
        view.show();

        report = view.getReport();
        report.sqlWhereClause = cacheMgr.getFindCriteria();
    }
    else {
        report.description = description;
        report.sqlWhereClause = cacheMgr.getFindCriteria();
    }

    report.save();

    cacheMgr.clearFindCriteria();
}

void Command::showReport(DBTransactionReport & report) {
    ShowReportView view;
    view.setReport(report);
    view.show();
}

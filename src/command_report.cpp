#include <string>
#include <string.h>
#include <vector>
#include <stdlib.h>

#include "command.h"
#include "pfm_error.h"
#include "db.h"
#include "strdate.h"
#include "cfgmgr.h"
#include "cache.h"
#include "jfile.h"

#include "db_transaction_report.h"
#include "report_views.h"


void Command::addReport() {
    AddReportView view;
    view.show();

    DBTransactionReport report = view.getReport();
    report.save();
}

void Command::copyReport() {
    std::string sequence = getParameter(SEQUENCE_PARAM_NAME);

    DBTransactionReport report = getReport(atoi(sequence.c_str()));

    report.id.clear();
    report.save();
}

void Command::listReports() {
    DBResult<DBTransactionReport> result;
    result.retrieveAll();

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    cacheMgr.clearReports();

    for (size_t i = 0;i < result.size();i++) {
        DBTransactionReport report = result[i];
        cacheMgr.addReport(report.sequence, report);
    }

    ReportDetailsListView view;
    view.addResults(result);
    view.show();
}

DBTransactionReport Command::getReport(int sequence) {
    CacheMgr & cacheMgr = CacheMgr::getInstance();

    DBTransactionReport report = cacheMgr.getReport(sequence);

    return report;
}

void Command::updateReport() {
    std::string sequence = getParameter(SEQUENCE_PARAM_NAME);

    DBTransactionReport report = getReport(atoi(sequence.c_str()));

    UpdateReportView view;
    view.setReport(report);
    view.show();

    DBTransactionReport updatedReport = view.getReport();
    updatedReport.save();
}

void Command::deleteReport() {
    std::string sequence = getParameter(SEQUENCE_PARAM_NAME);

    DBTransactionReport report = getReport(atoi(sequence.c_str()));

    report.remove();
    report.clear();
}

void Command::runReport() {
    std::string sequence = getParameter(SEQUENCE_PARAM_NAME);

    DBTransactionReport report = getReport(atoi(sequence.c_str()));

    findTransactions(report.sqlWhereClause);
}

void Command::saveReport() {
    CacheMgr & cacheMgr = CacheMgr::getInstance();

    DBTransactionReport report;
    std::string description;

    if (hasParameters()) {
        description = getParameter(SIMPLE_PARAM_NAME);
    }

    if (description.length() == 0) {
        SaveReportView view;
        view.show();

        report = view.getReport();
    }
    else {
        report.description = description;
    }

    report.sqlWhereClause = cacheMgr.getFindCriteria();    
    report.save();

    cacheMgr.clearFindCriteria();
}

void Command::showReport() {
    std::string sequence = getParameter(SEQUENCE_PARAM_NAME);

    DBTransactionReport report = getReport(atoi(sequence.c_str()));

    ShowReportView view;
    view.setReport(report);
    view.show();
}

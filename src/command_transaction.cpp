#include <string>
#include <string.h>
#include <vector>
#include <stdlib.h>
#include <ctype.h>
#include <algorithm>

#include "command.h"
#include "pfm_error.h"
#include "db.h"
#include "strdate.h"
#include "cfgmgr.h"
#include "cache.h"
#include "jfile.h"

#include "db_transaction.h"
#include "db_v_transaction.h"
#include "db_carried_over.h"
#include "transaction_views.h"
#include "debug_views.h"
#include "cli_widget.h"


void Command::listCarriedOverLogs() {
    DBResult<DBCarriedOverView> result;
    result.retrieveAll();

    CarriedOverListView view;
    view.addResults(result);
    view.show();
}

void Command::clearRecurringTransactions() {
    checkAccountSelected();

    DBTransaction tr;
    tr.deleteAllRecurringTransactionsForAccount(selectedAccount.id);
}

void Command::addTransaction() {
    checkAccountSelected();

    if (hasParameters()) {
        DBTransaction transaction;

        try {
            DBCategory category;
            std::string code = getParameter("c");
            category.retrieveByCode(code);
            transaction.categoryId = category.id;
        }
        catch (pfm_error & e) {
            transaction.categoryId.clear();
        }

        try {
            DBPayee payee;
            std::string code = getParameter("p");
            payee.retrieveByCode(code);
            transaction.payeeId = payee.id;
        }
        catch (pfm_error & e) {
            transaction.payeeId.clear();
        }

        std::string accountCode = getParameter("acc");

        if (accountCode.length() == 0) {
            transaction.accountId = selectedAccount.id;
        }
        else {
            DBAccount account;
            account.retrieveByCode(accountCode);

            transaction.accountId = account.id;
        }

        std::string date = getParameter("date");
        transaction.date = date.empty() ? StrDate::today() : date;

        std::string type = getParameter("type");
        transform(type.begin(), type.end(), type.begin(), ::toupper);
        transaction.type = type.empty() ? TYPE_DEBIT : type;

        std::string reconciled = getParameter("rec");
        transaction.isReconciled = reconciled == "Y" ? true : false;
        
        transaction.description = getParameter("desc");
        transaction.reference = getParameter("ref");
        transaction.amount = getParameter("amnt");

        transaction.save();
        return;
    }

    AddTransactionView view;
    view.show();

    DBTransaction transaction = view.getTransaction();
    transaction.accountId = selectedAccount.id;
    transaction.save();
}

void Command::copyTransaction() {
    DBTransaction transaction;

    if (hasParameters()) {
        std::string sequence = getParameter(SEQUENCE_PARAM_NAME);

        transaction = getTransaction(atoi(sequence.c_str()));
        transaction.recurringChargeId.clear();
    }
    else {
        transaction = transaction.retrieveLastNonRecurringTransaction();
    }

    transaction.id.clear();

    transaction.save();
}

void Command::listTransactions() {
    checkAccountSelected();

    DBTransactionView::recurring_type recurringType = DBTransactionView::non_recurring;
    bool thisPeriod = false;
    bool showTotal = false;
    uint32_t rowLimit = 25;
    DBCriteria::sql_order sortDirection = DBCriteria::descending;

    if (hasParameters()) {
        std::string rows = getParameter("rows");
        if (!rows.empty()) {
            rowLimit = strtoul(rows.c_str(), NULL, 10);
        }
        
        std::string timeframe = getParameter("timeframe");
        if (!timeframe.empty()) {
            if (timeframe.compare("period") == 0) {
                thisPeriod = true;
            }
            else if (timeframe.compare("any") == 0) {
                thisPeriod = false;
            }
        }

        std::string recurring = getParameter("recurring");
        if (!recurring.empty()) {
            if (recurring.compare("all") == 0) {
                recurringType = DBTransactionView::all;
            }
            else if (recurring.compare("nr") == 0) {
                recurringType = DBTransactionView::non_recurring;
            }
            else if (recurring.compare("rc") == 0) {
                recurringType = DBTransactionView::recurring_only;
            }
        }

        std::string sort = getParameter("sort");
        if (!sort.empty()) {
            if (sort.compare("asc") == 0) {
                sortDirection = DBCriteria::ascending;
            }
            else if (sort.compare("desc") == 0) {
                sortDirection = DBCriteria::descending;
            }
        }

        std::string total = getParameter("total");
        if (!total.empty()) {
            showTotal = true;
        }
    }

    DBTransactionView transactionInstance;
    DBResult<DBTransactionView> result = 
        transactionInstance.listByAccountID(
                                selectedAccount.id, 
                                recurringType, 
                                thisPeriod, 
                                sortDirection, 
                                rowLimit);

    if (Terminal::isOverWidthThreshold()) {
        TransactionListView view;

        if (showTotal) {
            view.addTotal();
        }
        
        view.addResults(result);
        view.show();
    }
    else {
        TransactionDetailsListView view;

        if (showTotal) {
            view.addTotal();
        }
        
        view.addResults(result);
        view.show();
    }

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    cacheMgr.clearTransactions();

    for (size_t i = 0;i < result.size();i++) {
        DBTransactionView transaction = result[i];
        cacheMgr.addTransaction(transaction.sequence, transaction);
    }
}

void Command::findTransactions() {
    if (hasParameters()) {
        std::string sql = getParameter("sql");

        if (!sql.empty()) {
            findTransactions(sql);
        }
        else {
            DBCriteria criteria;
            
            std::string dateAfter = getParameter("date>");
            if (!dateAfter.empty()) {
                criteria = DBTransactionView::FindCriteriaHelper::handleGreaterThanThisDate(criteria, dateAfter);
            }

            std::string dateBefore = getParameter("date<");
            if (!dateBefore.empty()) {
                criteria = DBTransactionView::FindCriteriaHelper::handleLessThanThisDate(criteria, dateBefore);
            }

            std::vector<std::string> dates = getParameters("date");
            std::vector<StrDate> onTheseDates;
            for (std::string & s : dates) {
                onTheseDates.push_back(s);
            }
            criteria = DBTransactionView::FindCriteriaHelper::handleOnTheseDates(criteria, onTheseDates);

            std::vector<std::string> withTheseAccountsList = getParameters("acc");
            criteria = DBTransactionView::FindCriteriaHelper::handleWithTheseAccounts(criteria, withTheseAccountsList);

            std::vector<std::string> withTheseCategoriesList = getParameters("c");
            criteria = DBTransactionView::FindCriteriaHelper::handleWithTheseCategories(criteria, withTheseCategoriesList);

            std::vector<std::string> withThesePayeesList = getParameters("p");
            criteria = DBTransactionView::FindCriteriaHelper::handleWithThesePayees(criteria, withThesePayeesList);
            
            std::string amountGreater = getParameter("amnt>");
            if (!amountGreater.empty()) {
                criteria = DBTransactionView::FindCriteriaHelper::handleGreaterThanThisAmount(criteria, amountGreater);
            }
            
            std::string amountLess = getParameter("amnt<");
            if (!amountLess.empty()) {
                criteria = DBTransactionView::FindCriteriaHelper::handleLessThanThisAmount(criteria, amountLess);
            }

            std::string type = getParameter("type");
            transform(type.begin(), type.end(), type.begin(), ::toupper);
            criteria = DBTransactionView::FindCriteriaHelper::handleWithThisType(criteria, type);

            std::string recurring = getParameter("rec");
            if (!recurring.empty()) {
                criteria = DBTransactionView::FindCriteriaHelper::handleIsRecurring(criteria, recurring.compare("r") == 0 ? true : false);
            }

            auto replaceWildcards = [](std::string & s) {
                for (size_t i = 0;i < s.length();i++) {
                    if (s[i] == '*') {
                        s[i] = '%';
                    }
                    else if (s[i] == '?') {
                        s[i] = '_';
                    }
                }

                return s;
            };

            std::string description = getParameter("desc");
            if (!description.empty()) {
                criteria = DBTransactionView::FindCriteriaHelper::handleWithThisDescription(criteria, replaceWildcards(description));
            }

            std::string reference = getParameter("ref");
            if (!reference.empty()) {
                criteria = DBTransactionView::FindCriteriaHelper::handleWithThisReference(criteria, replaceWildcards(reference));
            }

            findTransactions(criteria);
        }

        return;
    }

    FindTransactionView view;
    view.show();
    
    DBCriteria criteria = view.getCriteria();
    
    findTransactions(criteria);
}

void Command::findTransactions(const std::string & where) {
    DBTransactionView tr;
    DBResult<DBTransactionView> result = tr.findTransactions(where);

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    cacheMgr.clearTransactions();
    
    for (size_t i = 0;i < result.size();i++) {
        DBTransactionView transaction = result.at(i);
        cacheMgr.addTransaction(transaction.sequence, transaction);
    }

    cacheMgr.setFindCriteria(where);

    unsigned long terminalWidth = Terminal::getWidth();

    if (terminalWidth > LIST_VIEW_THRESHOLD_WIDTH) {
        TransactionListView view;
        view.addTotal();
        view.addResults(result);
        view.show();
    }
    else {
        TransactionDetailsListView view;
        view.addTotal();
        view.addResults(result);
        view.show();
    }
}

void Command::findTransactions(DBCriteria & criteria) {
    DBTransactionView tr;
    DBResult<DBTransactionView> result = tr.findTransactionsForCriteria(criteria);

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    cacheMgr.clearTransactions();
    
    for (size_t i = 0;i < result.size();i++) {
        DBTransactionView transaction = result.at(i);
        cacheMgr.addTransaction(transaction.sequence, transaction);
    }

    cacheMgr.setFindCriteria(criteria.getWhereCriteria());

    unsigned long terminalWidth = Terminal::getWidth();

    if (terminalWidth > LIST_VIEW_THRESHOLD_WIDTH) {
        TransactionListView view;
        view.addTotal();
        view.addResults(result);
        view.show();
    }
    else {
        TransactionDetailsListView view;
        view.addTotal();
        view.addResults(result);
        view.show();
    }
}

void Command::transactionsByCategory() {
    StrDate periodStart;
    StrDate periodEnd;

    if (hasParameters()) {
        int year = atoi(getParameter("y").c_str());
        int month = atoi(getParameter("m").c_str());
        int day = 15;

        StrDate reference = StrDate(year, month, day);
        periodStart = periodStart.getPeriodStartDate(reference);
        periodEnd = periodEnd.getPeriodEndDate(reference);
    }
    else {
        periodStart = StrDate::getPeriodStartDate();
        periodEnd = StrDate::getPeriodEndDate();
    }

    DBTransactionView tr;
    DBResult<DBTransactionView> result = tr.reportByCategory(selectedAccount, periodStart, periodEnd);

    TransactionCategoryReportListView view;
    view.addResults(result, selectedAccount.code, periodStart, periodEnd);
    view.show();
}

void Command::transactionsByPayee() {
    StrDate periodStart;
    StrDate periodEnd;

    if (hasParameters()) {
        int year = atoi(getParameter("y").c_str());
        int month = atoi(getParameter("m").c_str());
        int day = 15;

        StrDate reference = StrDate(year, month, day);
        periodStart = periodStart.getPeriodStartDate(reference);
        periodEnd = periodEnd.getPeriodEndDate(reference);
    }
    else {
        periodStart = StrDate::getPeriodStartDate();
        periodEnd = StrDate::getPeriodEndDate();
    }

    DBTransactionView tr;
    DBResult<DBTransactionView> result = tr.reportByPayee(selectedAccount, periodStart, periodEnd);

    TransactionPayeeReportListView view;
    view.addResults(result, selectedAccount.code, periodStart, periodEnd);
    view.show();
}

DBTransaction Command::getTransaction(int sequence) {
    int selectedSequence;

    if (sequence == 0) {
        ChooseTransactionView view;
        view.show();

        selectedSequence = view.getSequence();
    }
    else {
        selectedSequence = sequence;
    }

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    DBTransaction transaction = cacheMgr.getTransaction(selectedSequence);

    return transaction;
}

void Command::updateTransaction() {
    std::string sequence = getParameter(SEQUENCE_PARAM_NAME);

    DBTransaction transaction = getTransaction(atoi(sequence.c_str()));

    UpdateTransactionView view;
    view.setTransaction(transaction);
    view.show();

    DBTransaction updatedTransaction = view.getTransaction();

    DBTransaction currentTransaction;
    currentTransaction.id = updatedTransaction.id;
    currentTransaction.retrieve();

    /*
    ** If the transaction date has been updated, it is safest
    ** to removed the transactionand clear the id of the updated
    ** transaction to force an INSERT. Both of these actions will
    ** trigger the appropriate carried over logs to be updated.
    */
    if (currentTransaction.date != updatedTransaction.date) {
        currentTransaction.remove();

        updatedTransaction.id.clear();
    }

    updatedTransaction.save();
}

void Command::deleteTransaction() {
    std::string sequence = getParameter(SEQUENCE_PARAM_NAME);

    DBTransaction transaction = getTransaction(atoi(sequence.c_str()));

    transaction.remove();
}

void Command::reconcileTransaction() {
    std::string sequence = getParameter(SEQUENCE_PARAM_NAME);

    DBTransaction transaction = getTransaction(atoi(sequence.c_str()));

    transaction.retrieve();

    if (transaction.isReconciled) {
        transaction.isReconciled = false;
    }
    else {
        transaction.isReconciled = true;
    }
    
    transaction.save();
}

void Command::showTransaction() {
    std::string sequence = getParameter(SEQUENCE_PARAM_NAME);
    DBTransaction transaction = getTransaction(atoi(sequence.c_str()));

    DBTransactionView v;
    v.retrieve(transaction.id);

    TransactionDetailsView view;

    view.setTransaction(v);
    view.show();
}

bool Command::matchExistingTransactions(DBTransactionView & matchTransaction) {
    DBCriteria matchCriteria = matchTransaction.getMatchCriteria();

    DBTransactionView transactionToMatch;
    DBResult<DBTransactionView> matches = transactionToMatch.findTransactionsForCriteria(matchCriteria);

    bool isMatched = false;

    if (matches.size() == 1) {
        DBTransactionView tr = matches[0];

        std::cout << "Matched transaction:" << std::endl;
        matchTransaction.print();
        std::cout << "With:" << std::endl;
        tr.print();

        isMatched = true;
    }
    else if (matches.size() > 1) {
        std::cout << "Matched transaction:" << std::endl;
        matchTransaction.print();

        for (size_t i = 0;i < matches.size();i++) {
            DBTransactionView tr = matches[i];

            std::cout << "With:" << std::endl;
            tr.print();
        }

        isMatched = true;
    }
    else {
        isMatched = false;
    }

    return isMatched;
}

void Command::importTransactions() {
    std::string jsonFileName = getParameter(SIMPLE_PARAM_NAME);

    JFileReader jfile = JFileReader(jsonFileName);
    jfile.validate("DBTransaction");

    std::vector<JRecord> records = jfile.read("transactions");

    for (JRecord & record : records) {
        DBTransaction transaction;

        transaction.set(record);

        DBTransactionView transactionView = transaction;

        if (!matchExistingTransactions(transactionView)) {
            transaction.save();
        }
    }
}

void Command::exportTransactions() {
    std::string jsonFileName = getParameter(SIMPLE_PARAM_NAME);

    DBResult<DBTransaction> results;
    results.retrieveAll();

    std::vector<JRecord> records;

    for (size_t i = 0;i < results.size();i++) {
        DBTransaction transaction = results.at(i);

        JRecord r = transaction.getRecord();
        records.push_back(r);
    }
    
    JFileWriter jFile(jsonFileName, "DBTransaction");
    jFile.write(records, "transactions");
}

void Command::exportTransactionsAsCSV() {
    std::string csvFileName = getParameter(SIMPLE_PARAM_NAME);

    DBResult<DBTransaction> results;
    results.retrieveAll();

    std::ofstream out(csvFileName);

    out << DBTransaction::getCSVHeader();

    for (size_t i = 0;i < results.size();i++) {
        DBTransaction transaction = results.at(i);

        std::string record = transaction.getCSVRecord();
        out << record;
    }
    
    out.close();
}

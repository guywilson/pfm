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

using namespace std;

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
            string code = getParameter("c");
            category.retrieveByCode(code);
            transaction.categoryId = category.id;
        }
        catch (pfm_error & e) {
            transaction.categoryId.clear();
        }

        try {
            DBPayee payee;
            string code = getParameter("p");
            payee.retrieveByCode(code);
            transaction.payeeId = payee.id;
        }
        catch (pfm_error & e) {
            transaction.payeeId.clear();
        }

        string date = getParameter("date");
        transaction.date = date.empty() ? StrDate::today() : date;

        string type = getParameter("type");
        transform(type.begin(), type.end(), type.begin(), ::toupper);
        transaction.type = type.empty() ? TYPE_DEBIT : type;

        string reconciled = getParameter("rec");
        transaction.isReconciled = reconciled == "Y" ? true : false;
        
        transaction.accountId = selectedAccount.id;
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

void Command::addTransferTransaction() {
    checkAccountSelected();

    if (hasParameters()) {
        DBTransaction transaction;
        DBAccount accountTo;

        string accountToCode = getParameter("to");

        if (accountToCode.empty()) {
            throw pfm_error("addTransferTransaction: The account to transfer to must be supplied");
        }

        accountTo.retrieveByCode(accountToCode);

        try {
            DBCategory category;
            string code = getParameter("c");
            category.retrieveByCode(code);
            transaction.categoryId = category.id;
        }
        catch (pfm_error & e) {
            transaction.categoryId.clear();
        }

        string date = getParameter("date");
        transaction.date = date.empty() ? StrDate::today() : date;
        
        transaction.description = getParameter("desc");
        transaction.amount = getParameter("amnt");
        transaction.accountId = selectedAccount.id;

        string reconciled = getParameter("rec");
        transaction.isReconciled = (reconciled == "Y" ? true : false);

        DBTransaction::createTransferPairFromSource(transaction, accountTo);
        return;
    }

    TransferToAccountView view;
    view.show();

    DBTransaction sourceTransaction = view.getSourceTransaction();
    sourceTransaction.accountId = selectedAccount.id;

    DBAccount accountTo = view.getAccountTo();

    DBTransaction::createTransferPairFromSource(sourceTransaction, accountTo);
}

void Command::listTransactions() {
    checkAccountSelected();

    bool includeRecurring = false;
    uint32_t rowLimit = 25;
    DBCriteria::sql_order sortDirection = DBCriteria::descending;

    if (hasParameters()) {
        string rows = getParameter("rows");
        if (!rows.empty()) {
            rowLimit = strtoul(rows.c_str(), NULL, 10);
        }
        
        string recurring = getParameter("recurring");
        if (!recurring.empty()) {
            if (recurring.compare("all") == 0) {
                includeRecurring = true;
            }
            else if (recurring.compare("nr") == 0) {
                includeRecurring = false;
            }
        }

        string sort = getParameter("sort");
        if (!sort.empty()) {
            if (sort.compare("asc") == 0) {
                sortDirection = DBCriteria::ascending;
            }
            else if (sort.compare("desc") == 0) {
                sortDirection = DBCriteria::descending;
            }
        }
    }

    DBTransactionView transactionInstance;
    DBResult<DBTransactionView> result;

    if (includeRecurring) {
        result = transactionInstance.retrieveByAccountID(selectedAccount.id, sortDirection, rowLimit);
    }
    else {
        result = transactionInstance.retrieveNonRecurringByAccountID(selectedAccount.id, sortDirection, rowLimit);
    }

    TransactionListView view;
    view.addResults(result, selectedAccount.code);
    view.show();

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    cacheMgr.clearTransactions();

    for (int i = 0;i < result.size();i++) {
        DBTransactionView transaction = result[i];
        cacheMgr.addTransaction(transaction.sequence, transaction);
    }
}

void Command::findTransactions() {
    if (hasParameters()) {
        string sql = getParameter("sql");

        if (!sql.empty()) {
            findTransactions(sql);
        }
        else {
            vector<StrDate> betweenTheseDatesList;
            vector<StrDate> onTheseDatesList;
            vector<string> withTheseAccountsList;
            vector<string> withTheseCategoriesList;
            vector<string> withThesePayeesList;
            vector<Money> betweenTheseAmountList;
            
            string dateAfter = getParameter("date>");
            if (!dateAfter.empty()) {
                betweenTheseDatesList.push_back(dateAfter);
            }

            string dateBefore = getParameter("date<");
            if (!dateBefore.empty()) {
                betweenTheseDatesList.push_back(dateBefore);
            }

            vector<string> dates = getParameters("date");
            for (string & s : dates) {
                onTheseDatesList.push_back(s);
            }

            withTheseAccountsList = getParameters("acc");
            withTheseCategoriesList = getParameters("c");
            withThesePayeesList = getParameters("p");
            
            string amountGreater = getParameter("amnt>");
            if (!amountGreater.empty()) {
                betweenTheseAmountList.push_back(amountGreater);
            }
            
            string amountLess = getParameter("amnt<");
            if (!amountLess.empty()) {
                betweenTheseAmountList.push_back(amountLess);
            }

            string type = getParameter("type");
            transform(type.begin(), type.end(), type.begin(), ::toupper);

            string recurring = getParameter("rec");

            auto replaceWildcards = [](string & s) {
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

            string description = getParameter("desc");
            if (!description.empty()) {
                description = replaceWildcards(description);
            }

            string reference = getParameter("ref");
            if (!reference.empty()) {
                reference = replaceWildcards(reference);
            }

            DBCriteria criteria;

            criteria = DBTransactionView::FindCriteriaHelper::handleBetweenTheseDates(criteria, betweenTheseDatesList);
            criteria = DBTransactionView::FindCriteriaHelper::handleOnTheseDates(criteria, onTheseDatesList);

            criteria = DBTransactionView::FindCriteriaHelper::handleBetweenTheseAmounts(criteria, betweenTheseAmountList);

            criteria = DBTransactionView::FindCriteriaHelper::handleWithTheseAccounts(criteria, withTheseAccountsList);
            criteria = DBTransactionView::FindCriteriaHelper::handleWithTheseCategories(criteria, withTheseCategoriesList);
            criteria = DBTransactionView::FindCriteriaHelper::handleWithThesePayees(criteria, withThesePayeesList);

            criteria = DBTransactionView::FindCriteriaHelper::handleWithThisDescription(criteria, description);
            criteria = DBTransactionView::FindCriteriaHelper::handleWithThisReference(criteria, reference);
            criteria = DBTransactionView::FindCriteriaHelper::handleWithThisType(criteria, type);

            if (!recurring.empty()) {
                criteria = DBTransactionView::FindCriteriaHelper::handleIsRecurring(criteria, recurring.compare("r") == 0 ? true : false);
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

void Command::findTransactions(const string & where) {
    DBTransactionView tr;
    DBResult<DBTransactionView> result = tr.findTransactions(where);

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    cacheMgr.clearTransactions();
    
    for (int i = 0;i < result.size();i++) {
        DBTransactionView transaction = result.at(i);
        cacheMgr.addTransaction(transaction.sequence, transaction);
    }

    cacheMgr.addFindCriteria(where);

    TransactionListView view;
    view.addTotal();
    view.addResults(result);
    view.show();
}

void Command::findTransactions(DBCriteria & criteria) {
    DBTransactionView tr;
    DBResult<DBTransactionView> result = tr.findTransactionsForCriteria(criteria);

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    cacheMgr.clearTransactions();
    
    for (int i = 0;i < result.size();i++) {
        DBTransactionView transaction = result.at(i);
        cacheMgr.addTransaction(transaction.sequence, transaction);
    }

    cacheMgr.addFindCriteria(criteria.getWhereClause());

    TransactionListView view;
    view.addTotal();
    view.addResults(result);
    view.show();
}

void Command::transactionsByCategory() {
    DBTransactionView tr;
    DBResult<DBTransactionView> result = tr.reportByCategory();

    TransactionCategoryReportListView view;
    view.addResults(result);
    view.show();
}

void Command::transactionsByPayee() {
    DBTransactionView tr;
    DBResult<DBTransactionView> result = tr.reportByPayee();

    TransactionPayeeReportListView view;
    view.addResults(result);
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
    string sequence = getParameter(SEQUENCE_PARAM_NAME);

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
    string sequence = getParameter(SEQUENCE_PARAM_NAME);

    DBTransaction transaction = getTransaction(atoi(sequence.c_str()));

    transaction.remove();
}

void Command::reconcileTransaction() {
    string sequence = getParameter(SEQUENCE_PARAM_NAME);

    DBTransaction transaction = getTransaction(atoi(sequence.c_str()));

    transaction.retrieve();
    transaction.isReconciled = true;
    transaction.save();
}

void Command::importTransactions() {
    string jsonFileName = getParameter(SIMPLE_PARAM_NAME);

    JFileReader jfile = JFileReader(jsonFileName);
    jfile.validate("DBTransaction");

    vector<JRecord> records = jfile.read("transactions");

    for (JRecord & record : records) {
        DBAccount account;

        account.set(record);
        account.save();
    }
}

void Command::exportTransactions() {
    string jsonFileName = getParameter(SIMPLE_PARAM_NAME);

    DBResult<DBAccount> results;
    results.retrieveAll();

    vector<JRecord> records;

    for (int i = 0;i < results.size();i++) {
        DBAccount account = results.at(i);

        JRecord r = account.getRecord();
        records.push_back(r);
    }
    
    JFileWriter jFile = JFileWriter(jsonFileName, "DBTransaction");
    jFile.write(records, "transactions");
}

void Command::exportTransactionsAsCSV() {
    string csvFileName = getParameter(SIMPLE_PARAM_NAME);

    DBResult<DBTransaction> results;
    results.retrieveAll();

    ofstream out(csvFileName);

    out << DBTransaction::getCSVHeader();

    for (int i = 0;i < results.size();i++) {
        DBTransaction transaction = results.at(i);

        string record = transaction.getCSVRecord();
        out << record;
    }
    
    out.close();
}

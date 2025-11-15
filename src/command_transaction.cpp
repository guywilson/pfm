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

#include "db_transaction.h"
#include "db_v_transaction.h"
#include "transaction_views.h"

using namespace std;

void Command::clearRecurringTransactions() {
    checkAccountSelected();

    DBTransaction tr;
    tr.deleteAllRecurringTransactionsForAccount(selectedAccount.id);
}

void Command::addTransaction() {
    checkAccountSelected();

    AddTransactionView view;
    view.show();

    DBTransaction transaction = view.getTransaction();
    transaction.accountId = selectedAccount.id;
    transaction.save();
}

void Command::addTransaction(AddTransactionCriteriaBuilder & builder) {
    checkAccountSelected();

    DBTransaction transaction;

    try {
        DBCategory category;
        category.retrieveByCode(builder.categoryCode);
        transaction.categoryId = category.id;
    }
    catch (pfm_error & e) {
        transaction.categoryId.clear();
    }

    try {
        DBPayee payee;
        payee.retrieveByCode(builder.payeeCode);
        transaction.payeeId = payee.id;
    }
    catch (pfm_error & e) {
        transaction.payeeId.clear();
    }

    transaction.accountId = selectedAccount.id;
    transaction.date = builder.date;
    transaction.description = builder.description;
    transaction.reference = builder.reference;
    transaction.amount = builder.amount;
    transaction.type = builder.type;

    transaction.save();
}

void Command::addTransferTransaction() {
    checkAccountSelected();

    TransferToAccountView view;
    view.setSourceAccountCode(selectedAccount.code);
    view.show();

    DBTransaction sourceTransaction = view.getSourceTransaction();
    sourceTransaction.accountId = selectedAccount.id;

    DBAccount accountTo = view.getAccountTo();

    DBTransaction::createTransferPairFromSource(sourceTransaction, accountTo);
}

void Command::listTransactions(uint32_t rowLimit, DBCriteria::sql_order sortDirection, bool includeRecurring) {
    checkAccountSelected();

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
        DBTransactionView transaction = result.at(i);
        cacheMgr.addTransaction(transaction.sequence, transaction);
    }
}

void Command::findTransactions() {
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

void Command::updateTransaction(DBTransaction & transaction) {
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

void Command::deleteTransaction(DBTransaction & transaction) {
    transaction.remove();
}

void Command::reconcileTransaction(DBTransaction & transaction) {
    transaction.retrieve();

    transaction.isReconciled = true;

    transaction.save();
}

void Command::importTransactions(string & jsonFileName) {
    JFileReader jfile = JFileReader(jsonFileName, "DBTransaction");

    vector<JRecord> records = jfile.read("transactions");

    for (JRecord & record : records) {
        DBTransaction transaction;

        transaction.set(record);
        transaction.save();
    }
}

void Command::exportTransactions(string & jsonFileName) {
    DBResult<DBTransaction> results;
    results.retrieveAll();

    vector<JRecord> records;

    for (int i = 0;i < results.size();i++) {
        DBTransaction transaction = results.at(i);

        JRecord r = transaction.getRecord();
        records.push_back(r);
    }
    
    JFileWriter jFile = JFileWriter(jsonFileName, "DBTransaction");
    jFile.write(records, "transactions");
}

void Command::exportTransactionsAsCSV(string & csvFileName) {
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

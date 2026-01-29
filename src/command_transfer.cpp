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

#include "db_transfer_transaction_record.h"
#include "db_v_transfer_record.h"
#include "transfer_transaction_views.h"

using namespace std;

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

void Command::listTransferRecords() {
    DBResult<DBTransferRecordView> result;
    result.retrieveAll(); 

    TransferListView view;

    view.addTotal();
    
    view.addResults(result);
    view.show();

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    cacheMgr.clearTransactions();

    for (int i = 0;i < result.size();i++) {
        DBTransferRecordView transfer = result[i];

        DBTransferTransactionRecord record;
        record.retrieve(transfer.id);

        cacheMgr.addTransfer(transfer.sequence, record);
    }
}

DBTransferTransactionRecord Command::getTransferRecord(int sequence) {
    int selectedSequence;

    if (sequence == 0) {
        ChooseTransferView view;
        view.show();

        selectedSequence = view.getSequence();
    }
    else {
        selectedSequence = sequence;
    }

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    DBTransferTransactionRecord transfer = cacheMgr.getTransfer(selectedSequence);

    return transfer;
}

void Command::deleteTransferRecord() {
    Logger & log = Logger::getInstance();
    log.entry("Command::deleteTransferRecord()");

    string sequence = getParameter(SEQUENCE_PARAM_NAME);

    DBTransferTransactionRecord transfer = getTransferRecord(atoi(sequence.c_str()));

    DBTransaction transactionFrom;
    DBTransaction transactionTo;

    transactionFrom.id = transfer.transactionFromId;
    transactionTo.id = transfer.transactionToId;

    PFM_DB & db = PFM_DB::getInstance();

    try {
        db.begin();

        transfer.remove();

        transactionFrom.remove();
        transactionTo.remove();

        db.commit();
    }
    catch (pfm_error & e) {
        log.error("Failed to delete transfer transaction record: %s", e.what());
        db.rollback();

        throw e;
    }

    log.exit("Command::deleteTransferRecord()");
}

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pfm_error.h"
#include "db.h"
#include "cache.h"
#include "strdate.h"
#include "command.h"
#include "db_account.h"
#include "db_category.h"
#include "db_payee.h"
#include "db_recurring_charge.h"
#include "db_carried_over.h"
#include "db_transaction.h"
#include "db_budget.h"

using namespace std;

#define NUM_PAYEES                      21

const char * payeeList[][2] {
    {"EDF","EDF Energy"},
    {"BEXL","Bexley Borough Council"},
    {"HALFX","Halifax Building Society"},
    {"VODA","Vodafone"},
    {"TWAT","Thames Water"},
    {"BT","British Telecommunications"},
    {"MBNA","MBNA Credit Card"},
    {"ADOBE","Adobe CC"},
    {"RNLI","RNLI"},
    {"AVIVA","Aviva Insurance"},
    {"SAINS","Sainsburys"},
    {"ASDA","Asda Supermarket"},
    {"TFL","Transport for London"},
    {"WASAB","Wasabi Bento"},
    {"PRET","Pret a Manger"},
    {"UBER","Uber"},
    {"AMAZN","Amazon"},
    {"MAS","Marks & Spencer"},
    {"BAQ","B & Q"},
    {"APPLE","Apple Inc"},
    {"NSPRO","Nesspresso"}
};

static void createPayees() {
    PFM_DB & db = PFM_DB::getInstance();

    DBPayee payee;

    try {
        db.begin();

        for (int i = 0;i < NUM_PAYEES;i++) {
            payee.clear();
            payee.code = payeeList[i][0];
            payee.name = payeeList[i][1];
            payee.save();
        }

        db.commit();
    }
    catch (pfm_error & e) {
        db.rollback();
        throw pfm_error("Failed to insert payee list", __FILE__, __LINE__);
    }
}

pfm_id_t getCategoryId(const char * categoryCode) {
    string code(categoryCode);

    DBCategory category;
    category.retrieveByCode(code);

    return category.id;
}

pfm_id_t getPayeeId(const char * payeeCode) {
    string code(payeeCode);

    DBPayee payee;
    payee.retrieveByCode(code);

    return payee.id;
}

static pfm_id_t createAccount() {
    DBAccount account;
    account.code = "HSBC";
    account.name = "HSBC Current Account";
    account.currentBalance = 4238.72;
    account.openingBalance = 4238.72;
    account.save();

    return account.id;
}

#ifdef PFM_TEST_SUITE_ENABLED
extern void setTodayTestDate(const char * date);
#endif

void testAccount() {
    createPayees();
    pfm_id_t accountId = createAccount();

#ifdef PFM_TEST_SUITE_ENABLED
    setTodayTestDate("2024-08-16");
#endif
    StrDate today;

    Logger & log = Logger::getInstance();

    DBRecurringCharge charge1;
    DBRecurringCharge charge2;
    DBRecurringCharge charge3;

    try {
        charge1.accountId = accountId;
        charge1.amount = 126.58;
        charge1.date.set(today.year(), 1, 1);
        charge1.description = "Energy bill";
        charge1.frequency = "1m";
        charge1.categoryId = getCategoryId("UTILS");
        charge1.payeeId = getPayeeId("EDF");
        charge1.save();
    }
    catch (pfm_error & e) {
        log.logError("testAccount(): Failed to create charge1: '%s'", e.what());
        throw e;
    }

    try {
        charge2.accountId = accountId;
        charge2.amount = 9.98;
        charge2.date.set(today.year(), 2, 16);
        charge2.description = "Adobe Creative Cloud";
        charge2.frequency = "1m";
        charge2.categoryId = getCategoryId("HHOLD");
        charge2.payeeId = getPayeeId("ADOBE");
        charge2.save();
    }
    catch (pfm_error & e) {
        log.logError("testAccount(): Failed to create charge2: '%s'", e.what());
        throw e;
    }

    try {
        charge3.accountId = accountId;
        charge3.amount = 5.00;
        charge3.date.set(today.year(), 3, 25);
        charge3.description = "RNLI Contribution";
        charge3.frequency = "1m";
        charge3.categoryId = getCategoryId("CHARY");
        charge3.payeeId = getPayeeId("RNLI");
        charge3.save();
    }
    catch (pfm_error & e) {
        log.logError("testAccount(): Failed to create charge3: '%s'", e.what());
        throw e;
    }

    Command command;

    string cmdString = "use HSBC";
    command.process(cmdString);

    /*
    ** Check that we have the right number of transactions created...
    */
    DBTransaction tr;
    DBResult<DBTransaction> trResult1 = tr.retrieveByRecurringChargeID(charge1.id);

    if (trResult1.getNumRows() != 8) {
        throw pfm_error(pfm_error::buildMsg("Expected 8 transactions, got %d", trResult1.getNumRows()));
    }

    DBResult<DBTransaction> trResult2 = tr.retrieveByRecurringChargeID(charge2.id);

    if (trResult2.getNumRows() != 7) {
        throw pfm_error(pfm_error::buildMsg("Expected 7 transactions, got %d", trResult2.getNumRows()));
    }

    DBResult<DBTransaction> trResult3 = tr.retrieveByRecurringChargeID(charge3.id);

    if (trResult3.getNumRows() != 5) {
        throw pfm_error(pfm_error::buildMsg("Expected 5 transactions, got %d", trResult3.getNumRows()));
    }

    /*
    ** Check that we have the right number of carried-over logs...
    */
    DBCarriedOver co;
    DBResult<DBCarriedOver> coResultBeforeInsert = co.retrieveByAccountId(accountId);

    if (coResultBeforeInsert.getNumRows() != 7) {
        throw pfm_error(pfm_error::buildMsg("Expected 7 carried over logs, got %d", coResultBeforeInsert.getNumRows()));
    }

    /*
    ** Insert a transaction and check carried over logs...
    */
    DBTransaction trTest;
    trTest.accountId = accountId;
    trTest.amount = 28.30;
    trTest.categoryId = getCategoryId("LUNCH");
    trTest.date = "2024-03-24";
    trTest.description = "Lunch";
    trTest.isCredit = false;
    trTest.isReconciled = false;
    trTest.payeeId = getPayeeId("WASAB");
    trTest.recurringChargeId = 0;
    trTest.save();

    DBResult<DBCarriedOver> coResultAfterInsert = co.retrieveByAccountId(accountId);

    for (int i = 0;i < coResultAfterInsert.getNumRows();i++) {
        DBCarriedOver coBefore = coResultBeforeInsert.getResultAt(i);
        DBCarriedOver coAfter = coResultAfterInsert.getResultAt(i);

        if (coAfter.id >= 3) {
            Money expected = (coBefore.balance - trTest.amount);

            if (coAfter.balance != expected) {
                throw pfm_error(
                        pfm_error::buildMsg(
                            "Carried Over Test 1: Expected balance %.2f, got %.2f", 
                            expected.getDoubleValue(), 
                            coAfter.balance.getDoubleValue()),
                        __FILE__,
                        __LINE__);
            }
        }
    }

    /*
    ** Update the transaction amount...
    */
    trTest.amount = 8.30;
    trTest.save();

    DBResult<DBCarriedOver> coResultAfterUpdate = co.retrieveByAccountId(accountId);

    for (int i = 0;i < coResultAfterUpdate.getNumRows();i++) {
        DBCarriedOver coBefore = coResultBeforeInsert.getResultAt(i);
        DBCarriedOver coAfter = coResultAfterUpdate.getResultAt(i);

        if (coAfter.id >= 3) {
            Money expected = (coBefore.balance - trTest.amount);

            if (coAfter.balance != expected) {
                throw pfm_error(
                        pfm_error::buildMsg(
                            "Carried Over Test 2: Expected balance %.2f, got %.2f", 
                            expected.getDoubleValue(), 
                            coAfter.balance.getDoubleValue()),
                        __FILE__,
                        __LINE__);
            }
        }
    }

    /*
    ** Update the transaction from debit to credit...
    */
    trTest.isCredit = true;
    trTest.save();

    DBResult<DBCarriedOver> coResultAfterUpdate2 = co.retrieveByAccountId(accountId);

    for (int i = 0;i < coResultAfterUpdate2.getNumRows();i++) {
        DBCarriedOver coBefore = coResultBeforeInsert.getResultAt(i);
        DBCarriedOver coAfter = coResultAfterUpdate2.getResultAt(i);

        if (coAfter.id >= 3) {
            Money expected = (coBefore.balance + trTest.amount);

            if (coAfter.balance != expected) {
                throw pfm_error(
                        pfm_error::buildMsg(
                            "Carried Over Test 3: Expected balance %.2f, got %.2f", 
                            expected.getDoubleValue(), 
                            coAfter.balance.getDoubleValue()),
                        __FILE__,
                        __LINE__);
            }
        }
    }

    /*
    ** Delete the transaction...
    */
    trTest.remove();

    DBResult<DBCarriedOver> coResultAfterDelete = co.retrieveByAccountId(accountId);

    for (int i = 0;i < coResultAfterDelete.getNumRows();i++) {
        DBCarriedOver coBefore = coResultBeforeInsert.getResultAt(i);
        DBCarriedOver coAfter = coResultAfterDelete.getResultAt(i);

        if (coAfter.id >= 3) {
            Money expected = coBefore.balance;

            if (coAfter.balance != expected) {
                throw pfm_error(
                        pfm_error::buildMsg(
                            "Carried Over Test 4: Expected balance %.2f, got %.2f", 
                            expected.getDoubleValue(), 
                            coAfter.balance.getDoubleValue()),
                        __FILE__,
                        __LINE__);
            }
        }
    }
}

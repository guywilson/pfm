#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pfm_error.h"
#include "db.h"
#include "cache.h"
#include "cli.h"
#include "strdate.h"
#include "db_account.h"
#include "db_category.h"
#include "db_payee.h"
#include "db_recurring_charge.h"
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

extern void setTodayTestDate(const char * date);

void testAccount() {
    createPayees();
    pfm_id_t accountId = createAccount();

    setTodayTestDate("2024-08-16");

    StrDate today;

    DBRecurringCharge charge1;
    charge1.accountId = accountId;
    charge1.amount = 126.58;
    charge1.date.set(today.year(), 1, 1);
    charge1.description = "Energy bill";
    charge1.frequency = "1m";
    charge1.categoryId = getCategoryId("UTILS");
    charge1.payeeId = getPayeeId("EDF");
    charge1.save();

    DBRecurringCharge charge2;
    charge2.accountId = accountId;
    charge2.amount = 9.98;
    charge2.date.set(today.year(), 2, 16);
    charge2.description = "Adobe Creative Cloud";
    charge2.frequency = "1m";
    charge2.categoryId = getCategoryId("HHOLD");
    charge2.payeeId = getPayeeId("ADOBE");
    charge2.save();

    DBRecurringCharge charge3;
    charge3.accountId = accountId;
    charge3.amount = 5.00;
    charge3.date.set(today.year(), 3, 25);
    charge3.description = "RNLI Contribution";
    charge3.frequency = "1m";
    charge3.categoryId = getCategoryId("CHARY");
    charge3.payeeId = getPayeeId("RNLI");
    charge3.save();

    chooseAccount("HSBC");

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
}

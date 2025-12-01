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

#include "db_recurring_charge.h"
#include "recurring_charge_views.h"

using namespace std;

void Command::addRecurringCharge() {
    checkAccountSelected();

    if (hasParameters()) {
        DBRecurringCharge charge;

        try {
            DBCategory category;
            string code = getParameter("c");
            category.retrieveByCode(code);
            charge.categoryId = category.id;
        }
        catch (pfm_error & e) {
            charge.categoryId.clear();
        }

        try {
            DBPayee payee;
            string code = getParameter("p");
            payee.retrieveByCode(code);
            charge.payeeId = payee.id;
        }
        catch (pfm_error & e) {
            charge.payeeId.clear();
        }

        string date = getParameter("date");
        charge.date = date.empty() ? StrDate::today() : date;

        string end = getParameter("end");
        StrDate nullDate;
        nullDate.clear();
        charge.endDate = end.empty() ? nullDate : end;
        
        charge.accountId = selectedAccount.id;
        charge.description = getParameter("desc");
        charge.frequency = Frequency::parse(getParameter("freq"));
        charge.amount = getParameter("amnt");

        charge.save();
        return;
    }

    AddRecurringChargeView view;
    view.show();

    DBRecurringCharge charge = view.getRecurringCharge();
    charge.accountId = selectedAccount.id;

    charge.save();
}

void Command::listRecurringCharges() {
    checkAccountSelected();

    DBRecurringChargeView chargeInstance;
    DBResult<DBRecurringChargeView> result = chargeInstance.retrieveByAccountID(selectedAccount.id);

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    cacheMgr.clearRecurringCharges();

    for (int i = 0;i < result.size();i++) {
        DBRecurringCharge charge = result.at(i);
        cacheMgr.addRecurringCharge(charge.sequence, charge);
    }

    RecurringChargeListView view;
    view.addResults(result, selectedAccount.code);
    view.show();
}

DBRecurringCharge Command::getRecurringCharge(int sequence) {
    int selectedSequence;

    if (sequence == 0) {
        ChooseRecurringChargeView view;
        view.show();

        selectedSequence = view.getSequence();
    }
    else {
        selectedSequence = sequence;
    }

    CacheMgr & cacheMgr = CacheMgr::getInstance();
    DBRecurringCharge charge = cacheMgr.getRecurringCharge(selectedSequence);

    charge.retrieve();

    return charge;
}

void Command::updateRecurringCharge() {
    string sequence = getParameter(SEQUENCE_PARAM_NAME);

    DBRecurringCharge charge = getRecurringCharge(atoi(sequence.c_str()));

    UpdateRecurringChargeView view;
    
    view.setRecurringCharge(charge);
    view.show();

    DBRecurringCharge updatedCharge = view.getRecurringCharge();

    updatedCharge.save();
}

void Command::deleteRecurringCharge() {
    string sequence = getParameter(SEQUENCE_PARAM_NAME);

    DBRecurringCharge charge = getRecurringCharge(atoi(sequence.c_str()));

    PFM_DB & db = PFM_DB::getInstance();

    try {
        db.begin();

        charge.remove();
        charge.clear();

        db.commit();
    }
    catch (pfm_error & e) {
        db.rollback();
    }
}

void Command::importRecurringCharges() {
    string jsonFileName = getParameter(SIMPLE_PARAM_NAME);

    JFileReader jfile = JFileReader(jsonFileName, "DBRecurringCharge");

    vector<JRecord> records = jfile.read("charges");

    for (JRecord & record : records) {
        DBRecurringCharge charge;

        charge.set(record);
        charge.save();
    }
}

void Command::exportRecurringCharges() {
    string jsonFileName = getParameter(SIMPLE_PARAM_NAME);

    DBResult<DBRecurringCharge> results;
    results.retrieveAll();

    vector<JRecord> records;

    for (int i = 0;i < results.size();i++) {
        DBRecurringCharge charge = results.at(i);

        JRecord r = charge.getRecord();
        records.push_back(r);
    }
    
    JFileWriter jFile = JFileWriter(jsonFileName, "DBRecurringCharge");
    jFile.write(records, "charges");
}

void Command::migrateCharge() {
    string sequence = getParameter(SEQUENCE_PARAM_NAME);

    DBRecurringCharge charge = getRecurringCharge(atoi(sequence.c_str()));

    MigrateChargeView view;
    view.setCharge(charge);
    view.show();

    DBRecurringCharge updatedCharge = view.getCharge();

    updatedCharge.migrateToTransferCharge(updatedCharge.transfer.accountToId);
}

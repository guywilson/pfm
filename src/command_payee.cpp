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

#include "db_payee.h"
#include "payee_views.h"

using namespace std;

void Command::addPayee() {
    AddPayeeView view;
    view.show();

    DBPayee payee = view.getPayee();

    payee.save();
}

void Command::listPayees() {
    DBResult<DBPayee> result;
    result.retrieveAll();

    PayeeListView view;
    view.addResults(result);
    view.show();
}

DBPayee Command::getPayee(string & payeeCode) {
    if (payeeCode.length() == 0) {
        ChoosePayeeView view;
        view.show();

        payeeCode = view.getCode();
    }

    DBPayee payee;
    payee.retrieveByCode(payeeCode);

    return payee;
}

void Command::updatePayee(DBPayee & payee) {
    UpdatePayeeView view;
    view.setPayee(payee);
    view.show();

    DBPayee updatedPayee = view.getPayee();
    updatedPayee.save();
}

void Command::deletePayee(DBPayee & payee) {
    payee.remove();
    payee.clear();
}

void Command::importPayees(string & jsonFileName) {
    JFileReader jfile = JFileReader(jsonFileName, "DBPayee");

    vector<JRecord> records = jfile.read("payees");

    for (JRecord & record : records) {
        DBPayee payee;

        payee.set(record);
        payee.save();
    }
}

void Command::exportPayees(string & jsonFileName) {
    DBResult<DBPayee> results;
    results.retrieveAll();

    vector<JRecord> records;

    for (int i = 0;i < results.size();i++) {
        DBPayee payee = results.at(i);

        JRecord r = payee.getRecord();
        records.push_back(r);
    }
    
    JFileWriter jFile = JFileWriter(jsonFileName, "DBPayee");
    jFile.write(records, "payees");
}

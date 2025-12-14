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

void Command::updatePayee() {
    string payeeCode;

    if (hasParameters()) {
        payeeCode = getParameter(SIMPLE_PARAM_NAME);
    }

    DBPayee payee = getPayee(payeeCode);

    UpdatePayeeView view;
    view.setPayee(payee);
    view.show();

    DBPayee updatedPayee = view.getPayee();
    updatedPayee.save();
}

void Command::deletePayee() {
    string payeeCode;

    if (hasParameters()) {
        payeeCode = getParameter(SIMPLE_PARAM_NAME);
    }

    DBPayee payee = getPayee(payeeCode);

    payee.remove();
    payee.clear();
}

void Command::importPayees() {
    string jsonFileName = getParameter(SIMPLE_PARAM_NAME);

    JFileReader jFile = JFileReader(jsonFileName);

    DBPayee entity;
    entity.restore(jFile);
}

void Command::exportPayees() {
    string jsonFileName = getParameter(SIMPLE_PARAM_NAME);

    JFileWriter jFile = JFileWriter(jsonFileName);

    DBPayee entity;
    entity.backup(jFile);
}

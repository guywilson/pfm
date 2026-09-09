/*
 * Copyright (C) 2025-2026 Guy Wilson
 *
 * This file is part of PFM.
 *
 * PFM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PFM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PFM. If not, see <https://www.gnu.org/licenses/>.
 */

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


void Command::addPayee() {
    AddPayeeView view;
    view.show();

    DBPayee payee = view.getPayee();

    payee.save();
}

void Command::listPayees() {
    DBPayee payee;
    
    DBResult<DBPayee> result = payee.retrieveOrderedByCode();

    PayeeListView view;
    view.addResults(result);
    view.show();
}

DBPayee Command::getPayee(std::string & payeeCode) {
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
    std::string payeeCode;

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
    std::string payeeCode;

    if (hasParameters()) {
        payeeCode = getParameter(SIMPLE_PARAM_NAME);
    }

    DBPayee payee = getPayee(payeeCode);

    payee.remove();
    payee.clear();
}

void Command::importPayees() {
    std::string jsonFileName = getParameter(SIMPLE_PARAM_NAME);

    JFileReader jfile = JFileReader(jsonFileName);
    jfile.validate("DBPayee");

    std::vector<JRecord> records = jfile.read("payees");

    for (JRecord & record : records) {
        DBPayee payee;

        payee.set(record);
        payee.save();
    }
}

void Command::exportPayees() {
    std::string jsonFileName = getParameter(SIMPLE_PARAM_NAME);

    DBResult<DBPayee> results;
    results.retrieveAll();

    std::vector<JRecord> records;

    for (size_t i = 0;i < results.size();i++) {
        DBPayee payee = results.at(i);

        JRecord r = payee.getRecord();
        records.push_back(r);
    }
    
    JFileWriter jFile(jsonFileName, "DBPayee");
    jFile.write(records, "payees");
}

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

#include "db_account.h"
#include "db_category.h"
#include "db_payee.h"
#include "db_recurring_charge.h"
#include "db_transaction.h"
#include "db_public_holiday.h"
#include "cli_widget.h"

using namespace std;

static DBAccount * createSampleAccount() {
    DBAccount * account = new DBAccount();

    account->code = "BANK";
    account->name = "Sample bank account";
    account->openingBalance = 1234.56;
    account->openingDate = "2025-04-01";

    return account;
}

static DBPayee * createSamplePayee() {
    DBPayee * payee = new DBPayee();

    payee->code = "JOES";
    payee->name = "Joe's coffee shop";

    return payee;
}

static DBCategory * createSampleCategory() {
    DBCategory * category = new DBCategory();

    category->code = "COFFE";
    category->description = "Takeaway coffee";

    return category;
}

static DBPublicHoliday * createSampleHoliday() {
    DBPublicHoliday * holiday = new DBPublicHoliday();

    holiday->date = "01-01-2026";
    holiday->description = "New Year's day";

    return holiday;
}

static DBRecurringCharge * createSampleCharge() {
    DBRecurringCharge * charge = new DBRecurringCharge();

    DBAccount * account = createSampleAccount();
    DBCategory * category = createSampleCategory();
    DBPayee * payee = createSamplePayee();

    charge->account = *account;
    charge->amount = 12.63;
    charge->category = *category;
    charge->date = "2025-07-05";
    charge->description = "Sample charge";
    charge->endDate = "";
    charge->frequency = Frequency::parse("1m");
    charge->payee = *payee;

    return charge;
}

static DBTransaction * createSampleTransaction() {
    DBTransaction * transaction = new DBTransaction();

    DBAccount * account = createSampleAccount();
    DBCategory * category = createSampleCategory();
    DBPayee * payee = createSamplePayee();

    transaction->account = *account;
    transaction->amount = 15.78;
    transaction->category = *category;
    transaction->date = "2025-05-12";
    transaction->description = "Sample transaction";
    transaction->payee = *payee;
    transaction->reference = "";

    return transaction;
}

void Command::saveJsonTemplate() {
    cout << "For which entity do you want a JSON template:" << endl;
    cout << "1) Account" << endl;
    cout << "2) Payee" << endl;
    cout << "3) Category" << endl;
    cout << "4) Public Holiday" << endl;
    cout << "5) Recurring Charge" << endl;
    cout << "6) Transaction" << endl;

    CLITextField optionField = CLITextField("Enter option: ");
    optionField.show();

    int option = (int)optionField.getIntegerValue();

    cout << "Option = " << option << endl;

    DBEntity * entity;
    string name;
    string className;

    switch (option) {
        case 1:
            entity = createSampleAccount();
            name = "accounts";
            break;

        case 2:
            entity = createSamplePayee();
            name = "payees";
            break;

        case 3:
            entity = createSampleCategory();
            name = "categories";
            break;

        case 4:
            entity = createSampleHoliday();
            name = "holidays";
            break;

        case 5:
            entity = createSampleCharge();
            name = "charges";
            break;

        case 6:
            entity = createSampleTransaction();
            name = "transactions";
            break;

        default:
            throw pfm_validation_error(
                        pfm_error::buildMsg(
                            "saveJsonTemplate() : Invalid entity type %d", 
                            option), 
                        __FILE__, 
                        __LINE__);
    }

    string filename = name + "_template.json";
    JFileWriter writer(filename, entity->getClassName());

    vector<JRecord> records;

    records.push_back(entity->getRecord());
    records.push_back(entity->getRecord());

    writer.write(records, name);

    delete entity;
}

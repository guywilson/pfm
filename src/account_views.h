#include <iostream>
#include <string>
#include <string.h>
#include <vector>

#include "pfm_error.h"
#include "cli_widget.h"
#include "custom_widgets.h"
#include "db_account.h"

using namespace std;

#ifndef __ACCOUNT_VIEW
#define __ACCOUNT_VIEW

class AddAccountView : public CLIView {
    private:
        CLITextField nameField = CLITextField("Name: ");
        CLITextField codeField = CLITextField("Code (max. 5 chars): ");
        CLITextField openingBalanceField = CLITextField("Opening balance [0.00]: ");

    public:
        AddAccountView() : AddAccountView("Add account") {}

        AddAccountView(const char * title) : CLIView(title) {
            nameField.setLengthLimit(FIELD_STRING_LEN);
            codeField.setLengthLimit(CODE_FIELD_MAX_LENGTH);

            openingBalanceField.setDefaultValue("0.00");
            openingBalanceField.setLengthLimit(AMOUNT_FIELD_STRING_LEN);
        }

        void show() override {
            CLIView::show();

            nameField.show();
            codeField.show();
            openingBalanceField.show();
        }

        DBAccount getAccount() {
            DBAccount account;

            account.name = nameField.getValue();
            account.code = codeField.getValue();
            account.openingBalance = openingBalanceField.getDoubleValue();
            account.currentBalance = openingBalanceField.getDoubleValue();

            return account;
        }
};

class ChooseAccountView : public CLIView {
    private:
        CLITextField codeField = CLITextField("Code (max. 5 chars): ");

    public:
        ChooseAccountView() : ChooseAccountView("Use account") {}
        
        ChooseAccountView(const char * title) : CLIView(title) {
            codeField.setLengthLimit(CODE_FIELD_MAX_LENGTH);
        }

        void show() override {
            CLIView::show();

            codeField.show();
        }

        string getCode() {
            return codeField.getValue();
        }
};

class UpdateAccountView : public CLIView {
    private:
        sqlite3_int64 accountId;

        CLITextField nameField;
        CLITextField codeField;
        CLITextField openingBalanceField;
        CLITextField currentBalanceField;

    public:
        UpdateAccountView() : UpdateAccountView("Update account") {}

        UpdateAccountView(const char * title) : CLIView(title) {}

        void setAccount(DBAccount & account) {
            char szPrompt[MAX_PROMPT_LENGTH];
            char szBalance[AMOUNT_FIELD_STRING_LEN];

            accountId = account.id;

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Name ['%s']: ", account.name.c_str());
            nameField.setLabel(szPrompt);
            nameField.setDefaultValue(account.name);
            nameField.setLengthLimit(FIELD_STRING_LEN);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Code ['%s']: ", account.code.c_str());
            codeField.setLabel(szPrompt);
            codeField.setDefaultValue(account.code);
            codeField.setLengthLimit(CODE_FIELD_MAX_LENGTH);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Opening balance [%.2f]: ", account.openingBalance);
            snprintf(szBalance, AMOUNT_FIELD_STRING_LEN, "%.2f", account.openingBalance);
            openingBalanceField.setLabel(szPrompt);
            openingBalanceField.setDefaultValue(szBalance);
            openingBalanceField.setLengthLimit(AMOUNT_FIELD_STRING_LEN);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Current balance [%.2f]: ", account.currentBalance);
            snprintf(szBalance, AMOUNT_FIELD_STRING_LEN, "%.2f", account.currentBalance);
            currentBalanceField.setLabel(szPrompt);
            currentBalanceField.setDefaultValue(szBalance);
            currentBalanceField.setLengthLimit(AMOUNT_FIELD_STRING_LEN);
        }

        void show() override {
            CLIView::show();

            nameField.show();
            codeField.show();
            openingBalanceField.show();
            currentBalanceField.show();
        }

        DBAccount getAccount() {
            DBAccount account;

            account.id = accountId;

            account.name = nameField.getValue();
            account.code = codeField.getValue();
            account.openingBalance = openingBalanceField.getDoubleValue();
            account.currentBalance = openingBalanceField.getDoubleValue();

            if (account.code.length() == 0) {
                throw pfm_error("Account code must have a value");
            }

            return account;
        }
};

#endif

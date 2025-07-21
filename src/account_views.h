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
        DateField openingDateField = DateField("Opening date: ");
        CLITextField openingBalanceField = CLITextField("Opening balance [0.00]: ");

    public:
        AddAccountView() : AddAccountView("Add account") {}

        AddAccountView(const char * title) : CLIView(title) {
            nameField.setLengthLimit(FIELD_STRING_LEN);
            codeField.setLengthLimit(CODE_FIELD_MAX_LENGTH);
            openingDateField.setLengthLimit(DATE_FIELD_LENGTH);

            openingDateField.setDefaultValue(StrDate::today());
            openingBalanceField.setDefaultValue("0.00");
            openingBalanceField.setLengthLimit(AMOUNT_FIELD_STRING_LEN);
        }

        void show() override {
            CLIView::show();

            nameField.show();
            codeField.show();
            openingDateField.show();
            openingBalanceField.show();
        }

        DBAccount getAccount() {
            DBAccount account;

            account.name = nameField.getValue();
            account.code = codeField.getValue();
            account.openingDate = openingDateField.getValue();
            account.openingBalance = openingBalanceField.getValue();

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

class AccountListView : public CLIListView {
    public:
        AccountListView() : CLIListView() {}

        void addResults(DBResult<DBAccount> & result) {
            char szTitle[TITLE_BUFFER_LEN];

            snprintf(szTitle, TITLE_BUFFER_LEN, "Accounts (%d)", result.getNumRows());
            setTitle(szTitle);

            CLIListRow headerRow;

            CLIListColumn column1 = CLIListColumn("Code", 5, CLIListColumn::leftAligned);
            headerRow.addColumn(column1);

            CLIListColumn column2 = CLIListColumn("Name", 25, CLIListColumn::leftAligned);
            headerRow.addColumn(column2);

            CLIListColumn column3 = CLIListColumn("Balance", 16, CLIListColumn::rightAligned);
            headerRow.addColumn(column3);

            CLIListColumn column4 = CLIListColumn("Reconciled", 16, CLIListColumn::rightAligned);
            headerRow.addColumn(column4);

            CLIListColumn column5 = CLIListColumn("After Bills", 16, CLIListColumn::rightAligned);
            headerRow.addColumn(column5);

            CLIListColumn column6 = CLIListColumn("Primary", 7, CLIListColumn::leftAligned);
            headerRow.addColumn(column6);

            addHeaderRow(headerRow);

            for (int i = 0;i < result.getNumRows();i++) {
                DBAccount account = result.getResultAt(i);

                CLIListRow row(headerRow);

                Money currentBalance = account.calculateCurrentBalance();
                Money reconciledBalance = account.calculateReconciledBalance();
                Money balanceAfterBills = account.calculateBalanceAfterBills();

                row.addCellValue(account.code);
                row.addCellValue(account.name);
                row.addCellValue(currentBalance);
                row.addCellValue(reconciledBalance);
                row.addCellValue(balanceAfterBills);
                row.addCellValue(account.isPrimary() ? "   *" : "");

                addRow(row);
            }
        }
};

class UpdateAccountView : public CLIView {
    private:
        pfm_id_t accountId;

        CLITextField nameField;
        CLITextField codeField;
        DateField openingDateField;
        CLICurrencyField openingBalanceField;

    public:
        UpdateAccountView() : UpdateAccountView("Update account") {}

        UpdateAccountView(const char * title) : CLIView(title) {}

        void setAccount(DBAccount & account) {
            char szPrompt[MAX_PROMPT_LENGTH];

            accountId = account.id;

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Name ['%s']: ", account.name.c_str());
            nameField.setLabel(szPrompt);
            nameField.setDefaultValue(account.name);
            nameField.setLengthLimit(FIELD_STRING_LEN);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Code ['%s']: ", account.code.c_str());
            codeField.setLabel(szPrompt);
            codeField.setDefaultValue(account.code);
            codeField.setLengthLimit(CODE_FIELD_MAX_LENGTH);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Opening date ['%s']: ", account.openingDate.shortDate().c_str());
            openingDateField.setLabel(szPrompt);
            openingDateField.setDefaultValue(account.openingDate.shortDate());
            openingDateField.setLengthLimit(DATE_FIELD_LENGTH);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Opening balance [%s]: ", account.openingBalance.getRawStringValue().c_str());
            openingBalanceField.setLabel(szPrompt);
            openingBalanceField.setDefaultValue(account.openingBalance.getRawStringValue());
        }

        void show() override {
            CLIView::show();

            nameField.show();
            codeField.show();
            openingDateField.show();
            openingBalanceField.show();
        }

        DBAccount getAccount() {
            DBAccount account;

            account.id = accountId;

            account.name = nameField.getValue();
            account.code = codeField.getValue();
            account.openingDate = openingDateField.getValue();
            account.openingBalance = openingBalanceField.getValue();

            if (account.code.length() == 0) {
                throw pfm_error("Account code must have a value");
            }

            return account;
        }
};

#endif

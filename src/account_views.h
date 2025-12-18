#include <iostream>
#include <string>
#include <string.h>
#include <vector>

#include "pfm_error.h"
#include "cli_widget.h"
#include "custom_widgets.h"
#include "db_account.h"
#include "bold_modifier.h"

using namespace std;

#ifndef __ACCOUNT_VIEW
#define __ACCOUNT_VIEW

class AddAccountView : public CLIView {
    private:
        CLITextField nameField = CLITextField("Name: ");
        CLITextField codeField = CLITextField("Code (max. 5 chars): ");
        DateField openingDateField = DateField("Opening date [today]: ");
        CLITextField openingBalanceField = CLITextField("Opening balance [0.00]: ");
        CLITextField balanceLimitField = CLITextField("Balance limit [0.00]: ");

    public:
        AddAccountView() : AddAccountView("Add account") {}

        AddAccountView(const char * title) : CLIView(title) {
            nameField.setLengthLimit(FIELD_STRING_LEN);
            codeField.setLengthLimit(CODE_FIELD_MAX_LENGTH);

            openingDateField.setLengthLimit(DATE_FIELD_LENGTH);
            openingDateField.setDefaultValue(StrDate::today());

            openingBalanceField.setDefaultValue("0.00");
            openingBalanceField.setLengthLimit(AMOUNT_FIELD_STRING_LEN);
            
            balanceLimitField.setDefaultValue("0.00");
            balanceLimitField.setLengthLimit(AMOUNT_FIELD_STRING_LEN);
        }

        void show() override {
            CLIView::show();

            nameField.show();
            codeField.show();
            openingDateField.show();
            openingBalanceField.show();
            balanceLimitField.show();
        }

        DBAccount getAccount() {
            DBAccount account;

            account.name = nameField.getValue();
            account.code = codeField.getValue();
            account.openingDate = openingDateField.getValue();
            account.openingBalance = openingBalanceField.getValue();
            account.balanceLimit = balanceLimitField.getValue();

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

class AccountDetailsView : public CLIView {
    private:
        DBAccount account;

        void printHorizontalLine() {
            cout << "+--------------------------------------------+---------------+" << endl;
        }

        void printRow(const string & label, const Money & amount) {
            cout 
                << left
                << "| "
                << right 
                << setw(42) 
                << label 
                << left
                << " | "
                << right 
                << bold_on 
                << setw(LIST_VIEW_AMOUNT_WIDTH) 
                << amount.localeFormattedStringValue() 
                << bold_off 
                << left
                << " |"
                << endl;
        }

    public:
        void setAccount(DBAccount & account) {
            this->account = account;
        }

        void show() override {
            cout 
                << endl << endl 
                << bold_on 
                << account.name 
                << bold_off 
                << " [" << account.code << "]" 
                << (account.isPrimary() ? " - primary" : "") 
                << endl
                << endl
                << "Opened on " 
                << account.openingDate.longDate()
                << " with balance: " 
                << account.openingBalance.localeFormattedStringValue() 
                << endl
                << endl;

            account.doBalancePrerequisites();
            
            Money currentBalance = account.calculateCurrentBalance();
            Money reconciledBalance = account.calculateReconciledBalance();
            Money balanceAfterBills = account.calculateBalanceAfterBills();
            Money remainingBalance = account.calculateRemainingBalance(balanceAfterBills);

            printHorizontalLine();

            printRow("limit:", account.balanceLimit);
            printRow("current balance:", currentBalance);
            printRow("reconciled balance:", reconciledBalance);
            printRow("balance after bills:", balanceAfterBills);
            printRow("remaining balance:", remainingBalance);

            printHorizontalLine();

            cout << endl;
        }
};

class AccountListView : public CLIListView {
    public:
        AccountListView() : CLIListView() {}

        void addResults(DBResult<DBAccount> & result) {
            char szTitle[TITLE_BUFFER_LEN];

            snprintf(szTitle, TITLE_BUFFER_LEN, "Accounts (%d)", result.size());
            setTitle(szTitle);

            CLIListRow headerRow;

            CLIListColumn column1 = CLIListColumn("Code", 5, CLIListColumn::leftAligned);
            headerRow.addColumn(column1);

            CLIListColumn column2 = CLIListColumn("Name", 20, CLIListColumn::leftAligned);
            headerRow.addColumn(column2);

            CLIListColumn column3 = CLIListColumn("Balance", LIST_VIEW_AMOUNT_WIDTH, CLIListColumn::rightAligned);
            headerRow.addColumn(column3);

            CLIListColumn column4 = CLIListColumn("Reconciled", LIST_VIEW_AMOUNT_WIDTH, CLIListColumn::rightAligned);
            headerRow.addColumn(column4);

            CLIListColumn column5 = CLIListColumn("After Bills", LIST_VIEW_AMOUNT_WIDTH, CLIListColumn::rightAligned);
            headerRow.addColumn(column5);

            CLIListColumn column6 = CLIListColumn("Remaining", LIST_VIEW_AMOUNT_WIDTH, CLIListColumn::rightAligned);
            headerRow.addColumn(column6);

            CLIListColumn column7 = CLIListColumn("P", 1, CLIListColumn::leftAligned);
            headerRow.addColumn(column7);

            addHeaderRow(headerRow);

            for (int i = 0;i < result.size();i++) {
                DBAccount account = result[i];

                CLIListRow row(headerRow);

                account.doBalancePrerequisites();
                
                Money currentBalance = account.calculateCurrentBalance();
                Money reconciledBalance = account.calculateReconciledBalance();
                Money balanceAfterBills = account.calculateBalanceAfterBills();
                Money remainingBalance = account.calculateRemainingBalance(balanceAfterBills);

                row.addCellValue(account.code);
                row.addCellValue(account.name);
                row.addCellValue(currentBalance);
                row.addCellValue(reconciledBalance);
                row.addCellValue(balanceAfterBills);
                row.addCellValue(remainingBalance);
                row.addCellValue(account.isPrimary() ? "*" : "");

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
        CLICurrencyField balanceLimitField;

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

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Opening balance [%s]: ", account.openingBalance.rawStringValue().c_str());
            openingBalanceField.setLabel(szPrompt);
            openingBalanceField.setDefaultValue(account.openingBalance.rawStringValue());

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Balance limit [%s]: ", account.balanceLimit.rawStringValue().c_str());
            balanceLimitField.setLabel(szPrompt);
            balanceLimitField.setDefaultValue(account.balanceLimit.rawStringValue());
        }

        void show() override {
            CLIView::show();

            nameField.show();
            codeField.show();
            openingDateField.show();
            openingBalanceField.show();
            balanceLimitField.show();
        }

        DBAccount getAccount() {
            DBAccount account;

            account.id = accountId;

            account.name = nameField.getValue();
            account.code = codeField.getValue();
            account.openingDate = openingDateField.getValue();
            account.openingBalance = openingBalanceField.getValue();
            account.balanceLimit = balanceLimitField.getValue();

            if (account.code.length() == 0) {
                throw pfm_error("Account code must have a value");
            }

            return account;
        }
};

#endif

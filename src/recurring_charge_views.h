#include <iostream>
#include <string>
#include <string.h>
#include <vector>

#include "pfm_error.h"
#include "cli_widget.h"
#include "custom_widgets.h"
#include "db_recurring_charge.h"
#include "db_recurring_transfer.h"
#include "db_v_recurring_charge.h"
#include "money.h"
#include "cfgmgr.h"

using namespace std;

#ifndef __RECURRING_CHARGE_VIEW
#define __RECURRING_CHARGE_VIEW

class AddRecurringChargeView : public CLIView {
    private:
        CategorySpinField categoryField = CategorySpinField("Category code (max. 5 chars): ");
        PayeeSpinField payeeField = PayeeSpinField("Payee code (max. 5 chars): ");
        DateField startDateField = DateField("Start date (yyyy-mm-dd)[today]: ");
        DateField endDateField = DateField("End date (yyyy-mm-dd): ");
        CLITextField descriptionField = CLITextField("Description: ");
        FrequencyField frequencyField = FrequencyField("Frequency (N[wmy]): ");
        CLICurrencyField amountField = CLICurrencyField("Amount: ");
        AccountSpinField transferToAccountField = AccountSpinField("Transfer to: ");

    public:
        AddRecurringChargeView() : AddRecurringChargeView("Add recurring charge") {}

        AddRecurringChargeView(const char * title) : CLIView(title) {
            string today = StrDate::today();
            startDateField.setDefaultValue(today);
        }

        void show() override {
            CLIView::show();

            categoryField.show();
            payeeField.show();
            startDateField.show();
            endDateField.show();
            descriptionField.show();
            frequencyField.show();
            amountField.show();
            transferToAccountField.show();
        }

        DBRecurringCharge getRecurringCharge() {
            DBRecurringCharge charge;

            charge.category = categoryField.getCategory();
            charge.categoryId = charge.category.id;

            charge.payee = payeeField.getPayee();
            charge.payeeId = charge.payee.id;

            charge.date = startDateField.getValue();
            charge.endDate = endDateField.getValue();
            charge.description = descriptionField.getValue();
            charge.frequency = Frequency::parse(frequencyField.getValue());
            charge.amount = amountField.getDoubleValue();

            if (transferToAccountField.getValue().length() > 0) {
                charge.transfer = new DBRecurringTransfer();
                charge.transfer->accountTo = transferToAccountField.getAccount();
            }

            return charge;
        }
};

class ChooseRecurringChargeView : public CLIView {
    private:
        CLITextField sequenceField = CLITextField("Sequence: ");

    public:
        ChooseRecurringChargeView() : ChooseRecurringChargeView("Use recurring charge") {}
        ChooseRecurringChargeView(const char * title) : CLIView(title) {}

        void show() override {
            CLIView::show();

            sequenceField.show();
        }

        int getSequence() {
            return sequenceField.getIntegerValue();
        }
};

class RecurringChargeListView : public CLIListView {
    private:
        Money total;

    public:
        RecurringChargeListView() : CLIListView() {
            total = 0.0;
        }

        void addResults(DBResult<DBRecurringChargeView> & result, string & accountCode) {
            char szTitle[TITLE_BUFFER_LEN];

            snprintf(szTitle, TITLE_BUFFER_LEN, "Recurring charges for account: %s (%d)", accountCode.c_str(), result.size());
            setTitle(szTitle);

            CLIListRow headerRow;

            CLIListColumn column1 = CLIListColumn("Seq", 3, CLIListColumn::rightAligned);
            headerRow.addColumn(column1);

            CLIListColumn column2 = CLIListColumn("Start", DATE_FIELD_LENGTH, CLIListColumn::leftAligned);
            headerRow.addColumn(column2);

            CLIListColumn column3 = CLIListColumn("Last Paid", DATE_FIELD_LENGTH, CLIListColumn::leftAligned);
            headerRow.addColumn(column3);

            CLIListColumn column4 = CLIListColumn("Description", 25, CLIListColumn::leftAligned);
            headerRow.addColumn(column4);

            CLIListColumn column5 = CLIListColumn("Ctgry", 5, CLIListColumn::leftAligned);
            headerRow.addColumn(column5);

            CLIListColumn column6 = CLIListColumn("Payee", 5, CLIListColumn::leftAligned);
            headerRow.addColumn(column6);

            CLIListColumn column7 = CLIListColumn("Frq", 4, CLIListColumn::leftAligned);
            headerRow.addColumn(column7);

            CLIListColumn column8 = CLIListColumn("Amount", LIST_VIEW_AMOUNT_WIDTH, CLIListColumn::rightAligned);
            headerRow.addColumn(column8);

            addHeaderRow(headerRow);

            for (int i = 0;i < result.size();i++) {
                DBRecurringChargeView charge = result.at(i);

                CLIListRow row(headerRow);

                row.addCellValue(charge.sequence);
                row.addCellValue(charge.date.shortDate());
                row.addCellValue(charge.lastPaymentDate.shortDate());
                row.addCellValue(charge.description);
                row.addCellValue(charge.categoryCode);
                row.addCellValue(charge.payeeCode);
                row.addCellValue(charge.frequency.toString());
                row.addCellValue(charge.amount);

                total += charge.amount;
                addRow(row);
            }
        }

        void show() override {
            CLIListView::showNoExtraCR();

            showTotal(7, total.localeFormattedStringValue());
        }
};

class UpdateRecurringChargeView : public CLIView {
    private:
        pfm_id_t chargeId;

        CategorySpinField categoryField;
        PayeeSpinField payeeField;
        DateField startDateField;
        DateField endDateField;
        DateField lastPaymentDateField;
        CLITextField descriptionField;
        FrequencyField frequencyField;
        CLICurrencyField amountField;

    public:
        UpdateRecurringChargeView() : UpdateRecurringChargeView("Update recurring charge") {}
        UpdateRecurringChargeView(const char * title) : CLIView(title) {}

        void setRecurringCharge(DBRecurringCharge & charge) {
            char szPrompt[MAX_PROMPT_LENGTH];

            chargeId = charge.id;

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Category ['%s']: ", charge.category.code.c_str());
            categoryField.setLabel(szPrompt);
            categoryField.setDefaultValue(charge.category.code);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Payee ['%s']: ", charge.payee.code.c_str());
            payeeField.setLabel(szPrompt);
            payeeField.setDefaultValue(charge.payee.code);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Start date ['%s']: ", charge.date.shortDate().c_str());
            startDateField.setLabel(szPrompt);
            startDateField.setDefaultValue(charge.date.shortDate());

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "End date ['%s']: ", charge.endDate.shortDate().c_str());
            endDateField.setLabel(szPrompt);
            endDateField.setDefaultValue(charge.endDate.shortDate());

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Last payment date ['%s']: ", charge.lastPaymentDate.shortDate().c_str());
            lastPaymentDateField.setLabel(szPrompt);
            lastPaymentDateField.setDefaultValue(charge.lastPaymentDate.shortDate());

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Description ['%s']: ", charge.description.c_str());
            descriptionField.setLabel(szPrompt);
            descriptionField.setDefaultValue(charge.description);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Frequency (x[d|w|m|y]) ['%s']: ", charge.frequency.c_str());
            frequencyField.setLabel(szPrompt);
            frequencyField.setDefaultValue(charge.frequency.toString());

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Amount [%s]: ", charge.amount.rawStringValue().c_str());
            amountField.setLabel(szPrompt);
            amountField.setDefaultValue(charge.amount.rawStringValue());
        }

        void show() override {
            printTitle();

            categoryField.show();
            payeeField.show();
            startDateField.show();
            endDateField.show();
            lastPaymentDateField.show();
            descriptionField.show();
            frequencyField.show();
            amountField.show();
        }

        DBRecurringCharge getRecurringCharge() {
            DBRecurringCharge charge;

            charge.id = chargeId;

            charge.category = categoryField.getCategory();
            charge.categoryId = charge.category.id;

            charge.payee = payeeField.getPayee();
            charge.payeeId = charge.payee.id;

            charge.date = startDateField.getValue();
            charge.endDate = endDateField.getValue();
            charge.lastPaymentDate = lastPaymentDateField.getValue();
            charge.description = descriptionField.getValue();
            charge.frequency = Frequency::parse(frequencyField.getValue());
            charge.amount = amountField.getValue();

            return charge;
        }
};

#endif

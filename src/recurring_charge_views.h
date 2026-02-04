#include <iostream>
#include <string>
#include <string.h>
#include <vector>

#include "pfm_error.h"
#include "cli_widget.h"
#include "terminal.h"
#include "custom_widgets.h"
#include "db_account.h"
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
                charge.isTransfer = true;
            }

            return charge;
        }

        DBAccount getTransferTo() {
            DBAccount transferTo;

            if (transferToAccountField.getValue().length() > 0) {
                transferTo = transferToAccountField.getAccount();
            }

            return transferTo;
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
        string title;

    public:
        RecurringChargeListView(DBAccount & account) : CLIListView() {
            title = "Recurring charges for account: " + account.code;

            if (Terminal::getWidth() < getMinimumWidth()) {
                throw pfm_error(
                    pfm_error::buildMsg(
                        "Terminal is not wide enough for RecurringChargeListView. Terminal width %u, minimum width %u", 
                        (unsigned int)Terminal::getWidth(), 
                        (unsigned int)getMinimumWidth()));
            }
        }

        RecurringChargeListView(const string & title) : CLIListView() {
            this->title = title;

            if (Terminal::getWidth() < getMinimumWidth()) {
                throw pfm_error(
                    pfm_error::buildMsg(
                        "Terminal is not wide enough for RecurringChargeListView. Terminal width %u, minimum width %u", 
                        (unsigned int)Terminal::getWidth(), 
                        (unsigned int)getMinimumWidth()));
            }
        }

        inline uint16_t getMinimumWidth() override {
            return (
                3 + 
                DATE_FIELD_LENGTH + 
                DATE_FIELD_LENGTH + 
                25 + 
                5 + 
                5 + 
                4 + 
                LIST_VIEW_AMOUNT_WIDTH + 
                2);
        }

        void addResults(DBResult<DBRecurringChargeView> & result) {
            reserveRows(result.size());
            
            setColumns({
                CLIListColumn("Sq", LIST_VIEW_SEQUENCE_WIDTH, CLIListColumn::rightAligned),
                CLIListColumn("Last Paid", DATE_FIELD_LENGTH, CLIListColumn::leftAligned),
                CLIListColumn("Description", LIST_VIEW_DESCRIPTION_WIDTH, CLIListColumn::leftAligned),
                CLIListColumn("Ctgry", LIST_VIEW_CODE_WIDTH, CLIListColumn::leftAligned),
                CLIListColumn("Frq", LIST_VIEW_FREQUENCY_WIDTH, CLIListColumn::leftAligned),
                CLIListColumn("Amount", LIST_VIEW_AMOUNT_WIDTH, CLIListColumn::rightAligned),
                CLIListColumn("T", LIST_VIEW_TRANSFER_WIDTH, CLIListColumn::leftAligned)
            });

            title += " (" + to_string(result.size()) + ')' + '[' + to_string(getTotalWidth()) + ']';

            setTitle(title);

            total = 0.0;

            for (size_t i = 0;i < result.size();i++) {
                DBRecurringChargeView charge = result.at(i);

                CLIListRow row(getNumColumns());

                row.addCell(charge.sequence);
                row.addCell(charge.lastPaymentDate);
                row.addCell(charge.description);
                row.addCell(charge.categoryCode);
                row.addCell(charge.frequency.toString());
                row.addCell(charge.amount);
                row.addCell(charge.getIsTransferValue());

                total += charge.amount;
                addRow(row);
            }
        }

        void show() override {
            CLIListView::showNoExtraCR();

            showTotal("Total amount: ", total);
        }
};

class MigrateChargeView : public CLIView {
    private:
        string sourceAccountCode;
        DBRecurringCharge charge;

        AccountSpinField accountField = AccountSpinField("Account to transfer to (max. 5 chars): ");

    public:
        MigrateChargeView() : MigrateChargeView("Migrate charge") {}

        MigrateChargeView(const char * title) : CLIView(title) {
        }

        void show() override {
            CLIView::show();

            accountField.show();
        }

        void setCharge(DBRecurringCharge & charge) {
            this->charge = charge;
        }

        DBRecurringCharge getCharge() {
            return charge;
        }

        DBRecurringTransfer getTransfer() {
            DBAccount accountTo = accountField.getAccount();

            DBRecurringTransfer transfer;
            transfer.accountToId = accountTo.id;
            transfer.recurringChargeId = charge.id;

            return transfer;
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

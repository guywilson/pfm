#include <iostream>
#include <string>
#include <string.h>
#include <vector>

#include "pfm_error.h"
#include "cli_widget.h"
#include "custom_widgets.h"
#include "db_recurring_charge.h"

using namespace std;

#ifndef __RECURRING_CHARGE_VIEW
#define __RECURRING_CHARGE_VIEW

class AddRecurringChargeView : public CLIView {
    private:
        CategorySpinField categoryField = CategorySpinField("Category code (max. 5 chars): ");
        PayeeSpinField payeeField = PayeeSpinField("Category code (max. 5 chars): ");
        DateField dateField = DateField("Start date (yyyy-mm-dd)[today]: ");
        CLITextField descriptionField = CLITextField("Description: ");
        FrequencyField frequencyField = FrequencyField("Frequency (N[wmy]): ");
        CLITextField amountField = CLITextField("Amount: ");

    public:
        AddRecurringChargeView() : AddRecurringChargeView("Add recurring charge") {}

        AddRecurringChargeView(const char * title) : CLIView(title) {
            categoryField.setLengthLimit(CODE_FIELD_MAX_LENGTH);
            payeeField.setLengthLimit(CODE_FIELD_MAX_LENGTH);
            dateField.setLengthLimit(10);
            descriptionField.setLengthLimit(FIELD_STRING_LEN);
            amountField.setLengthLimit(AMOUNT_FIELD_STRING_LEN);

            string today = StrDate::today();
            dateField.setDefaultValue(today);
        }

        void show() override {
            CLIView::show();

            categoryField.show();
            payeeField.show();
            dateField.show();
            descriptionField.show();
            frequencyField.show();
            amountField.show();
        }

        DBRecurringCharge getRecurringCharge() {
            DBRecurringCharge charge;

            charge.category = categoryField.getCategory();
            charge.categoryId = charge.category.id;

            charge.payee = payeeField.getPayee();
            charge.payeeId = charge.payee.id;

            charge.date = dateField.getValue();
            charge.description = descriptionField.getValue();
            charge.frequency = frequencyField.getValue();
            charge.amount = amountField.getDoubleValue();

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

class UpdateRecurringChargeView : public CLIView {
    private:
        sqlite3_int64 chargeId;

        CategorySpinField categoryField;
        PayeeSpinField payeeField;
        DateField dateField;
        CLITextField descriptionField;
        FrequencyField frequencyField;
        CLITextField amountField;

    public:
        UpdateRecurringChargeView() : UpdateRecurringChargeView("Update recurring charge") {}
        UpdateRecurringChargeView(const char * title) : CLIView(title) {}

        void setRecurringCharge(DBRecurringCharge & charge) {
            char szPrompt[MAX_PROMPT_LENGTH];
            char szBalance[AMOUNT_FIELD_STRING_LEN];

            chargeId = charge.id;

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Category ['%s']: ", charge.category.code.c_str());
            categoryField.setLabel(szPrompt);
            categoryField.setDefaultValue(charge.category.code);
            categoryField.setLengthLimit(CODE_FIELD_MAX_LENGTH);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Payee ['%s']: ", charge.payee.code.c_str());
            payeeField.setLabel(szPrompt);
            payeeField.setDefaultValue(charge.payee.code);
            payeeField.setLengthLimit(CODE_FIELD_MAX_LENGTH);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Date ['%s']: ", charge.date.c_str());
            dateField.setLabel(szPrompt);
            dateField.setDefaultValue(charge.date);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Description ['%s']: ", charge.description.c_str());
            descriptionField.setLabel(szPrompt);
            descriptionField.setDefaultValue(charge.description);
            descriptionField.setLengthLimit(FIELD_STRING_LEN);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Frequency (x[d|w|m|y]) ['%s']: ", charge.frequency.c_str());
            frequencyField.setLabel(szPrompt);
            frequencyField.setDefaultValue(charge.frequency);
            frequencyField.setLengthLimit(FIELD_STRING_LEN);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Amount [%.2f]: ", charge.amount);
            snprintf(szBalance, AMOUNT_FIELD_STRING_LEN, "%.2f", charge.amount);
            amountField.setLabel(szPrompt);
            amountField.setDefaultValue(szBalance);
            amountField.setLengthLimit(AMOUNT_FIELD_STRING_LEN);
        }

        void show() override {
            printTitle();

            categoryField.show();
            payeeField.show();
            dateField.show();
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

            charge.date = dateField.getValue();
            charge.description = descriptionField.getValue();
            charge.frequency = frequencyField.getValue();
            charge.amount = amountField.getDoubleValue();

            return charge;
        }
};

#endif

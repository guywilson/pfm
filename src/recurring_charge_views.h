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
        CLICurrencyField amountField = CLICurrencyField("Amount: ");

    public:
        AddRecurringChargeView() : AddRecurringChargeView("Add recurring charge") {}

        AddRecurringChargeView(const char * title) : CLIView(title) {
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

class RecurringChargeListView : public CLIListView {
    private:
        double total;

    public:
        RecurringChargeListView() : CLIListView() {
            total = 0.0;
        }

        void addResults(DBRecurringChargeResult & result, string & accountCode) {
            char szTitle[TITLE_BUFFER_LEN];

            snprintf(szTitle, TITLE_BUFFER_LEN, "Recurring charges for account: %s (%d)", accountCode.c_str(), result.getNumRows());
            setTitle(szTitle);

            CLIListRow headerRow;

            CLIListColumn column1 = CLIListColumn("Seq", 3, CLIListColumn::rightAligned);
            headerRow.addColumn(column1);

            CLIListColumn column2 = CLIListColumn("Start Dt", DATE_FIELD_LENGTH, CLIListColumn::leftAligned);
            headerRow.addColumn(column2);

            CLIListColumn column3 = CLIListColumn("Nxt Pmnt", DATE_FIELD_LENGTH, CLIListColumn::leftAligned);
            headerRow.addColumn(column3);

            CLIListColumn column4 = CLIListColumn("Description", 25, CLIListColumn::leftAligned);
            headerRow.addColumn(column4);

            CLIListColumn column5 = CLIListColumn("Ctgry", 5, CLIListColumn::leftAligned);
            headerRow.addColumn(column5);

            CLIListColumn column6 = CLIListColumn("Payee", 5, CLIListColumn::leftAligned);
            headerRow.addColumn(column6);

            CLIListColumn column7 = CLIListColumn("Frq", 4, CLIListColumn::leftAligned);
            headerRow.addColumn(column7);

            CLIListColumn column8 = CLIListColumn("Amount", 13, CLIListColumn::rightAligned);
            headerRow.addColumn(column8);

            addHeaderRow(headerRow);

            for (int i = 0;i < result.getNumRows();i++) {
                DBRecurringCharge charge = result.getResultAt(i);

                CLIListRow row(headerRow);

                row.addCellValue(charge.sequence);
                row.addCellValue(charge.date);
                row.addCellValue(charge.nextPaymentDate);
                row.addCellValue(charge.description);
                row.addCellValue(charge.category.code);
                row.addCellValue(charge.payee.code);
                row.addCellValue(charge.frequency);
                row.addCellValue(formatCurrency(charge.amount));

                total += charge.amount;
                addRow(row);
            }
        }

        void show() override {
            CLIListView::show();
            showBottomBorder();
            cout << "                                                                    Total charges: | " << bold_on << right << setw(13) << formatCurrency(total) << bold_off << " |" << endl << endl;
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

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Date ['%s']: ", charge.date.c_str());
            dateField.setLabel(szPrompt);
            dateField.setDefaultValue(charge.date);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Description ['%s']: ", charge.description.c_str());
            descriptionField.setLabel(szPrompt);
            descriptionField.setDefaultValue(charge.description);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Frequency (x[d|w|m|y]) ['%s']: ", charge.frequency.c_str());
            frequencyField.setLabel(szPrompt);
            frequencyField.setDefaultValue(charge.frequency);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Amount [%.2f]: ", charge.amount);
            amountField.setLabel(szPrompt);
            amountField.setDefaultValue(charge.amount);
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

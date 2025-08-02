#include <iostream>
#include <string>
#include <string.h>
#include <vector>

#include "pfm_error.h"
#include "cli_widget.h"
#include "custom_widgets.h"
#include "db_budget.h"

using namespace std;

#ifndef __INCL_BUDGET_VIEWS
#define __INCL_BUDGET_VIEWS

class AddBudgetView : public CLIView {
    private:
        DateField startDateField = DateField("Start date: ");
        DateField endDateField = DateField("End date: ");
        CategorySpinField categoryCodeField = CategorySpinField("Category code: ");
        PayeeSpinField payeeCodeField = PayeeSpinField("Payee code: ");
        CLITextField descriptionField = CLITextField("Description: ");
        CLICurrencyField minBudgetField = CLICurrencyField("Minimum budget: ");
        CLICurrencyField maxBudgetField = CLICurrencyField("Maximum budget: ");

    public:
        AddBudgetView() : AddBudgetView("Add budget") {}

        AddBudgetView(const char * title) : CLIView(title) {
            descriptionField.setLengthLimit(FIELD_STRING_LEN);

            categoryCodeField.setLengthLimit(CODE_FIELD_MAX_LENGTH);
            payeeCodeField.setLengthLimit(CODE_FIELD_MAX_LENGTH);

            minBudgetField.setDefaultValue("0.00");
            minBudgetField.setLengthLimit(AMOUNT_FIELD_STRING_LEN);

            maxBudgetField.setDefaultValue("0.00");
            maxBudgetField.setLengthLimit(AMOUNT_FIELD_STRING_LEN);
        }

        void show() override {
            CLIView::show();

            startDateField.show();
            endDateField.show();
            categoryCodeField.show();
            payeeCodeField.show();
            descriptionField.show();
            minBudgetField.show();
            maxBudgetField.show();
        }

        DBBudget getBudget() {
            DBBudget budget;

            budget.startDate = startDateField.getValue();
            budget.endDate = endDateField.getValue();
            budget.categoryCode = categoryCodeField.getValue();
            budget.payeeCode = payeeCodeField.getValue();
            budget.description = descriptionField.getValue();
            budget.minimumBudget = minBudgetField.getDoubleValue();
            budget.maximumBudget = maxBudgetField.getDoubleValue();

            return budget;
        }
};

class ChooseBudgetView : public CLIView {
    private:
        CLITextField sequenceField = CLITextField("Sequence: ");

    public:
        ChooseBudgetView() : ChooseBudgetView("Use transaction") {}
        ChooseBudgetView(const char * title) : CLIView(title) {}

        void show() override {
            CLIView::show();

            sequenceField.show();
        }

        int getSequence() {
            return sequenceField.getIntegerValue();
        }
};

class BudgetListView : public CLIListView {
    public:
        BudgetListView() : CLIListView() {}

        void addResults(DBResult<DBBudget> & result) {
            char szTitle[TITLE_BUFFER_LEN];

            snprintf(szTitle, TITLE_BUFFER_LEN, "Budgets (%d)", result.size());
            setTitle(szTitle);

            CLIListRow headerRow;

            CLIListColumn column1 = CLIListColumn("Seq", 3, CLIListColumn::rightAligned);
            headerRow.addColumn(column1);

            CLIListColumn column2 = CLIListColumn("Start dt", DATE_FIELD_LENGTH, CLIListColumn::leftAligned);
            headerRow.addColumn(column2);

            CLIListColumn column3 = CLIListColumn("End dt", DATE_FIELD_LENGTH, CLIListColumn::leftAligned);
            headerRow.addColumn(column3);

            CLIListColumn column4 = CLIListColumn("Ctgry", 5, CLIListColumn::leftAligned);
            headerRow.addColumn(column4);

            CLIListColumn column5 = CLIListColumn("Payee", 5, CLIListColumn::leftAligned);
            headerRow.addColumn(column5);

            CLIListColumn column6 = CLIListColumn("Description", 25, CLIListColumn::leftAligned);
            headerRow.addColumn(column6);

            CLIListColumn column7 = CLIListColumn("Minimum budget", 16, CLIListColumn::rightAligned);
            headerRow.addColumn(column7);

            CLIListColumn column8 = CLIListColumn("Maximum budget", 16, CLIListColumn::rightAligned);
            headerRow.addColumn(column8);

            addHeaderRow(headerRow);

            for (int i = 0;i < result.size();i++) {
                DBBudget budget = result.at(i);

                CLIListRow row(headerRow);

                row.addCellValue(budget.sequence);
                row.addCellValue(budget.startDate.shortDate());
                row.addCellValue(budget.endDate.getDisplayDate());
                row.addCellValue(budget.categoryCode);
                row.addCellValue(budget.payeeCode);
                row.addCellValue(budget.description);
                row.addCellValue(budget.minimumBudget);
                row.addCellValue(budget.maximumBudget);

                addRow(row);
            }
        }
};

class UpdateBudgetView : public CLIView {
    private:
        pfm_id_t budgetId;

        DateField startDateField = DateField("Start date: ");
        DateField endDateField = DateField("End date: ");
        CategorySpinField categoryCodeField = CategorySpinField("Category code: ");
        PayeeSpinField payeeCodeField = PayeeSpinField("Payee code: ");
        CLITextField descriptionField = CLITextField("Description: ");
        CLICurrencyField minBudgetField = CLICurrencyField("Minimum budget: ");
        CLICurrencyField maxBudgetField = CLICurrencyField("Maximum budget: ");

    public:
        UpdateBudgetView() : UpdateBudgetView("Update budget") {}

        UpdateBudgetView(const char * title) : CLIView(title) {}

        void setBudget(DBBudget & budget) {
            char szPrompt[MAX_PROMPT_LENGTH];

            budgetId = budget.id;

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Start date ['%s']: ", budget.startDate.shortDate().c_str());
            startDateField.setLabel(szPrompt);
            startDateField.setDefaultValue(budget.startDate.shortDate());

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "End date ['%s']: ", budget.endDate.getDisplayDate().c_str());
            endDateField.setLabel(szPrompt);
            endDateField.setDefaultValue(budget.endDate.getDisplayDate());

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Category ['%s']: ", budget.categoryCode.c_str());
            categoryCodeField.setLabel(szPrompt);
            categoryCodeField.setDefaultValue(budget.categoryCode);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Payee ['%s']: ", budget.payeeCode.c_str());
            payeeCodeField.setLabel(szPrompt);
            payeeCodeField.setDefaultValue(budget.payeeCode);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Description ['%s']: ", budget.description.c_str());
            descriptionField.setLabel(szPrompt);
            descriptionField.setDefaultValue(budget.description);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Minimum budget [%s]: ", budget.minimumBudget.getRawStringValue().c_str());
            minBudgetField.setLabel(szPrompt);
            minBudgetField.setDefaultValue(budget.minimumBudget.getRawStringValue());

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Maximum budget [%s]: ", budget.maximumBudget.getRawStringValue().c_str());
            maxBudgetField.setLabel(szPrompt);
            maxBudgetField.setDefaultValue(budget.maximumBudget.getRawStringValue());
        }

        void show() override {
            CLIView::show();

            startDateField.show();
            endDateField.show();
            categoryCodeField.show();
            payeeCodeField.show();
            descriptionField.show();
            minBudgetField.show();
            maxBudgetField.show();
        }

        DBBudget getBudget() {
            DBBudget budget;

            budget.id = budgetId;

            budget.startDate = startDateField.getValue();

            if (endDateField.getValue() != "N/A") {
                budget.endDate = endDateField.getValue();
            }
            
            budget.categoryCode = categoryCodeField.getValue();
            budget.payeeCode = payeeCodeField.getValue();
            budget.description = descriptionField.getValue();
            budget.minimumBudget = minBudgetField.getDoubleValue();
            budget.maximumBudget = maxBudgetField.getDoubleValue();

            return budget;
        }
};

#endif

#include <iostream>
#include <string>
#include <string.h>
#include <vector>

#include "pfm_error.h"
#include "cli_widget.h"
#include "custom_widgets.h"
#include "db_payee.h"

using namespace std;

#ifndef __PAYEE_VIEW
#define __PAYEE_VIEW

class AddPayeeView : public CLIView {
    private:
        CLITextField nameField = CLITextField("Name: ");
        CLITextField codeField = CLITextField("Code (max. 5 chars): ");

    public:
        AddPayeeView() : AddPayeeView("Add payee") {}

        AddPayeeView(const char * title) : CLIView(title) {
            nameField.setLengthLimit(FIELD_STRING_LEN);
            codeField.setLengthLimit(CODE_FIELD_MAX_LENGTH);
        }

        void show() override {
            CLIView::show();

            nameField.show();
            codeField.show();
        }

        DBPayee getPayee() {
            DBPayee payee;

            payee.name = nameField.getValue();
            payee.code = codeField.getValue();

            return payee;
        }
};

class ChoosePayeeView : public CLIView {
    private:
        CLITextField codeField = CLITextField("Code (max. 5 chars): ");

    public:
        ChoosePayeeView() : ChoosePayeeView("Use payee") {}
        
        ChoosePayeeView(const char * title) : CLIView(title) {
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

class PayeeListView : public CLIListView {
    public:
        PayeeListView() : CLIListView() {}

        void addResults(DBResult<DBPayee> & result) {
            char szTitle[TITLE_BUFFER_LEN];

            snprintf(szTitle, TITLE_BUFFER_LEN, "Payees (%d)", result.size());
            setTitle(szTitle);

            CLIListRow headerRow;

            CLIListColumn column1 = CLIListColumn("Code", 5, CLIListColumn::leftAligned);
            headerRow.addColumn(column1);

            CLIListColumn column2 = CLIListColumn("Name", 55, CLIListColumn::leftAligned);
            headerRow.addColumn(column2);

            addHeaderRow(headerRow);

            for (int i = 0;i < result.size();i++) {
                DBPayee payee = result.at(i);

                CLIListRow row(headerRow);

                row.addCellValue(payee.code);
                row.addCellValue(payee.name);

                addRow(row);
            }
        }
};

class UpdatePayeeView : public CLIView {
    private:
        pfm_id_t categoryId;

        CLITextField nameField;
        CLITextField codeField;

    public:
        UpdatePayeeView() : UpdatePayeeView("Update payee") {}

        UpdatePayeeView(const char * title) : CLIView(title) {}

        void setPayee(DBPayee & payee) {
            char szPrompt[MAX_PROMPT_LENGTH];

            categoryId = payee.id;

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Name ['%s']: ", payee.name.c_str());
            nameField.setLabel(szPrompt);
            nameField.setDefaultValue(payee.name);
            nameField.setLengthLimit(FIELD_STRING_LEN);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Code ['%s']: ", payee.code.c_str());
            codeField.setLabel(szPrompt);
            codeField.setDefaultValue(payee.code);
            codeField.setLengthLimit(CODE_FIELD_MAX_LENGTH);
        }

        void show() override {
            CLIView::show();

            nameField.show();
            codeField.show();
        }

        DBPayee getPayee() {
            DBPayee payee;

            payee.id = categoryId;

            payee.name = nameField.getValue();
            payee.code = codeField.getValue();

            if (payee.code.length() == 0) {
                throw pfm_error("Payee code must have a value");
            }

            return payee;
        }
};

#endif

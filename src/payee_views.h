/*
 * Copyright (C) 2025-2026 Guy Wilson
 *
 * This file is part of PFM.
 *
 * PFM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PFM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PFM. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <iostream>
#include <string>
#include <string.h>
#include <vector>

#include "pfm_error.h"
#include "cli_widget.h"
#include "terminal.h"
#include "custom_widgets.h"
#include "db_payee.h"


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

        std::string getCode() {
            return codeField.getValue();
        }
};

class PayeeListView : public CLIListView {
    public:
        PayeeListView() : CLIListView() {
            if (Terminal::getWidth() < getMinimumWidth()) {
                throw pfm_error(
                    pfm_error::buildMsg(
                        "Terminal is not wide enough for PayeeListView. Terminal width %u, minimum width %u", 
                        (unsigned int)Terminal::getWidth(), 
                        (unsigned int)getMinimumWidth()));
            }
        }

        inline uint16_t getMinimumWidth() override {
            return (
                5 + 
                55);
        }

        void addResults(DBResult<DBPayee> & result) {
            char szTitle[TITLE_BUFFER_LEN];

            snprintf(szTitle, TITLE_BUFFER_LEN, "Payees (%zu)", result.size());
            setTitle(szTitle);

            reserveRows(result.size());
            
            setColumns({
                CLIListColumn("Code", LIST_VIEW_CODE_WIDTH, CLIListColumn::leftAligned),
                CLIListColumn("Name", 30, CLIListColumn::leftAligned)
            });

            for (size_t i = 0;i < result.size();i++) {
                DBPayee payee = result.at(i);

                CLIListRow row(getNumColumns());

                row.addCell(payee.code);
                row.addCell(payee.name);

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

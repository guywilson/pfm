#include <iostream>
#include <string>
#include <string.h>
#include <vector>

#include "pfm_error.h"
#include "cli_widget.h"
#include "terminal.h"
#include "custom_widgets.h"
#include "db_category.h"

using namespace std;

#ifndef __CATEGORY_VIEW
#define __CATEGORY_VIEW

class AddCategoryView : public CLIView {
    private:
        CLITextField descriptionField = CLITextField("Description: ");
        CLITextField codeField = CLITextField("Code (max. 5 chars): ");

    public:
        AddCategoryView() : AddCategoryView("Add category") {}

        AddCategoryView(const char * title) : CLIView(title) {
            descriptionField.setLengthLimit(FIELD_STRING_LEN);
            codeField.setLengthLimit(CODE_FIELD_MAX_LENGTH);
        }

        void show() override {
            CLIView::show();

            descriptionField.show();
            codeField.show();
        }

        DBCategory getCategory() {
            DBCategory category;

            category.description = descriptionField.getValue();
            category.code = codeField.getValue();

            return category;
        }
};

class ChooseCategoryView : public CLIView {
    private:
        CLITextField codeField = CLITextField("Code (max. 5 chars): ");

    public:
        ChooseCategoryView() : ChooseCategoryView("Use category") {}
        
        ChooseCategoryView(const char * title) : CLIView(title) {
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

class CategoryListView : public CLIListView {
    public:
        CategoryListView() : CLIListView() {
            if (Terminal::getWidth() < getMinimumWidth()) {
                throw pfm_error(
                    pfm_error::buildMsg(
                        "Terminal is not wide enough for CategoryListView. Terminal width %u, minimum width %u", 
                        (unsigned int)Terminal::getWidth(), 
                        (unsigned int)getMinimumWidth()));
            }
        }

        inline uint16_t getMinimumWidth() override {
            return (
                5 + 
                55);
        }

        void addResults(DBResult<DBCategory> & result) {
            char szTitle[TITLE_BUFFER_LEN];

            snprintf(szTitle, TITLE_BUFFER_LEN, "Categories (%d)", result.size());
            setTitle(szTitle);

            setColumns({
                CLIListColumn("Code", 5, CLIListColumn::leftAligned),
                CLIListColumn("Description", 55, CLIListColumn::leftAligned)
            });

            for (int i = 0;i < result.size();i++) {
                DBCategory category = result.at(i);

                CLIListRow row(getNumColumns());

                row.addCell(category.code);
                row.addCell(category.description);

                addRow(row);
            }
        }
};

class UpdateCategoryView : public CLIView {
    private:
        pfm_id_t categoryId;

        CLITextField descriptionField;
        CLITextField codeField;

    public:
        UpdateCategoryView() : UpdateCategoryView("Update category") {}

        UpdateCategoryView(const char * title) : CLIView(title) {}

        void setCategory(DBCategory & category) {
            char szPrompt[MAX_PROMPT_LENGTH];

            categoryId = category.id;

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Description ['%s']: ", category.description.c_str());
            descriptionField.setLabel(szPrompt);
            descriptionField.setDefaultValue(category.description);
            descriptionField.setLengthLimit(FIELD_STRING_LEN);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Code ['%s']: ", category.code.c_str());
            codeField.setLabel(szPrompt);
            codeField.setDefaultValue(category.code);
            codeField.setLengthLimit(CODE_FIELD_MAX_LENGTH);
        }

        void show() override {
            CLIView::show();

            descriptionField.show();
            codeField.show();
        }

        DBCategory getCategory() {
            DBCategory category;

            category.id = categoryId;

            category.description = descriptionField.getValue();
            category.code = codeField.getValue();

            if (category.code.length() == 0) {
                throw pfm_error("Category code must have a value");
            }

            return category;
        }
};

#endif

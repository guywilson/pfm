#include <iostream>
#include <string>
#include <string.h>
#include <vector>

#include "pfm_error.h"
#include "cli_widget.h"
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
        CategoryListView() : CLIListView() {}

        void addResults(DBResult<DBCategory> & result) {
            char szTitle[TITLE_BUFFER_LEN];

            snprintf(szTitle, TITLE_BUFFER_LEN, "Categories (%d)", result.size());
            setTitle(szTitle);

            CLIListRow headerRow;

            headerRow.addColumn(CLIListColumn("Code", 5, CLIListColumn::leftAligned));
            headerRow.addColumn(CLIListColumn("Description", 55, CLIListColumn::leftAligned));

            addHeaderRow(headerRow);

            for (int i = 0;i < result.size();i++) {
                DBCategory category = result.at(i);

                CLIListRow row(headerRow);

                row.addCellValue(category.code);
                row.addCellValue(category.description);

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

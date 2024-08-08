#include <string>

#include "cli_widget.h"
#include "db_category.h"
#include "db_payee.h"

using namespace std;

#ifndef __INCL_CUSTOM_WIDGETS
#define __INCL_CUSTOM_WIDGETS

#define FIELD_STRING_LEN                        64
#define MAX_PROMPT_LENGTH                      128
#define AMOUNT_FIELD_STRING_LEN                 16
#define CODE_FIELD_MAX_LENGTH                    5

class CategorySpinField : public CLISpinTextField {
    public:
        CategorySpinField(string & label) : CLISpinTextField(label) {}
        CategorySpinField(const char * label) : CLISpinTextField(label) {}

        void show() override {
            DBCategory category;
            DBCategoryResult result = category.retrieveAll();

            for (int i = 0;i < result.getNumRows();i++) {
                DBCategory c = result.getResultAt(i);
                addItem(c.code);
            }

            CLISpinTextField::show();
        }
};

class PayeeSpinField : public CLISpinTextField {
    public:
        PayeeSpinField(string & label) : CLISpinTextField(label) {}
        PayeeSpinField(const char * label) : CLISpinTextField(label) {}

        void show() override {
            DBPayee payee;
            DBPayeeResult result = payee.retrieveAll();

            for (int i = 0;i < result.getNumRows();i++) {
                DBPayee p = result.getResultAt(i);
                addItem(p.code);
            }

            CLISpinTextField::show();
        }
};

#endif

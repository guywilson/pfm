#include <iostream>
#include <string>
#include <string.h>
#include <vector>

#include "pfm_error.h"
#include "utils.h"
#include "cli_widget.h"
#include "custom_widgets.h"
#include "db_transaction.h"

using namespace std;

#ifndef __TRANSACTION_VIEW
#define __TRANSACTION_VIEW

class AddTransactionView : public CLIView {
    private:
        CategorySpinField categoryField = CategorySpinField("Category code (max. 5 chars): ");
        PayeeSpinField payeeField = PayeeSpinField("Category code (max. 5 chars): ");
        DateField dateField = DateField("Date (yyyy-mm-dd)[today]: ");
        CLITextField descriptionField = CLITextField("Description: ");
        CLITextField creditDebitField = CLITextField("Credit/Debit [DB]: ");
        CLITextField amountField = CLITextField("Amount: ");
        CLITextField isReconciledField = CLITextField("Is reconciled [N]: ");

    public:
        AddTransactionView() : AddTransactionView("Add transaction") {}

        AddTransactionView(const char * title) : CLIView(title) {
            categoryField.setLengthLimit(CODE_FIELD_MAX_LENGTH);
            payeeField.setLengthLimit(CODE_FIELD_MAX_LENGTH);
            dateField.setLengthLimit(10);
            descriptionField.setLengthLimit(FIELD_STRING_LEN);

            creditDebitField.setLengthLimit(2);
            creditDebitField.setDefaultValue("DB");

            amountField.setLengthLimit(AMOUNT_FIELD_STRING_LEN);

            isReconciledField.setLengthLimit(1);
            isReconciledField.setDefaultValue("N");

            string today = StrDate::today();
            dateField.setDefaultValue(today);
        }

        void show() override {
            CLIView::show();

            categoryField.show();
            payeeField.show();
            dateField.show();
            descriptionField.show();
            creditDebitField.show();
            amountField.show();
            isReconciledField.show();
        }

        DBTransaction getTransaction() {
            DBTransaction transaction;

            transaction.category = categoryField.getCategory();
            transaction.categoryId = transaction.category.id;

            transaction.payee = payeeField.getPayee();
            transaction.payeeId = transaction.payee.id;

            transaction.date = dateField.getValue();
            transaction.description = descriptionField.getValue();

            string creditDebit = creditDebitField.getValue();
            transaction.isCredit = transaction.getIsCredit(creditDebit);
            transaction.amount = amountField.getDoubleValue();
            transaction.isReconciled = strtobool(isReconciledField.getValue().c_str());

            return transaction;
        }
};

class ChooseTransactionView : public CLIView {
    private:
        CLITextField sequenceField = CLITextField("Sequence: ");

    public:
        ChooseTransactionView() : ChooseTransactionView("Use transaction") {}
        ChooseTransactionView(const char * title) : CLIView(title) {}

        void show() override {
            CLIView::show();

            sequenceField.show();
        }

        int getSequence() {
            return sequenceField.getIntegerValue();
        }
};

class UpdateTransactionView : public CLIView {
    private:
        sqlite3_int64 transactionId;

        CategorySpinField categoryField;
        PayeeSpinField payeeField;
        DateField dateField;
        CLITextField descriptionField;
        CLITextField creditDebitField;
        CLITextField amountField;
        CLITextField isReconciledField;

    public:
        UpdateTransactionView() : UpdateTransactionView("Update transaction") {}
        UpdateTransactionView(const char * title) : CLIView(title) {}

        void setTransaction(DBTransaction & transaction) {
            char szPrompt[MAX_PROMPT_LENGTH];
            char szBalance[AMOUNT_FIELD_STRING_LEN];

            transactionId = transaction.id;

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Category ['%s']: ", transaction.category.code.c_str());
            categoryField.setLabel(szPrompt);
            categoryField.setDefaultValue(transaction.category.code);
            categoryField.setLengthLimit(CODE_FIELD_MAX_LENGTH);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Payee ['%s']: ", transaction.payee.code.c_str());
            payeeField.setLabel(szPrompt);
            payeeField.setDefaultValue(transaction.payee.code);
            payeeField.setLengthLimit(CODE_FIELD_MAX_LENGTH);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Date ['%s']: ", transaction.date.c_str());
            dateField.setLabel(szPrompt);
            dateField.setDefaultValue(transaction.date);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Description ['%s']: ", transaction.description.c_str());
            payeeField.setLabel(szPrompt);
            payeeField.setDefaultValue(transaction.description);
            payeeField.setLengthLimit(FIELD_STRING_LEN);

            string creditDebit = transaction.getCreditDebitValue();
            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Credit/debit ['%s']: ", creditDebit.c_str());
            creditDebitField.setLabel(szPrompt);
            creditDebitField.setDefaultValue(creditDebit);
            creditDebitField.setLengthLimit(2);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Amount [%.2f]: ", transaction.amount);
            snprintf(szBalance, AMOUNT_FIELD_STRING_LEN, "%.2f", transaction.amount);
            amountField.setLabel(szPrompt);
            amountField.setDefaultValue(szBalance);
            amountField.setLengthLimit(AMOUNT_FIELD_STRING_LEN);
        }

        void show() override {
            CLIView::show();

            categoryField.show();
            payeeField.show();
            dateField.show();
            descriptionField.show();
            creditDebitField.show();
            amountField.show();
        }

        DBTransaction getTransaction() {
            DBTransaction transaction;

            transaction.id = transactionId;

            transaction.category = categoryField.getCategory();
            transaction.categoryId = transaction.category.id;

            transaction.payee = payeeField.getPayee();
            transaction.payeeId = transaction.payee.id;

            transaction.date = dateField.getValue();
            transaction.description = descriptionField.getValue();

            string creditDebit = creditDebitField.getValue();
            transaction.isCredit = decodeCreditDebit(creditDebit.c_str());
            transaction.amount = amountField.getDoubleValue();

            return transaction;
        }
};

#endif

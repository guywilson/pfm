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
        CLICurrencyField amountField = CLICurrencyField("Amount: ");
        CLITextField isReconciledField = CLITextField("Is reconciled [N]: ");

    public:
        AddTransactionView() : AddTransactionView("Add transaction") {}

        AddTransactionView(const char * title) : CLIView(title) {
            creditDebitField.setLengthLimit(2);
            creditDebitField.setDefaultValue("DB");

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

class TransactionListView : public CLIListView {
    private:
        double total;

    public:
        TransactionListView() : CLIListView() {
            total = 0.0;
        }

        void addResults(DBTransactionResult & result, string & accountCode) {
            char szTitle[TITLE_BUFFER_LEN];

            snprintf(szTitle, TITLE_BUFFER_LEN, "Transactions for account: %s (%d)", accountCode.c_str(), result.getNumRows());
            setTitle(szTitle);

            CLIListRow headerRow;

            CLIListColumn column1 = CLIListColumn("Seq", 3, CLIListColumn::rightAligned);
            headerRow.addColumn(column1);

            CLIListColumn column2 = CLIListColumn("Date", DATE_FIELD_LENGTH, CLIListColumn::leftAligned);
            headerRow.addColumn(column2);

            CLIListColumn column3 = CLIListColumn("Description", 25, CLIListColumn::leftAligned);
            headerRow.addColumn(column3);

            CLIListColumn column4 = CLIListColumn("Ctgry", 5, CLIListColumn::leftAligned);
            headerRow.addColumn(column4);

            CLIListColumn column5 = CLIListColumn("Payee", 5, CLIListColumn::leftAligned);
            headerRow.addColumn(column5);

            CLIListColumn column6 = CLIListColumn("CR/DB", 5, CLIListColumn::leftAligned);
            headerRow.addColumn(column6);

            CLIListColumn column7 = CLIListColumn("Amount", 13, CLIListColumn::rightAligned);
            headerRow.addColumn(column7);

            CLIListColumn column8 = CLIListColumn("Rec", 3, CLIListColumn::leftAligned);
            headerRow.addColumn(column8);

            addHeaderRow(headerRow);

            for (int i = 0;i < result.getNumRows();i++) {
                DBTransaction transaction = result.getResultAt(i);

                CLIListRow row(headerRow);

                row.addCellValue(transaction.sequence);
                row.addCellValue(transaction.date);
                row.addCellValue(transaction.description);
                row.addCellValue(transaction.category.code);
                row.addCellValue(transaction.payee.code);
                row.addCellValue(transaction.getCreditDebitValue());
                row.addCellValue(formatCurrency(transaction.amount));
                row.addCellValue(transaction.getIsReconciledValue());

                total += transaction.amount;
                addRow(row);
            }
        }

        void show() override {
            CLIListView::show();
            showBottomBorder();
            cout << "                                                                    Total amount: | " << bold_on << right << setw(13) << formatCurrency(total) << bold_off << " |" << endl << endl;
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
        CLICurrencyField amountField;
        CLITextField isReconciledField;

    public:
        UpdateTransactionView() : UpdateTransactionView("Update transaction") {}
        UpdateTransactionView(const char * title) : CLIView(title) {}

        void setTransaction(DBTransaction & transaction) {
            char szPrompt[MAX_PROMPT_LENGTH];

            transactionId = transaction.id;

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Category ['%s']: ", transaction.category.code.c_str());
            categoryField.setLabel(szPrompt);
            categoryField.setDefaultValue(transaction.category.code);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Payee ['%s']: ", transaction.payee.code.c_str());
            payeeField.setLabel(szPrompt);
            payeeField.setDefaultValue(transaction.payee.code);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Date ['%s']: ", transaction.date.c_str());
            dateField.setLabel(szPrompt);
            dateField.setDefaultValue(transaction.date);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Description ['%s']: ", transaction.description.c_str());
            descriptionField.setLabel(szPrompt);
            descriptionField.setDefaultValue(transaction.description);

            string creditDebit = transaction.getCreditDebitValue();
            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Credit/debit ['%s']: ", creditDebit.c_str());
            creditDebitField.setLabel(szPrompt);
            creditDebitField.setDefaultValue(creditDebit);
            creditDebitField.setLengthLimit(2);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Amount [%.2f]: ", transaction.amount);
            amountField.setLabel(szPrompt);
            amountField.setDefaultValue(transaction.amount);
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

#include <iostream>
#include <string>
#include <string.h>
#include <vector>

#include "pfm_error.h"
#include "cli_widget.h"
#include "custom_widgets.h"
#include "db_transaction.h"

using namespace std;

#ifndef __TRANSACTION_VIEW
#define __TRANSACTION_VIEW

class AddTransactionView : public CLIView {
    private:
        CategorySpinField categoryField = CategorySpinField("Category code (max. 5 chars): ");
        PayeeSpinField payeeField = PayeeSpinField("Payee code (max. 5 chars): ");
        DateField dateField = DateField("Date (yyyy-mm-dd)[today]: ");
        CLITextField descriptionField = CLITextField("Description: ");
        CLITextField creditDebitField = CLITextField("Credit/Debit [DB]: ");
        CLICurrencyField amountField = CLICurrencyField("Amount: ");
        CLITextField isReconciledField = CLITextField("Is reconciled [N]: ");

        bool strtobool(const char * yes_no) {
            if (yes_no[0] == 'y' || yes_no[0] == 'Y') {
                return true;
            }
            else {
                return false;
            }
        }

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
        Money total;

    public:
        TransactionListView() : CLIListView() {
            total = 0.0;
        }

        void addResults(DBResult<DBTransaction> & result, string & accountCode) {
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

            CLIListColumn column7 = CLIListColumn("Amount", 16, CLIListColumn::rightAligned);
            headerRow.addColumn(column7);

            CLIListColumn column8 = CLIListColumn("Rec", 3, CLIListColumn::leftAligned);
            headerRow.addColumn(column8);

            addHeaderRow(headerRow);

            for (int i = 0;i < result.getNumRows();i++) {
                DBTransaction transaction = result.getResultAt(i);

                CLIListRow row(headerRow);

                row.addCellValue(transaction.sequence);
                row.addCellValue(transaction.date.shortDate());
                row.addCellValue(transaction.description);
                row.addCellValue(transaction.category.code);
                row.addCellValue(transaction.payee.code);
                row.addCellValue(transaction.getCreditDebitValue());
                row.addCellValue(transaction.amount);
                row.addCellValue(transaction.getIsReconciledValue());

                total += transaction.getSignedAmount();
                addRow(row);
            }
        }

        void show() override {
            CLIListView::show();
            showBottomBorder();
            cout << "                                                                    Total amount: | " << bold_on << right << setw(13) << total.getFormattedStringValue() << bold_off << " |" << endl << endl;
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
        pfm_id_t transactionId;

        CategorySpinField categoryField;
        PayeeSpinField payeeField;
        DateField dateField;
        CLITextField descriptionField;
        CLITextField creditDebitField;
        CLICurrencyField amountField;
        CLITextField isReconciledField;

        bool decodeCreditDebit(const char * credit_debit) {
            if (strncmp(credit_debit, "CR", 2) == 0) {
                return true;
            }
            else if (strncmp(credit_debit, "DB", 2) == 0) {
                return false;
            }
            else {
                return false;
            }
        }

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

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Date ['%s']: ", transaction.date.shortDate().c_str());
            dateField.setLabel(szPrompt);
            dateField.setDefaultValue(transaction.date.shortDate());

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Description ['%s']: ", transaction.description.c_str());
            descriptionField.setLabel(szPrompt);
            descriptionField.setDefaultValue(transaction.description);

            string creditDebit = transaction.getCreditDebitValue();
            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Credit/debit ['%s']: ", creditDebit.c_str());
            creditDebitField.setLabel(szPrompt);
            creditDebitField.setDefaultValue(creditDebit);
            creditDebitField.setLengthLimit(2);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Amount [%s]: ", transaction.amount.getRawStringValue().c_str());
            amountField.setLabel(szPrompt);
            amountField.setDefaultValue(transaction.amount.getRawStringValue());
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
            transaction.amount = amountField.getValue();

            return transaction;
        }
};

class FindTransactionByPayeeView : CLIView {
    private:
        PayeeSpinField payeeField = PayeeSpinField("Payee code (max. 5 chars): ");
        DateField afterDateField = DateField("Date (yyyy-mm-dd): ");
        DateField beforeDateField = DateField("Date (yyyy-mm-dd)[today]: ");

    public:
        FindTransactionByPayeeView() : FindTransactionByPayeeView("Find transaction (by payee)") {}

        FindTransactionByPayeeView(const char * title) : CLIView(title) {
            string today = StrDate::today();
            beforeDateField.setDefaultValue(today);
        }

        void show() override {
            CLIView::show();

            payeeField.show();
            afterDateField.show();
            beforeDateField.show();
        }

        string getCriteria() {
            static string criteria;

            DBPayee payee = payeeField.getPayee();

            char buffer[32];
            snprintf(buffer, 16, "payee_id = %lld", payee.id);
            criteria = buffer;

            if (afterDateField.getValue().length() > 0) {
                criteria += " AND date > '" + afterDateField.getValue() + "'";
            }

            if (beforeDateField.getValue().length() > 0) {
                criteria += " AND date <= '" + beforeDateField.getValue() + "'";
            }

            return criteria;
        }
};

class FindTransactionByCategoryView : CLIView {
    private:
        CategorySpinField categoryField = CategorySpinField("Category code (max. 5 chars): ");
        DateField afterDateField = DateField("Date (yyyy-mm-dd): ");
        DateField beforeDateField = DateField("Date (yyyy-mm-dd)[today]: ");

    public:
        FindTransactionByCategoryView() : FindTransactionByCategoryView("Find transaction (by category)") {}

        FindTransactionByCategoryView(const char * title) : CLIView(title) {
            string today = StrDate::today();
            beforeDateField.setDefaultValue(today);
        }

        void show() override {
            CLIView::show();

            categoryField.show();
            afterDateField.show();
            beforeDateField.show();
        }

        string getCriteria() {
            static string criteria;

            DBCategory category = categoryField.getCategory();

            char buffer[32];
            snprintf(buffer, 16, "category_id = %lld", category.id);
            criteria = buffer;

            if (afterDateField.getValue().length() > 0) {
                criteria += " AND date > '" + afterDateField.getValue() + "'";
            }

            if (beforeDateField.getValue().length() > 0) {
                criteria += " AND date <= '" + beforeDateField.getValue() + "'";
            }

            return criteria;
        }
};

class FindTransactionByDescriptionView : CLIView {
    private:
        CLITextField descriptionField = CLITextField("Transaction description: ");
        DateField afterDateField = DateField("Date (yyyy-mm-dd): ");
        DateField beforeDateField = DateField("Date (yyyy-mm-dd)[today]: ");

    public:
        FindTransactionByDescriptionView() : FindTransactionByDescriptionView("Find transaction (by description)") {}

        FindTransactionByDescriptionView(const char * title) : CLIView(title) {
            string today = StrDate::today();
            beforeDateField.setDefaultValue(today);
        }

        void show() override {
            CLIView::show();

            descriptionField.show();
            afterDateField.show();
            beforeDateField.show();
        }

        string getCriteria() {
            static string criteria;

            string description = descriptionField.getValue();

            criteria = "description~= '*" + description + "*'";

            if (afterDateField.getValue().length() > 0) {
                criteria += " AND date > '" + afterDateField.getValue() + "'";
            }

            if (beforeDateField.getValue().length() > 0) {
                criteria += " AND date <= '" + beforeDateField.getValue() + "'";
            }

            return criteria;
        }
};

class FindTransactionByDateView : CLIView {
    private:
        DateField afterDateField = DateField("Date (yyyy-mm-dd): ");
        DateField beforeDateField = DateField("Date (yyyy-mm-dd)[today]: ");

    public:
        FindTransactionByDateView() : FindTransactionByDateView("Find transaction (by date)") {}

        FindTransactionByDateView(const char * title) : CLIView(title) {
            string today = StrDate::today();
            beforeDateField.setDefaultValue(today);
        }

        void show() override {
            CLIView::show();

            afterDateField.show();
            beforeDateField.show();
        }

        string getCriteria() {
            static string criteria;

            if (afterDateField.getValue().length() > 0) {
                criteria += " date > '" + afterDateField.getValue() + "'";
            }

            if (beforeDateField.getValue().length() > 0) {
                if (afterDateField.getValue().length() > 0) {
                    criteria += " AND";
                }

                criteria += " date <= '" + beforeDateField.getValue() + "'";
            }

            return criteria;
        }
};

class FindTransactionView : CLIView {
    private:
        CLITextField criteriaField = CLITextField("WHERE: ");

    public:
        FindTransactionView() : FindTransactionView("Find transaction (by SQL criteria)") {}

        FindTransactionView(const char * title) : CLIView(title) {}

        void show() override {
            CLIView::show();

            criteriaField.show();
        }

        string getCriteria() {
            return criteriaField.getValue();
        }
};

#endif

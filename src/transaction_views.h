#include <iostream>
#include <string>
#include <string.h>
#include <vector>

#include "pfm_error.h"
#include "cli_widget.h"
#include "custom_widgets.h"
#include "db_transaction.h"
#include "db_v_transaction.h"

using namespace std;

#ifndef __TRANSACTION_VIEW
#define __TRANSACTION_VIEW

#define CRITERIA_BUFFER_LENGTH                      64

class AddTransactionView : public CLIView {
    private:
        CategorySpinField categoryField = CategorySpinField("Category code (max. 5 chars): ");
        PayeeSpinField payeeField = PayeeSpinField("Payee code (max. 5 chars): ");
        DateField dateField = DateField("Date (yyyy-mm-dd)[today]: ");
        CLITextField descriptionField = CLITextField("Description: ");
        CLITextField referenceField = CLITextField("Reference: ");
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
            referenceField.show();
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
            transaction.reference = referenceField.getValue();

            string creditDebit = creditDebitField.getValue();
            transaction.isCredit = transaction.getIsCredit(creditDebit);
            transaction.amount = amountField.getDoubleValue();
            transaction.isReconciled = strtobool(isReconciledField.getValue().c_str());

            return transaction;
        }
};

class TransferToAccountView : public CLIView {
    private:
        string sourceAccountCode;

        AccountSpinField accountField = AccountSpinField("Account to transfer to (max. 5 chars): ");
        CategorySpinField categoryField = CategorySpinField("Category code (max. 5 chars): ");
        DateField dateField = DateField("Date [today]: ");
        CLITextField descriptionField = CLITextField("Description: ");
        CLICurrencyField amountField = CLICurrencyField("Amount: ");

    public:
        TransferToAccountView() : TransferToAccountView("Transfer to Account") {}

        TransferToAccountView(const char * title) : CLIView(title) {
            string today = StrDate::today();
            dateField.setDefaultValue(today);
        }

        void setSourceAccountCode(const string & accountCode) {
            sourceAccountCode = accountCode;
        }

        void show() override {
            CLIView::show();

            accountField.show();
            categoryField.show();
            dateField.show();
            descriptionField.show();
            amountField.show();
        }

        DBTransaction getSourceTransaction() {
            DBTransaction transaction;

            transaction.category = categoryField.getCategory();
            transaction.categoryId = transaction.category.id;

            transaction.date = dateField.getValue();
            transaction.description = descriptionField.getValue();
            transaction.reference = "TR > " + accountField.getAccount().code;

            transaction.isCredit = false;
            transaction.amount = amountField.getDoubleValue();
            transaction.isReconciled = false;

            return transaction;
        }

        DBTransaction getTargetTransaction() {
            DBTransaction transaction;

            transaction.accountId = accountField.getAccount().id;
            transaction.category = categoryField.getCategory();
            transaction.categoryId = transaction.category.id;

            transaction.date = dateField.getValue();
            transaction.description = descriptionField.getValue();
            transaction.reference = "TR < " + sourceAccountCode;

            transaction.isCredit = true;
            transaction.amount = amountField.getDoubleValue();
            transaction.isReconciled = false;

            return transaction;
        }
};

class TransactionListView : public CLIListView {
    private:
        Money total;
        bool showTotal;

        void showResultsTable(DBResult<DBTransactionView> & result) {
            CLIListRow headerRow;

            CLIListColumn column1 = CLIListColumn("Seq", 3, CLIListColumn::rightAligned);
            headerRow.addColumn(column1);

            CLIListColumn column2 = CLIListColumn("Acct.", 5, CLIListColumn::leftAligned);
            headerRow.addColumn(column2);

            CLIListColumn column3 = CLIListColumn("Date", DATE_FIELD_LENGTH, CLIListColumn::leftAligned);
            headerRow.addColumn(column3);

            CLIListColumn column4 = CLIListColumn("Description", 25, CLIListColumn::leftAligned);
            headerRow.addColumn(column4);

            CLIListColumn column5 = CLIListColumn("Reference", 9, CLIListColumn::leftAligned);
            headerRow.addColumn(column5);

            CLIListColumn column6 = CLIListColumn("Ctgry", 5, CLIListColumn::leftAligned);
            headerRow.addColumn(column6);

            CLIListColumn column7 = CLIListColumn("Payee", 5, CLIListColumn::leftAligned);
            headerRow.addColumn(column7);

            CLIListColumn column8 = CLIListColumn("CD", 2, CLIListColumn::leftAligned);
            headerRow.addColumn(column8);

            CLIListColumn column9 = CLIListColumn("Amount", 11, CLIListColumn::rightAligned);
            headerRow.addColumn(column9);

            CLIListColumn column10 = CLIListColumn("R", 1, CLIListColumn::leftAligned);
            headerRow.addColumn(column10);

            addHeaderRow(headerRow);

            for (int i = 0;i < result.getNumRows();i++) {
                DBTransactionView transaction = result.getResultAt(i);

                CLIListRow row(headerRow);

                row.addCellValue(transaction.sequence);
                row.addCellValue(transaction.accountCode);
                row.addCellValue(transaction.date.shortDate());
                row.addCellValue(transaction.description);
                row.addCellValue(transaction.reference);
                row.addCellValue(transaction.categoryCode);
                row.addCellValue(transaction.payeeCode);
                row.addCellValue(transaction.getCreditDebitValue());
                row.addCellValue(transaction.amount);
                row.addCellValue(transaction.getIsReconciledValue());

                total += transaction.getSignedAmount();
                addRow(row);
            }
        }

    public:
        TransactionListView() : CLIListView() {
            TransactionListView(true);
        }

        TransactionListView(bool showTotal) : CLIListView() {
            total = 0.0;
            this->showTotal = showTotal;
        }

        void addResults(DBResult<DBTransactionView> & result) {
            char szTitle[TITLE_BUFFER_LEN];

            snprintf(szTitle, TITLE_BUFFER_LEN, "Transactions list (%d)", result.getNumRows());
            setTitle(szTitle);

            showResultsTable(result);
        }

        void addResults(DBResult<DBTransactionView> & result, string & accountCode) {
            char szTitle[TITLE_BUFFER_LEN];

            snprintf(szTitle, TITLE_BUFFER_LEN, "Transactions for account: %s (%d)", accountCode.c_str(), result.getNumRows());
            setTitle(szTitle);

            showResultsTable(result);
        }

        void show() override {
            CLIListView::showNoExtraCR();

            if (showTotal) {
                cout << "                                                                          Total amount: | " << bold_on << right << setw(12) << total.getFormattedStringValue() << bold_off << " |" << endl << endl;
            }
            else {
                cout << endl;
            }
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
        pfm_id_t accountId;
        pfm_id_t transactionId;
        pfm_id_t recurringChargeId;

        CategorySpinField categoryField;
        PayeeSpinField payeeField;
        DateField dateField;
        CLITextField descriptionField;
        CLITextField referenceField;
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

            transaction.retrieve();

            transactionId = transaction.id;
            recurringChargeId = transaction.recurringChargeId;
            accountId = transaction.accountId;

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

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Reference ['%s']: ", transaction.reference.c_str());
            referenceField.setLabel(szPrompt);
            referenceField.setDefaultValue(transaction.reference);

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
            transaction.recurringChargeId = recurringChargeId;
            transaction.accountId = accountId;

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

class FindTransactionByPayeeView : public CLIFindView {
    private:
        AccountSpinField accountField = AccountSpinField("Account code: ");
        PayeeSpinField payeeField = PayeeSpinField("Payee code (max. 5 chars): ");
        DateField afterDateField = DateField("Earlist date (yyyy-mm-dd): ");
        DateField beforeDateField = DateField("Latest date (yyyy-mm-dd)[today]: ");
        CLITextField recurringIncludeType = CLITextField("Include recurring (yes, no, only)[no]: ");

    public:
        FindTransactionByPayeeView() : FindTransactionByPayeeView("Find transaction (by payee)") {}

        FindTransactionByPayeeView(const char * title) : CLIFindView(title) {
            string today = StrDate::today();
            beforeDateField.setDefaultValue(today);
            recurringIncludeType.setDefaultValue("no");
        }

        void show() override {
            CLIFindView::show();

            accountField.show();
            payeeField.show();
            afterDateField.show();
            beforeDateField.show();
            recurringIncludeType.show();
        }

        string getCriteria() override {
            string criteria = "";

            DBAccount account = accountField.getAccount();
            DBPayee payee = payeeField.getPayee();

            if (accountField.getValue().length() > 0) {
                criteria += "account_code = '" + account.code + "' AND ";
            }

            char buffer[CRITERIA_BUFFER_LENGTH];
            snprintf(buffer, CRITERIA_BUFFER_LENGTH, "payee_id = %lld", payee.id);

            criteria += buffer;

            if (afterDateField.getValue().length() > 0) {
                StrDate earliestDate = afterDateField.getValue();
                criteria += " AND date > '" + earliestDate.shortDate() + "'";
            }

            if (beforeDateField.getValue().length() > 0) {
                StrDate latestDate = beforeDateField.getValue();
                criteria += " AND date <= '" + latestDate.shortDate() + "'";
            }

            if (recurringIncludeType.getValue().length() > 0) {
                if (recurringIncludeType.getValue().compare("only") == 0) {
                    criteria += " AND recurring_charge_id <> 0";
                }
                else if (recurringIncludeType.getValue().compare("no") == 0) {
                    criteria += " AND recurring_charge_id = 0";
                }
                else if (recurringIncludeType.getValue().compare("yes") == 0) {
                }
                else {
                    throw pfm_validation_error(
                                pfm_error::buildMsg(
                                    "Invalid recurring include type '%s'", 
                                    recurringIncludeType.getValue().c_str()));
                }
            }
            
            return criteria;
        }
};

class FindTransactionByCategoryView : public CLIFindView {
    private:
        AccountSpinField accountField = AccountSpinField("Account code: ");
        CategorySpinField categoryField = CategorySpinField("Category code (max. 5 chars): ");
        DateField afterDateField = DateField("Earlist date (yyyy-mm-dd): ");
        DateField beforeDateField = DateField("Latest date (yyyy-mm-dd)[today]: ");
        CLITextField recurringIncludeType = CLITextField("Include recurring (yes, no, only)[no]: ");

    public:
        FindTransactionByCategoryView() : FindTransactionByCategoryView("Find transaction (by category)") {}

        FindTransactionByCategoryView(const char * title) : CLIFindView(title) {
            string today = StrDate::today();
            beforeDateField.setDefaultValue(today);
            recurringIncludeType.setDefaultValue("no");
        }

        void show() override {
            CLIFindView::show();

            accountField.show();
            categoryField.show();
            afterDateField.show();
            beforeDateField.show();
            recurringIncludeType.show();
        }

        string getCriteria() override {
            string criteria = "";

            DBAccount account = accountField.getAccount();
            DBCategory category = categoryField.getCategory();

            if (accountField.getValue().length() > 0) {
                criteria += "account_code = '" + account.code + "' AND ";
            }

            char buffer[CRITERIA_BUFFER_LENGTH];
            snprintf(buffer, CRITERIA_BUFFER_LENGTH, "category_id = %lld", category.id);

            criteria += buffer;

            if (afterDateField.getValue().length() > 0) {
                StrDate earliestDate = afterDateField.getValue();
                criteria += " AND date >= '" + earliestDate.shortDate() + "'";
            }

            if (beforeDateField.getValue().length() > 0) {
                StrDate latestDate = beforeDateField.getValue();
                criteria += " AND date <= '" + latestDate.shortDate() + "'";
            }

            if (recurringIncludeType.getValue().length() > 0) {
                if (recurringIncludeType.getValue().compare("only") == 0) {
                    criteria += " AND recurring_charge_id <> 0";
                }
                else if (recurringIncludeType.getValue().compare("no") == 0) {
                    criteria += " AND recurring_charge_id = 0";
                }
                else if (recurringIncludeType.getValue().compare("yes") == 0) {
                }
                else {
                    throw pfm_validation_error(
                                pfm_error::buildMsg(
                                    "Invalid recurring include type '%s'", 
                                    recurringIncludeType.getValue().c_str()));
                }
            }
            
            return criteria;
        }
};

class FindTransactionByDescriptionView : public CLIFindView {
    private:
        AccountSpinField accountField = AccountSpinField("Account code: ");
        CLITextField descriptionField = CLITextField("Transaction description: ");
        DateField afterDateField = DateField("Earlist date (yyyy-mm-dd): ");
        DateField beforeDateField = DateField("Latest date (yyyy-mm-dd)[today]: ");
        CLITextField recurringIncludeType = CLITextField("Include recurring (yes, no, only)[no]: ");

    public:
        FindTransactionByDescriptionView() : FindTransactionByDescriptionView("Find transaction (by description)") {}

        FindTransactionByDescriptionView(const char * title) : CLIFindView(title) {
            string today = StrDate::today();
            beforeDateField.setDefaultValue(today);
            recurringIncludeType.setDefaultValue("no");
        }

        void show() override {
            CLIView::show();

            accountField.show();
            descriptionField.show();
            afterDateField.show();
            beforeDateField.show();
            recurringIncludeType.show();
        }

        string getCriteria() override {
            string criteria = "";

            DBAccount account = accountField.getAccount();
            string description = descriptionField.getValue();

            if (accountField.getValue().length() > 0) {
                criteria += "account_code = '" + account.code + "' AND ";
            }

            criteria += "description LIKE '%" + description + "%'";

            if (afterDateField.getValue().length() > 0) {
                StrDate earliestDate = afterDateField.getValue();
                criteria += " AND date >= '" + earliestDate.shortDate() + "'";
            }

            if (beforeDateField.getValue().length() > 0) {
                StrDate latestDate = beforeDateField.getValue();
                criteria += " AND date <= '" + latestDate.shortDate() + "'";
            }
            
            if (recurringIncludeType.getValue().length() > 0) {
                if (recurringIncludeType.getValue().compare("only") == 0) {
                    criteria += " AND recurring_charge_id <> 0";
                }
                else if (recurringIncludeType.getValue().compare("no") == 0) {
                    criteria += " AND recurring_charge_id = 0";
                }
                else if (recurringIncludeType.getValue().compare("yes") == 0) {
                }
                else {
                    throw pfm_validation_error(
                                pfm_error::buildMsg(
                                    "Invalid recurring include type '%s'", 
                                    recurringIncludeType.getValue().c_str()));
                }
            }
            
            return criteria;
        }
};

class FindTransactionByDateView : public CLIFindView {
    private:
        AccountSpinField accountField = AccountSpinField("Account code: ");
        DateField afterDateField = DateField("Earliest date (yyyy-mm-dd): ");
        DateField beforeDateField = DateField("Latest date (yyyy-mm-dd)[today]: ");
        CLITextField recurringIncludeType = CLITextField("Include recurring (yes, no, only)[no]: ");

    public:
        FindTransactionByDateView() : FindTransactionByDateView("Find transaction (by date)") {}

        FindTransactionByDateView(const char * title) : CLIFindView(title) {
            string today = StrDate::today();
            beforeDateField.setDefaultValue(today);
            recurringIncludeType.setDefaultValue("no");
        }

        void show() override {
            CLIView::show();

            accountField.show();
            afterDateField.show();
            beforeDateField.show();
            recurringIncludeType.show();
        }

        string getCriteria() override {
            string criteria = "";

            DBAccount account = accountField.getAccount();

            if (accountField.getValue().length() > 0) {
                criteria += "account_code = '" + account.code + "' AND ";
            }
 
            if (afterDateField.getValue().length() > 0) {
                StrDate earliestDate = afterDateField.getValue();
                criteria += " date >= '" + earliestDate.shortDate() + "'";
            }

            if (beforeDateField.getValue().length() > 0) {
                StrDate latestDate = beforeDateField.getValue();

                if (afterDateField.getValue().length() > 0) {
                    criteria += " AND";
                }

                criteria += " date <= '" + latestDate.shortDate() + "'";
            }

            if (recurringIncludeType.getValue().length() > 0) {
                if (recurringIncludeType.getValue().compare("only") == 0) {
                    criteria += " AND recurring_charge_id <> 0";
                }
                else if (recurringIncludeType.getValue().compare("no") == 0) {
                    criteria += " AND recurring_charge_id = 0";
                }
                else if (recurringIncludeType.getValue().compare("yes") == 0) {
                }
                else {
                    throw pfm_validation_error(
                                pfm_error::buildMsg(
                                    "Invalid recurring include type '%s'", 
                                    recurringIncludeType.getValue().c_str()));
                }
            }
            
            return criteria;
        }
};

class FindTransactionView : public CLIFindView {
    private:
        CLITextField criteriaField = CLITextField("WHERE: ");

    public:
        FindTransactionView() : FindTransactionView("Find transaction (by SQL criteria)") {}

        FindTransactionView(const char * title) : CLIFindView(title) {
            criteriaField.setLengthLimit(CRITERIA_FIELD_MAX_LEN);
        }

        void show() override {
            CLIView::show();

            criteriaField.show();
        }

        string getCriteria() override {
            string criteria = criteriaField.getValue();

            return criteria;
        }
};

#endif

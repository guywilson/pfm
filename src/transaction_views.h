#include <iostream>
#include <string>
#include <string.h>
#include <vector>

#include "terminal.h"
#include "pfm_error.h"
#include "cli_widget.h"
#include "custom_widgets.h"
#include "db_base.h"
#include "db_transaction.h"
#include "db_v_transaction.h"
#include "cfgmgr.h"

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
            creditDebitField.setDefaultValue(TYPE_DEBIT);

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

            transaction.type = creditDebitField.getValue();
            transaction.amount = amountField.getDoubleValue();
            transaction.isReconciled = strtobool(isReconciledField.getValue().c_str());

            return transaction;
        }
};

class TransactionListView : public CLIListView {
    private:
        Money total;
        bool isTotalEnabled;
        int totalAlignmentColumn;

        void buildResultsTable(DBResult<DBTransactionView> & result, bool showAccount) {
            reserveRows(result.size());

            if (showAccount) {
                totalAlignmentColumn = 9;

                setColumns({
                    CLIListColumn("Sq", LIST_VIEW_SEQUENCE_WIDTH, CLIListColumn::rightAligned),
                    CLIListColumn("Acct.", LIST_VIEW_CODE_WIDTH, CLIListColumn::leftAligned),
                    CLIListColumn("Date", DATE_FIELD_LENGTH, CLIListColumn::leftAligned),
                    CLIListColumn("C", LIST_VIEW_RECURRING_CHARGE_WIDTH, CLIListColumn::leftAligned),
                    CLIListColumn("Description", LIST_VIEW_DESCRIPTION_WIDTH, CLIListColumn::leftAligned),
                    CLIListColumn("Ctgry", LIST_VIEW_CODE_WIDTH, CLIListColumn::leftAligned),
                    CLIListColumn("Tp", LIST_VIEW_TYPE_WIDTH, CLIListColumn::leftAligned),
                    CLIListColumn("Amount", LIST_VIEW_AMOUNT_WIDTH, CLIListColumn::rightAligned),
                    CLIListColumn("R", LIST_VIEW_RECONCILED_WIDTH, CLIListColumn::leftAligned)
                });
            }
            else {
                totalAlignmentColumn = 8;
                
                setColumns({
                    CLIListColumn("Sq", LIST_VIEW_SEQUENCE_WIDTH, CLIListColumn::rightAligned),
                    CLIListColumn("Date", DATE_FIELD_LENGTH, CLIListColumn::leftAligned),
                    CLIListColumn("C", LIST_VIEW_RECURRING_CHARGE_WIDTH, CLIListColumn::leftAligned),
                    CLIListColumn("Description", LIST_VIEW_DESCRIPTION_WIDTH, CLIListColumn::leftAligned),
                    CLIListColumn("Ctgry", LIST_VIEW_CODE_WIDTH, CLIListColumn::leftAligned),
                    CLIListColumn("Tp", LIST_VIEW_TYPE_WIDTH, CLIListColumn::leftAligned),
                    CLIListColumn("Amount", LIST_VIEW_AMOUNT_WIDTH, CLIListColumn::rightAligned),
                    CLIListColumn("R", LIST_VIEW_RECONCILED_WIDTH, CLIListColumn::leftAligned)
                });
            }

            for (size_t i = 0;i < result.size();i++) {
                DBTransactionView transaction = result.at(i);

                CLIListRow row = CLIListRow(getNumColumns());

                row.addCell(transaction.sequence);

                if (showAccount) {
                    row.addCell(transaction.account);
                }

                row.addCell(transaction.date);
                row.addCell(transaction.isRecurring);
                row.addCell(transaction.description);
                row.addCell(transaction.category);
                row.addCell(transaction.type);
                row.addCell(transaction.amount);
                row.addCell(transaction.isReconciled);

                total += transaction.getSignedAmount();
                addRow(row);
            }
        }

        void buildResultsTable(DBResult<DBTransactionView> & result) {
            buildResultsTable(result, true);
        }

    public:
        TransactionListView() : CLIListView() {
            isTotalEnabled = false;

#ifndef RUN_IN_DEBUGGER
            if (Terminal::getWidth() < getMinimumWidth()) {
                throw pfm_error(
                    pfm_error::buildMsg(
                        "Terminal is not wide enough for TransactionListView. Terminal width %u, minimum width %u", 
                        (unsigned int)Terminal::getWidth(), 
                        (unsigned int)getMinimumWidth()));
            }
#endif
        }

        inline uint16_t getMinimumWidth() override {
            return (
                LIST_VIEW_SEQUENCE_WIDTH + 
                DATE_FIELD_LENGTH + 
                LIST_VIEW_DESCRIPTION_WIDTH + 
                LIST_VIEW_TYPE_WIDTH + 
                LIST_VIEW_AMOUNT_WIDTH);
        }

        void addTotal() {
            total = 0.0;
            isTotalEnabled = true;
        }

        void addResults(DBResult<DBTransactionView> & result) {
            buildResultsTable(result, true);

            char szTitle[TITLE_BUFFER_LEN];

            snprintf(szTitle, TITLE_BUFFER_LEN, "Transactions list (%zu) [%d]", result.size(), getTotalWidth());
            setTitle(szTitle);
        }

        void addResults(DBResult<DBTransactionView> & result, string & accountCode) {
            buildResultsTable(result, false);

            char szTitle[TITLE_BUFFER_LEN];

            snprintf(szTitle, TITLE_BUFFER_LEN, "Transactions for account: %s (%zu) [%d]", accountCode.c_str(), result.size(), getTotalWidth());
            setTitle(szTitle);
        }

        void show() override {
            CLIListView::showNoExtraCR();

            if (isTotalEnabled) {
                showTotal("Total amount: ", total);
            }
            else {
                cout << endl;
            }
        }
};

class TransactionCategoryReportListView : public CLIListView {
    private:
        Money total = 0.0;

        void buildResultsTable(DBResult<DBTransactionView> & result) {
            reserveRows(result.size());
            
            setColumns({
                CLIListColumn("Ctgry", LIST_VIEW_CODE_WIDTH, CLIListColumn::leftAligned),
                CLIListColumn("Total", LIST_VIEW_AMOUNT_WIDTH, CLIListColumn::rightAligned),
                CLIListColumn("% of Total", 10, CLIListColumn::rightAligned)
            });

            for (size_t i = 0;i < result.size();i++) {
                DBTransactionView transaction = result[i];
                total += transaction.total;
            }

            char percentOfTotal[8];

            for (size_t i = 0;i < result.size();i++) {
                DBTransactionView transaction = result[i];

                snprintf(
                    percentOfTotal, 
                    8, 
                    "%.2f%%", ((float)transaction.total.doubleValue() / (float)total.doubleValue() * 100.0f));

                CLIListRow row(getNumColumns());

                row.addCell(transaction.category);
                row.addCell(transaction.total);
                row.addCell(string(percentOfTotal));

                addRow(row);
            }
        }

    public:
        TransactionCategoryReportListView() : CLIListView() {
            if (Terminal::getWidth() < getMinimumWidth()) {
                throw pfm_error(
                    pfm_error::buildMsg(
                        "Terminal is not wide enough for TransactionCategoryReportListView. Terminal width %u, minimum width %u", 
                        (unsigned int)Terminal::getWidth(), 
                        (unsigned int)getMinimumWidth()));
            }
        }

        inline uint16_t getMinimumWidth() override {
            return (
                15 + 
                LIST_VIEW_AMOUNT_WIDTH + 
                15);
        }

        void addResults(DBResult<DBTransactionView> & result) {
            char szTitle[TITLE_BUFFER_LEN];

            snprintf(szTitle, TITLE_BUFFER_LEN, "Category report (%zu)", result.size());
            setTitle(szTitle);

            buildResultsTable(result);
        }

        void show() override {
            CLIListView::showNoExtraCR();

            showTotal("Total: ", total);
        }
};

class TransactionPayeeReportListView : public CLIListView {
    private:
        Money total = 0.0;

        void buildResultsTable(DBResult<DBTransactionView> & result) {
            reserveRows(result.size());
            
            setColumns({
                CLIListColumn("Payee", LIST_VIEW_CODE_WIDTH, CLIListColumn::leftAligned),
                CLIListColumn("Total", LIST_VIEW_AMOUNT_WIDTH, CLIListColumn::rightAligned),
                CLIListColumn("% of Total", 10, CLIListColumn::rightAligned)
            });

            for (size_t i = 0;i < result.size();i++) {
                DBTransactionView transaction = result[i];
                total += transaction.total;
            }

            char percentOfTotal[8];

            for (size_t i = 0;i < result.size();i++) {
                DBTransactionView transaction = result.at(i);

                snprintf(
                    percentOfTotal, 
                    8, 
                    "%.2f%%", ((float)transaction.total.doubleValue() / (float)total.doubleValue() * 100.0f));

                CLIListRow row(getNumColumns());

                row.addCell(transaction.payee);
                row.addCell(transaction.total);
                row.addCell(string(percentOfTotal));

                addRow(row);
            }
        }

    public:
        TransactionPayeeReportListView() : CLIListView() {
            if (Terminal::getWidth() < getMinimumWidth()) {
                throw pfm_error(
                    pfm_error::buildMsg(
                        "Terminal is not wide enough for TransactionPayeeReportListView. Terminal width %u, minimum width %u", 
                        (unsigned int)Terminal::getWidth(), 
                        (unsigned int)getMinimumWidth()));
            }
        }

        inline uint16_t getMinimumWidth() override {
            return (
                15 + 
                LIST_VIEW_AMOUNT_WIDTH + 
                15);
        }

        void addResults(DBResult<DBTransactionView> & result) {
            char szTitle[TITLE_BUFFER_LEN];

            snprintf(szTitle, TITLE_BUFFER_LEN, "Payee report (%zu)", result.size());
            setTitle(szTitle);

            buildResultsTable(result);
        }

        void show() override {
            CLIListView::showNoExtraCR();

            showTotal("Total: ", total);
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

        bool isReconciled;

        CategorySpinField categoryField;
        PayeeSpinField payeeField;
        DateField dateField;
        CLITextField descriptionField;
        CLITextField referenceField;
        CLITextField creditDebitField;
        CLICurrencyField amountField;

        bool decodeCreditDebit(const char * credit_debit) {
            if (strncmp(credit_debit, TYPE_CREDIT, 2) == 0) {
                return true;
            }
            else if (strncmp(credit_debit, TYPE_DEBIT, 2) == 0) {
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
            isReconciled = transaction.isReconciled;

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

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Credit/debit ['%s']: ", transaction.type.c_str());
            creditDebitField.setLabel(szPrompt);
            creditDebitField.setDefaultValue(transaction.type);
            creditDebitField.setLengthLimit(2);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Amount [%s]: ", transaction.amount.rawStringValue().c_str());
            amountField.setLabel(szPrompt);
            amountField.setDefaultValue(transaction.amount.rawStringValue());
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
            transaction.isReconciled = isReconciled;

            transaction.category = categoryField.getCategory();
            transaction.categoryId = transaction.category.id;

            transaction.payee = payeeField.getPayee();
            transaction.payeeId = transaction.payee.id;

            transaction.date = dateField.getValue();
            transaction.description = descriptionField.getValue();

            transaction.type = creditDebitField.getValue();
            transaction.amount = amountField.getValue();

            return transaction;
        }
};

class FindTransactionView : public CLIView {
    private:
        AccountSpinField accountField = AccountSpinField("Account: ");
        CategorySpinField categoryField = CategorySpinField("Category (max. 5 chars): ");
        PayeeSpinField payeeField = PayeeSpinField("Payee (max. 5 chars): ");
        CLITextField descriptionField = CLITextField("Transaction description: ");
        DateField afterDateField = DateField("Earlist date (yyyy-mm-dd): ");
        DateField beforeDateField = DateField("Latest date (yyyy-mm-dd)[today]: ");
        CLITextField recurringIncludeType = CLITextField("Include recurring (yes, no, only)[no]: ");

    public:
        FindTransactionView() : FindTransactionView("Find transactions") {}

        FindTransactionView(const char * title) : CLIView(title) {
            string today = StrDate::today();
            beforeDateField.setDefaultValue(today);
            recurringIncludeType.setDefaultValue("no");
        }

        void show() override {
            CLIView::show();

            accountField.show();
            categoryField.show();
            payeeField.show();
            descriptionField.show();
            afterDateField.show();
            beforeDateField.show();
            recurringIncludeType.show();
        }

        DBCriteria getCriteria() {
            DBCriteria criteria;

            DBAccount account = accountField.getAccount();
            DBCategory category = categoryField.getCategory();
            DBPayee payee = payeeField.getPayee();
            string description = descriptionField.getValue();
            StrDate afterDate = afterDateField.getValue();
            StrDate beforeDate = beforeDateField.getValue();

            if (accountField.getValue().length() > 0) {
                criteria.add("account_id", DBCriteria::equal_to, account.id);
            }

            if (categoryField.getValue().length() > 0) {
                criteria.add("category_id", DBCriteria::equal_to, category.id);
            }

            if (payeeField.getValue().length() > 0) {
                criteria.add("payee_id", DBCriteria::equal_to, payee.id);
            }

            if (descriptionField.getValue().length() > 0) {
                string d = '%' + description + '%';
                criteria.add("description", DBCriteria::like, d);
            }

            if (afterDateField.getValue().length() > 0) {
                criteria.add("date", DBCriteria::greater_than, afterDate);
            }

            if (beforeDateField.getValue().length() > 0) {
                criteria.add("date", DBCriteria::less_than_or_equal, beforeDate);
            }

            if (recurringIncludeType.getValue().length() > 0) {
                if (recurringIncludeType.getValue().compare("only") == 0) {
                    criteria.add("recurring", true);
                }
                else if (recurringIncludeType.getValue().compare("no") == 0) {
                    criteria.add("recurring", false);
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

#endif

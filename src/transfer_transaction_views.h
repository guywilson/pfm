#include <iostream>
#include <string>
#include <string.h>
#include <vector>

#include "pfm_error.h"
#include "cli_widget.h"
#include "terminal.h"
#include "custom_widgets.h"
#include "db_base.h"
#include "db_transfer_transaction_record.h"
#include "db_v_transfer_record.h"
#include "cfgmgr.h"

using namespace std;

#ifndef __TRANSFER_TRANSACTION_VIEW
#define __TRANSFER_TRANSACTION_VIEW

#define CRITERIA_BUFFER_LENGTH                      64

class TransferToAccountView : public CLIView {
    private:
        AccountSpinField accountField = AccountSpinField("Account to transfer to (max. 5 chars): ");
        CategorySpinField categoryField = CategorySpinField("Category code (max. 5 chars): ");
        DateField dateField = DateField("Date [today]: ");
        CLITextField descriptionField = CLITextField("Description: ");
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
        TransferToAccountView() : TransferToAccountView("Transfer to Account") {}

        TransferToAccountView(const char * title) : CLIView(title) {
            string today = StrDate::today();
            dateField.setDefaultValue(today);
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
            transaction.amount = amountField.getDoubleValue();
            transaction.isReconciled = strtobool(isReconciledField.getValue().c_str());

            return transaction;
        }

        DBAccount getAccountTo() {
            return accountField.getAccount();
        }
};

class TransferListView : public CLIListView {
    private:
        Money total;
        bool isTotalEnabled;

        void showResultsTable(DBResult<DBTransferRecordView> & result) {
            setColumns({
                CLIListColumn("Seq", 3, CLIListColumn::rightAligned),
                CLIListColumn("From:", 5, CLIListColumn::leftAligned),
                CLIListColumn("To:", 5, CLIListColumn::leftAligned),
                CLIListColumn("Date", DATE_FIELD_LENGTH, CLIListColumn::leftAligned),
                CLIListColumn("Description", 25, CLIListColumn::leftAligned),
                CLIListColumn("Ctgry", 5, CLIListColumn::leftAligned),
                CLIListColumn("Payee", 5, CLIListColumn::leftAligned),
                CLIListColumn("Amount", LIST_VIEW_AMOUNT_WIDTH, CLIListColumn::rightAligned)
            });

            for (int i = 0;i < result.size();i++) {
                DBTransferRecordView transfer = result[i];

                CLIListRow row(getNumColumns());

                row.addCell(transfer.sequence);
                row.addCell(transfer.accountFromCode);
                row.addCell(transfer.accountToCode);
                row.addCell(transfer.date);
                row.addCell(transfer.description);
                row.addCell(transfer.categoryCode);
                row.addCell(transfer.payeeCode);
                row.addCell(transfer.amount);

                total += transfer.amount;
                addRow(row);
            }
        }

    public:
        TransferListView() : CLIListView() {
            isTotalEnabled = false;

            if (Terminal::getWidth() < getMinimumWidth()) {
                throw pfm_error(
                    pfm_error::buildMsg(
                        "Terminal is not wide enough for TransferListView. Terminal width %u, minimum width %u", 
                        (unsigned int)Terminal::getWidth(), 
                        (unsigned int)getMinimumWidth()));
            }
        }

        inline uint16_t getMinimumWidth() override {
            return (
                3 + 
                5 + 
                5 + 
                DATE_FIELD_LENGTH + 
                25 + 
                5 + 
                5 + 
                LIST_VIEW_AMOUNT_WIDTH);
        }

        void addTotal() {
            total = 0.0;
            isTotalEnabled = true;
        }

        void addResults(DBResult<DBTransferRecordView> & result) {
            char szTitle[TITLE_BUFFER_LEN];

            snprintf(szTitle, TITLE_BUFFER_LEN, "Transfer list (%d)", result.size());
            setTitle(szTitle);

            showResultsTable(result);
        }

        void show() override {
            CLIListView::showNoExtraCR();

            if (isTotalEnabled) {
                showTotal(7, "Total amount: ", total);
            }
            else {
                cout << endl;
            }
        }
};

class ChooseTransferView : public CLIView {
    private:
        CLITextField sequenceField = CLITextField("Sequence: ");

    public:
        ChooseTransferView() : ChooseTransferView("Use transfer") {}
        ChooseTransferView(const char * title) : CLIView(title) {}

        void show() override {
            CLIView::show();

            sequenceField.show();
        }

        int getSequence() {
            return sequenceField.getIntegerValue();
        }
};

#endif

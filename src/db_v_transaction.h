#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdint.h>

#include <sqlcipher/sqlite3.h>

#include "db.h"
#include "db_base.h"
#include "db_transaction.h"
#include "strdate.h"
#include "pfm_error.h"

using namespace std;

#ifndef __INCL_TRANSACTION_VIEW
#define __INCL_TRANSACTION_VIEW

class DBTransactionView : public DBTransaction {
    protected:
        struct Columns {
            static constexpr const char * account = "account";
            static constexpr ColumnType account_type = ColumnType::TEXT;

            static constexpr const char * category = "category";
            static constexpr ColumnType category_type = ColumnType::TEXT;

            static constexpr const char * payee = "payee";
            static constexpr ColumnType payee_type = ColumnType::TEXT;

            static constexpr const char * type = "type";
            static constexpr ColumnType type_type = ColumnType::TEXT;

            static constexpr const char * recurring = "recurring";
            static constexpr ColumnType recurring_type = ColumnType::BOOL;

            static constexpr const char * isReconciled = "reconciled";
            static constexpr ColumnType isReconciled_type = ColumnType::BOOL;

            static constexpr const char * isTransfer = "is_transfer";
            static constexpr ColumnType isTransfer_type = ColumnType::BOOL;

            static constexpr const char * total = "total";
            static constexpr ColumnType total_type = ColumnType::MONEY;
        };

    public:
        string account;
        string payee;
        string category;
        bool isRecurring;
        bool isTransfer;

        Money total; // For category, payee reports

        enum recurring_type {
            non_recurring,
            recurring_only,
            all
        };

        DBTransactionView() : DBTransaction() {
            clear();
        }

        void clear() {
            DBTransaction::clear();

            this->account = "";
            this->payee = "";
            this->category = "";
            this->isRecurring = false;
            this->isTransfer = false;
        }

        void set(const DBTransactionView & src) {
            DBTransaction::set(src);

            this->account = src.account;
            this->payee = src.payee;
            this->category = src.category;
            this->isRecurring = src.isRecurring;
            this->isTransfer = src.isTransfer;
        }

        void print() {
            DBTransaction::print();

            cout << "Account: '" << account << "'" << endl;
            cout << "IsRecurring: '" << isRecurring << "'" << endl;
            cout << "IsTransfer: '" << isTransfer << "'" << endl;
            cout << "Payee: '" << payee << "'" << endl;
            cout << "Category: '" << category << "'" << endl;
        }

        void assignColumn(DBColumn & column) override {
            DBTransaction::assignColumn(column);
            
            if (column.getName() == Columns::payee) {
                payee = column.getValue();
            }
            else if (column.getName() == Columns::category) {
                category = column.getValue();
            }
            else if (column.getName() == Columns::account) {
                account = column.getValue();
            }
            else if (column.getName() == Columns::recurring) {
                isRecurring = column.getBoolValue();
            }
            else if (column.getName() == Columns::type) {
                type = column.getValue();
            }
            else if (column.getName() == Columns::isReconciled) {
                isReconciled = column.getBoolValue();
            }
            else if (column.getName() == Columns::isTransfer) {
                isTransfer = column.getBoolValue();
            }
            else if (column.getName() == Columns::total) {
                total = column.doubleValue();
            }
        }

        const string getTableName() const override {
            return "v_transaction_list";
        }

        const string getClassName() const override {
            return "DBTransactionView";
        }

        DBResult<DBTransactionView> retrieveByAccountID(pfm_id_t & accountId);
        DBResult<DBTransactionView> retrieveByAccountID(pfm_id_t & accountId, DBCriteria::sql_order dateSortDirection, int rowLimit);
        DBResult<DBTransactionView> listByAccountID(pfm_id_t & accountId, recurring_type recurringType, bool thisPeriod, DBCriteria::sql_order dateSortDirection, int rowLimit);
        DBResult<DBTransactionView> retrieveReconciledByAccountID(pfm_id_t & accountId);
        DBResult<DBTransactionView> retrieveByAccountIDForPeriod(pfm_id_t & accountId, DBCriteria::sql_order dateSortDirection, StrDate & firstDate, StrDate & secondDate);
        DBResult<DBTransactionView> retrieveNonRecurringByAccountIDForPeriod(pfm_id_t & accountId, StrDate & firstDate, StrDate & secondDate);
        DBResult<DBTransactionView> retrieveReconciledByAccountIDForPeriod(pfm_id_t & accountId, StrDate & firstDate, StrDate & secondDate);
        DBResult<DBTransactionView> findTransactions(const string & sql);
        DBResult<DBTransactionView> findTransactionsForCriteria(DBCriteria & criteria);
        DBResult<DBTransactionView> findTransactionsForAccountID(pfm_id_t & accountId, DBCriteria & criteria);
        DBResult<DBTransactionView> reportByCategory();
        DBResult<DBTransactionView> reportByPayee();

        class FindCriteriaHelper {
            public:
                static DBCriteria handleGreaterThanThisDate(DBCriteria & src, const StrDate & date) {
                    src.add(DBPayment::Columns::date, DBCriteria::greater_than, date);
                    return src;
                }

                static DBCriteria handleLessThanThisDate(DBCriteria & src, const StrDate & date) {
                    src.add(DBPayment::Columns::date, DBCriteria::less_than, date);
                    return src;
                }

                static DBCriteria handleOnTheseDates(DBCriteria & src, vector<StrDate> & dates) {
                    if (dates.size() > 0) {
                        for (StrDate & date : dates) {
                            src.addToInClause(DBPayment::Columns::date, date);
                        }

                        src.endInClause_StrDate(DBPayment::Columns::date);
                    }

                    return src;
                }

                static DBCriteria handleLessThanThisAmount(DBCriteria & src, const Money & amount) {
                    src.add(DBPayment::Columns::amount, DBCriteria::less_than, amount);
                    return src;
                }

                static DBCriteria handleGreaterThanThisAmount(DBCriteria & src, const Money & amount) {
                    src.add(DBPayment::Columns::amount, DBCriteria::greater_than, amount);
                    return src;
                }

                static DBCriteria handleWithTheseAccounts(DBCriteria & src, vector<string> & accounts) {
                    if (accounts.size() > 0) {
                        for (string & account : accounts) {
                            src.addToInClause(Columns::account, account);
                        }

                        src.endInClause_string(Columns::account);
                    }

                    return src;
                }

                static DBCriteria handleWithTheseCategories(DBCriteria & src, vector<string> & categories) {
                    if (categories.size() > 0) {
                        for (string & category : categories) {
                            src.addToInClause(Columns::category, category);
                        }

                        src.endInClause_string(Columns::category);
                    }

                    return src;
                }

                static DBCriteria handleWithThesePayees(DBCriteria & src, vector<string> & payees) {
                    if (payees.size() > 0) {
                        for (string & payee : payees) {
                            src.addToInClause(Columns::payee, payee);
                        }

                        src.endInClause_string(Columns::payee);
                    }

                    return src;
                }

                static DBCriteria handleWithThisDescription(DBCriteria & src, const string & description) {
                    if (description.length() > 0) {
                        if (description.find_first_of('%') != string::npos || description.find_first_of('_') != string::npos) {
                            src.add(DBPayment::Columns::description, DBCriteria::like, description);
                        }
                        else {
                            src.add(DBPayment::Columns::description, DBCriteria::equal_to, description);
                        }
                    }

                    return src;
                }

                static DBCriteria handleWithThisReference(DBCriteria & src, const string & reference) {
                    if (reference.length() > 0) {
                        if (reference.find_first_of('%') != string::npos || reference.find_first_of('_') != string::npos) {
                            src.add(DBTransaction::Columns::reference, DBCriteria::like, reference);
                        }
                        else {
                            src.add(DBTransaction::Columns::reference, DBCriteria::equal_to, reference);
                        }
                    }

                    return src;
                }

                static DBCriteria handleWithThisType(DBCriteria & src, const string & type) {
                    if (type.length() > 0) {
                        src.add(Columns::type, DBCriteria::equal_to, type);
                    }

                    return src;
                }

                static DBCriteria handleIsRecurring(DBCriteria & src, const bool & isRecurring) {
                    src.add(Columns::recurring, isRecurring);
                    return src;
                }
        };
};

#endif

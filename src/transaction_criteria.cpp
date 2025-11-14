#include <string>
#include <vector>
#include <string.h>

#include "db_base.h"
#include "db_transaction.h"
#include "db_v_transaction.h"
#include "strdate.h"
#include "money.h"
#include "transaction_criteria.h"

using namespace std;

string FindTransactionCriteriaBuilder::getParameter(int index) const {
    if (parameters.size() == 0) {
        throw pfm_error("Expected parameters but none were supplied");
    }
    else if (index >= (int)parameters.size()) {
        throw pfm_error(
                pfm_error::buildMsg(
                    "Expecting at least %d parameters but only %d have been supplied", 
                    index + 1, 
                    parameters.size()));
    }

    return parameters[index];
}

void FindTransactionCriteriaBuilder::parse() {
    int i = 0;

    while (i < (int)parameters.size()) {
        string parameter = getParameter(i);

        if (parameter.compare("D") == 0) {
            StrDate date = getParameter(i + 1);
            betweenTheseDatesList.push_back(date);
            i += 2;
        }
        else if (parameter.compare("d") == 0) {
            StrDate date = getParameter(i + 1);
            onTheseDatesList.push_back(date);
            i += 2;
        }
        else if (parameter.compare("c") == 0) {
            string categoryCode = getParameter(i + 1);
            withTheseCategoriesList.push_back(categoryCode);
            i += 2;
        }
        else if (parameter.compare("p") == 0) {
            string payeeCode = getParameter(i + 1);
            withThesePayeesList.push_back(payeeCode);
            i += 2;
        }
        else if (parameter.compare("ds") == 0) {
            string ds = getParameter(i + 1);

            for (int c = 0;c < (int)ds.length();c++) {
                if (ds[c] == '*') {
                    description[c] = '%';
                }
                else if (ds[c] == '?') {
                    description[c] = '_';
                }
                else {
                    description[c] = ds[c];
                }
            }
            i += 2;
        }
        else if (parameter.compare("acc") == 0) {
            string accountCode = getParameter(i + 1);
            withTheseAccountsList.push_back(accountCode);
            i += 2;
        }
        else if (parameter.compare("r") == 0) {
            string r = getParameter(i + 1);

            for (int c = 0;c < (int)r.length();c++) {
                if (r[c] == '*') {
                    reference[c] = '%';
                }
                else if (r[c] == '?') {
                    reference[c] = '_';
                }
                else {
                    reference[c] = r[c];
                }
            }
            i += 2;
        }
        else if (parameter.compare("rc") == 0) {
            recurringOrNonRecurring = getParameter(i + 1);
            i += 2;
        }
        else if (parameter.compare("db") == 0) {
            debitOrCredit = TYPE_DEBIT;
            i++;
        }
        else if (parameter.compare("cr") == 0) {
            debitOrCredit = TYPE_CREDIT;
            i++;
        }
        else if (parameter.compare("a") == 0) {
            lessThanThisAmount = getParameter(i + 1);
            i += 2;
        }
        else if (parameter.compare("A") == 0) {
            string amount = getParameter(i + 1);
            betweenTheseAmountList.push_back(amount);
            i += 2;
        }
        else if (parameter.compare("sql") == 0) {
            sqlWhereClause = getParameter(i + 1);

            for (int c = 0;c < (int)sqlWhereClause.length();c++) {
                if (sqlWhereClause[c] == '*') {
                    sqlWhereClause[c] = '%';
                }
                else if (sqlWhereClause[c] == '?') {
                    sqlWhereClause[c] = '_';
                }
            }
            i += 2;
        }
    }
}

FindTransactionCriteriaBuilder::FindTransactionCriteriaBuilder(vector<string> & parms) {
    this->parameters = parms;
    parse();
}

bool FindTransactionCriteriaBuilder::hasRawSQL() {
    if (sqlWhereClause.length() > 0) {
        return true;
    }

    return false;
}

DBCriteria FindTransactionCriteriaBuilder::getCriteria() {
    DBCriteria criteria;

    criteria = DBTransactionView::FindCriteriaHelper::handleBetweenTheseDates(criteria, betweenTheseDatesList);
    criteria = DBTransactionView::FindCriteriaHelper::handleOnTheseDates(criteria, onTheseDatesList);

    criteria = DBTransactionView::FindCriteriaHelper::handleBetweenTheseAmounts(criteria, betweenTheseAmountList);
    criteria = DBTransactionView::FindCriteriaHelper::handleLessThanThisAmount(criteria, lessThanThisAmount);

    criteria = DBTransactionView::FindCriteriaHelper::handleWithTheseAccounts(criteria, withTheseAccountsList);
    criteria = DBTransactionView::FindCriteriaHelper::handleWithTheseCategories(criteria, withTheseCategoriesList);
    criteria = DBTransactionView::FindCriteriaHelper::handleWithThesePayees(criteria, withThesePayeesList);

    criteria = DBTransactionView::FindCriteriaHelper::handleWithThisDescription(criteria, description);
    criteria = DBTransactionView::FindCriteriaHelper::handleWithThisReference(criteria, reference);
    criteria = DBTransactionView::FindCriteriaHelper::handleWithThisType(criteria, debitOrCredit);

    if (recurringOrNonRecurring.length() > 0) {
        criteria = DBTransactionView::FindCriteriaHelper::handleIsRecurring(criteria, recurringOrNonRecurring.compare("r") == 0 ? true : false);
    }

    return criteria;
}

string FindTransactionCriteriaBuilder::getRawSQL() {
    return sqlWhereClause;
}

string AddTransactionCriteriaBuilder::getParameter(int index) const {
    if (parameters.size() == 0) {
        throw pfm_error("Expected parameters but none were supplied");
    }
    else if (index >= (int)parameters.size()) {
        throw pfm_error(
                pfm_error::buildMsg(
                    "Expecting at least %d parameters but only %d have been supplied", 
                    index + 1, 
                    parameters.size()));
    }

    return parameters[index];
}

void AddTransactionCriteriaBuilder::parse() {
    int i = 0;

    while (i < (int)parameters.size()) {
        string parameter = getParameter(i);

        if (parameter.compare("d") == 0) {
            this->date = getParameter(i + 1);
            i += 2;
        }
        else if (parameter.compare("c") == 0) {
            this->categoryCode = getParameter(i + 1);
            i += 2;
        }
        else if (parameter.compare("p") == 0) {
            this->payeeCode = getParameter(i + 1);
            i += 2;
        }
        else if (parameter.compare("ds") == 0) {
            this->description = getParameter(i + 1);
            i += 2;
        }
        else if (parameter.compare("r") == 0) {
            this->reference = getParameter(i + 1);
            i += 2;
        }
        else if (parameter.compare("db") == 0) {
            this->type = TYPE_DEBIT;
            i++;
        }
        else if (parameter.compare("cr") == 0) {
            this->type = TYPE_CREDIT;
            i++;
        }
        else if (parameter.compare("a") == 0) {
            this->amount = getParameter(i + 1);
            i += 2;
        }
    }
}

AddTransactionCriteriaBuilder::AddTransactionCriteriaBuilder(vector<string> & parms) {
    this->parameters = parms;
    parse();
}

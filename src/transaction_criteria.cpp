#include <string>
#include <vector>
#include <string.h>

#include "db_base.h"
#include "db_transaction.h"
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

    if (betweenTheseDatesList.size() == 1) {
        criteria.add("date", DBCriteria::greater_than, betweenTheseDatesList[0]);
    }
    else if (betweenTheseDatesList.size() >= 2) {
        criteria.add("date", DBCriteria::greater_than_or_equal, betweenTheseDatesList[0]);
        criteria.add("date", DBCriteria::less_than_or_equal, betweenTheseDatesList[1]);
    }

    if (betweenTheseAmountList.size() == 1) {
        criteria.add("amount", DBCriteria::greater_than, betweenTheseAmountList[0]);
    }
    else if (betweenTheseAmountList.size() >= 2) {
        criteria.add("amount", DBCriteria::greater_than_or_equal, betweenTheseAmountList[0]);
        criteria.add("amount", DBCriteria::less_than_or_equal, betweenTheseAmountList[1]);
    }

    if (onTheseDatesList.size() > 0) {
        for (StrDate & date : onTheseDatesList) {
            criteria.addToInClause("date", date);
        }

        criteria.endInClause_StrDate("date");
    }

    if (withTheseAccountsList.size() > 0) {
        for (string & account : withTheseAccountsList) {
            criteria.addToInClause("account", account);
        }

        criteria.endInClause_string("account");
    }

    if (withTheseCategoriesList.size() > 0) {
        for (string & category : withTheseCategoriesList) {
            criteria.addToInClause("category", category);
        }

        criteria.endInClause_string("category");
    }

    if (withThesePayeesList.size() > 0) {
        for (string & payee : withThesePayeesList) {
            criteria.addToInClause("payee", payee);
        }

        criteria.endInClause_string("payee");
    }

    if (description.length() > 0) {
        if (description.find_first_of('%') != string::npos || description.find_first_of('_') != string::npos) {
            criteria.add("description", DBCriteria::like, description);
        }
        else {
            criteria.add("description", DBCriteria::equal_to, description);
        }
    }

    if (reference.length() > 0) {
        if (reference.find_first_of('%') != string::npos || reference.find_first_of('_') != string::npos) {
            criteria.add("reference", DBCriteria::like, reference);
        }
        else {
            criteria.add("reference", DBCriteria::equal_to, reference);
        }
    }

    if (debitOrCredit.length() > 0) {
        criteria.add("type", DBCriteria::equal_to, debitOrCredit);
    }

    if (lessThanThisAmount > 0.0) {
        criteria.add("amount", DBCriteria::less_than, lessThanThisAmount);
    }

    if (recurringOrNonRecurring.length() > 0) {
        if (recurringOrNonRecurring.compare("r") == 0) {
            criteria.add("recurring", true);
        }
        else if (recurringOrNonRecurring.compare("n") == 0) {
            criteria.add("recurring", false);
        }
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

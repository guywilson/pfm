#include <string>
#include <vector>
#include <string.h>

#include "strdate.h"
#include "money.h"
#include "transaction_criteria.h"

using namespace std;

string FindTransactionCriteriaBuilder::getParameter(int index) const {
    if (parameters.size() == 0) {
        throw pfm_error("Expected parameters but none were supplied");
    }
    else if (index >= parameters.size()) {
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

    while (i < parameters.size()) {
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

            for (int c = 0;c < ds.length();c++) {
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
        else if (parameter.compare("r") == 0) {
            string r = getParameter(i + 1);

            for (int c = 0;c < r.length();c++) {
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
            debitOrCredit = "DB";
            i++;
        }
        else if (parameter.compare("cr") == 0) {
            debitOrCredit = "CR";
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
    }
}

FindTransactionCriteriaBuilder::FindTransactionCriteriaBuilder(vector<string> & parms) {
    this->parameters = parms;
    parse();
}

string FindTransactionCriteriaBuilder::getCriteria() {
    string criteria;

    if (betweenTheseDatesList.size() == 1) {
        if (criteria.length() > 0) {
            criteria += " AND ";
        }

        criteria += "date > '" + betweenTheseDatesList[0].shortDate() + "'";
    }
    else if (betweenTheseDatesList.size() >= 2) {
        if (criteria.length() > 0) {
            criteria += " AND ";
        }

        criteria += "date >= '" + betweenTheseDatesList[0].shortDate() + "' AND date <= '" + betweenTheseDatesList[1].shortDate() + "'";
    }

    if (betweenTheseAmountList.size() == 1) {
        if (criteria.length() > 0) {
            criteria += " AND ";
        }

        criteria += "amount > " + betweenTheseAmountList[0].getRawStringValue();
    }
    else if (betweenTheseAmountList.size() >= 2) {
        if (criteria.length() > 0) {
            criteria += " AND ";
        }

        criteria += "amount >= " + betweenTheseAmountList[0].getRawStringValue() + " AND amount <= " + betweenTheseAmountList[1].getRawStringValue();
    }

    if (onTheseDatesList.size() > 0) {
        if (criteria.length() > 0) {
            criteria += " AND ";
        }

        bool hadFirstDate = false;
        criteria += "date in (";

        for (StrDate & date : onTheseDatesList) {
            if (hadFirstDate) {
                criteria += ", ";
            }

            criteria += "'" + date.shortDate() + "'";
            hadFirstDate = true;
        }

        criteria += ")";
    }

    if (withTheseCategoriesList.size() > 0) {
        if (criteria.length() > 0) {
            criteria += " AND ";
        }

        bool hadFirstCategory = false;
        criteria += "category_code in (";
        for (string & category : withTheseCategoriesList) {
            if (hadFirstCategory) {
                criteria += ", ";
            }

            criteria += "'" + category + "'";
            hadFirstCategory = true;
        }

        criteria += ")";
    }

    if (withThesePayeesList.size() > 0) {
        if (criteria.length() > 0) {
            criteria += " AND ";
        }

        bool hadFirstPayee = false;
        criteria += "payee_code in (";
        for (string & payee : withThesePayeesList) {
            if (hadFirstPayee) {
                criteria += ", ";
            }

            criteria += "'" + payee + "'";
            hadFirstPayee = true;
        }

        criteria += ")";
    }

    if (description.length() > 0) {
        if (criteria.length() > 0) {
            criteria += " AND ";
        }

        if (description.find_first_of('%') != string::npos || description.find_first_of('_') != string::npos) {
            criteria += "description LIKE '" + description + "'";
        }
        else {
            criteria += "description = '" + description + "'";
        }
    }

    if (reference.length() > 0) {
        if (criteria.length() > 0) {
            criteria += " AND ";
        }

        if (reference.find_first_of('%') != string::npos || reference.find_first_of('_') != string::npos) {
            criteria += "reference LIKE '" + reference + "'";
        }
        else {
            criteria += "reference = '" + reference + "'";
        }
    }

    if (debitOrCredit.length() > 0) {
        if (criteria.length() > 0) {
            criteria += " AND ";
        }

        criteria += "credit_debit = '" + debitOrCredit + "'";
    }

    if (lessThanThisAmount > 0.0) {
        if (criteria.length() > 0) {
            criteria += " AND ";
        }

        criteria += "amount < " + lessThanThisAmount.getRawStringValue();
    }

    if (recurringOrNonRecurring.length() > 0) {
        if (criteria.length() > 0) {
            criteria += " AND ";
        }

        if (recurringOrNonRecurring.compare("r") == 0) {
            criteria += "recurring_charge_id <> 0";
        }
        else if (recurringOrNonRecurring.compare("n") == 0) {
            criteria += "recurring_charge_id = 0";
        }
    }

    return criteria;
}

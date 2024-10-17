#include <string>
#include <vector>
#include <string.h>

#include "strdate.h"
#include "money.h"

using namespace std;

#ifndef __INCL_TRANSACTION_CRITERIA
#define __INCL_TRANSACTION_CRITERIA

/*
** Supported find criteria:
**
** D/dd-mm-yyyy - transactions after the specified date
** D/dd-mm-yyyy/D/dd-mm-yyyy - transactions between the two specified dates
** d/dd-mm-yyyy - transactions on this date (repeat for multiple)
** c/categoryCode - transactions with this category code (repeat for multiple)
** p/payeeCode - transactions with this payee code (repeat for multiple)
** ds/description - transactions with this description (accepts wildcards using '*' and '?')
** r/reference - transactions with this reference (accepts wildcards using '*' and '?')
** db - only debit transactions
** cr - only credit transactions
** rc/|r|n - recurring transaction filter - r = only recurring, n = only non-recurring
** a/amount - transactions for less than this amount
** A/amount - transactions for greater than this amount
** A/amount/A/amount - transactions for between these amounts
*/
class FindTransactionCriteriaBuilder {
    private:
        vector<string> parameters;

        vector<StrDate> betweenTheseDatesList;
        vector<StrDate> onTheseDatesList;
        vector<string> withTheseCategoriesList;
        vector<string> withThesePayeesList;
        vector<Money> betweenTheseAmountList;
        string description;
        string reference;
        string debitOrCredit;
        string recurringOrNonRecurring;
        Money lessThanThisAmount = 0.0;

        void parse();
        string getParameter(int index) const;

    public:
        FindTransactionCriteriaBuilder(vector<string> & parms);

        string getCriteria();
};

class AddTransactionCriteriaBuilder {
    private:
        vector<string> parameters;

        void parse();
        string getParameter(int index) const;

    public:
        StrDate date;
        string categoryCode;
        string payeeCode;
        string description;
        string reference;
        bool isCredit = false;
        Money amount = 0.0;

        AddTransactionCriteriaBuilder(vector<string> & parms);
};

#endif

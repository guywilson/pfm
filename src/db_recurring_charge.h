#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdint.h>

#include <sqlite3.h>

#include "db_category.h"
#include "db_payee.h"
#include "db_base.h"
#include "strdate.h"

using namespace std;

#ifndef __INCL_RECURRING_CHARGE
#define __INCL_RECURRING_CHARGE

class DBRecurringCharge : public DBBase {
    public:
        DBRecurringCharge() : DBBase() {
            clear();
        }

        void clear(void) {
            DBBase::clear();

            this->accountId = 0;
            this->categoryId = 0;
            this->payeeId = 0;

            this->category.clear();
            this->payee.clear();

            this->date = "";
            this->description = "";
            this->frequency = "";
            this->amount = 0.0;
        }

        void set(const DBRecurringCharge & src) {
            DBBase::set(src);

            this->accountId = src.accountId;
            this->categoryId = src.categoryId;
            this->payeeId = src.payeeId;

            this->date = src.date;
            this->description = src.description;
            this->frequency = src.frequency;
            this->amount = src.amount;
        }

        void print(void) {
            DBBase::print();

            cout << "AccountID: " << accountId << endl;
            cout << "CategoryID: " << categoryId << endl;
            cout << "PayeeID: " << payeeId << endl;

            cout << "Date: '" << date << "'" << endl;
            cout << "Description: '" << description << "'" << endl;
            cout << "Frequency: '" << frequency << "'" << endl;

            cout << fixed << setprecision(2);
            cout << "Amount: " << amount << endl;

            cout << "DBCategory (encapsulated object):" << endl;
            category.print();

            cout << "DBPayee (encapsulated object):" << endl;
            payee.print();
        }

        int getFrequencyValue(void) {
            return atoi(frequency.substr(0, frequency.length() - 1).c_str());
        }

        char getFrequencyUnit(void) {
            return frequency.substr(frequency.length() - 1, 1).c_str()[0];
        }

        void setNextPaymentDate(void) {
            StrDate     chargeDate(date);
            StrDate     dateToday;
            char        frequencyUnit;
            int         frequencyValue;

            frequencyValue = getFrequencyValue();
            frequencyUnit = getFrequencyUnit();

            if (chargeDate > dateToday || chargeDate == dateToday) {
                nextPaymentDate.assign(date);
            }
            else {
                switch (frequencyUnit) {
                    case 'y':
                        chargeDate.addYears(frequencyValue * (dateToday.year() - chargeDate.year()));

                        if (chargeDate.month() <= dateToday.month() && 
                            chargeDate.day() <= dateToday.day())
                        {
                            chargeDate.addYears(1);
                        }
                        break;

                    case 'm':
                        chargeDate.addMonths(frequencyValue * (dateToday.month() - chargeDate.month()));

                        if (chargeDate.day() <= dateToday.day()) {
                            chargeDate.addMonths(1);
                        }
                        break;

                    case 'w':
                        chargeDate.addWeeks(frequencyValue * ((dateToday.day() - chargeDate.day()) / 7));
                        break;

                    case 'd':
                        while (chargeDate.year() < dateToday.year() || chargeDate.month() < dateToday.month()) {
                            chargeDate.addDays(frequencyValue);
                        }
                        break;

                    default:
                        throw pfm_validation_error(
                                    pfm_error::buildMsg(
                                        "Invalid frequency unit '%c'", 
                                        frequencyUnit), 
                                    __FILE__, 
                                    __LINE__);
                        break;
                }

                nextPaymentDate.assign(chargeDate.shortDate());
            }
        }

        string                  nextPaymentDate;    // Not persistent

        sqlite3_int64           accountId;
        sqlite3_int64           categoryId;
        sqlite3_int64           payeeId;

        DBCategory              category;
        DBPayee                 payee;

        string                  date;
        string                  description;
        string                  frequency;
        double                  amount;
};

class DBRecurringChargeResult {
    public:
        DBRecurringChargeResult() {
            numRows = 0;
        }

        void clear() {
            numRows = 0;
            results.clear();
        }

        int                     numRows;

        vector<DBRecurringCharge> results;
};

#endif

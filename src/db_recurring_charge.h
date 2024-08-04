#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdint.h>

#include <sqlite3.h>

#include "db_category.h"
#include "db_payee.h"
#include "db_payment.h"
#include "strdate.h"

using namespace std;

#ifndef __INCL_RECURRING_CHARGE
#define __INCL_RECURRING_CHARGE

class DBRecurringCharge : public DBPayment {
    public:
        DBRecurringCharge() : DBPayment() {
            clear();
        }

        void clear(void) {
            DBPayment::clear();

            this->nextPaymentDate = "";
            this->frequency = "";
        }

        void set(const DBRecurringCharge & src) {
            DBPayment::set(src);

            this->nextPaymentDate = src.nextPaymentDate;
            this->frequency = src.frequency;
        }

        void print(void) {
            DBPayment::print();

            cout << "Frequency: '" << frequency << "'" << endl;
            cout << "NextPaymentDate: '" << nextPaymentDate << "'" << endl;
        }

        int getFrequencyValue(void) {
            return atoi(frequency.substr(0, frequency.length() - 1).c_str());
        }

        char getFrequencyUnit(void) {
            return frequency.substr(frequency.length() - 1, 1).c_str()[0];
        }

        bool isDue(void) {
            StrDate     paymentDate(nextPaymentDate);
            StrDate     today;

            if (paymentDate > today || paymentDate == today) {
                return true;
            }
            else {
                return false;
            }
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
        string                  frequency;
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

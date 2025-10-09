#include <iostream>
#include <string>
#include <time.h>
#include <sys/time.h>

#include "logger.h"
#include "pfm_error.h"

using namespace std;

#ifndef __INCL_STRDATE
#define __INCL_STRDATE

#define TIME_STAMP_BUFFER_LEN               64
#define DATE_STAMP_BUFFER_LEN               32
#define DATE_STRING_LENGTH                  10

class StrDate {
    private:
        string _date;
        const string _nullDate = "N/A";

        Logger & log = Logger::getInstance();

        class YMD {
            public:
                unsigned int year;
                unsigned int month;
                unsigned int day;

                YMD() {
                    year = 0;
                    month = 0;
                    day = 0;
                }
        };

        /*
        ** Calculates the number of seconds since Jan 1, 1970
        */
        time_t epoch();
        bool isLeapYear();

        int daysInMonth(int year, int month);

        YMD splitDate(const string & date);

        void validateDateString(const string & date);

        void set(YMD & date);
        
        static bool isYear(string & part);
        static bool isMonth(string & part);

        static bool isLeapYear(int year);
        static string getTimestamp(bool includems);

    public:
        typedef enum {
            sd_sunday = 0,
            sd_monday = 1,
            sd_tuesday = 2,
            sd_wednesday = 3,
            sd_thursday = 4,
            sd_friday = 5,
            sd_saturday = 6
        }
        sd_weekday;

        StrDate();
        StrDate(const StrDate & sd);
        StrDate(const string & sd);
        StrDate(const char * pszDate);
        StrDate(int year, int month, int day);

        void set(const string & date);
        void set(const char * date);
        void set(int year, int month, int day);

        void clear();
        
        bool isNull();

        static void setToday(const string & today);
        static void clearToday();

        static string today();
        static string getTimestamp();
        static string getTimestampToMicrosecond();
        static int getDaysInMonth(int year, int month);
        static bool isDateValid(const string & date);
        
        string shortDate() const;
        string getDisplayDate() const;
    
        int year() const;
        int month() const;
        int day() const;
        
        sd_weekday dayOfTheWeek() const;

        bool isSunday() const;
        bool isSaturday() const;
        bool isWeekend() const;

        bool isEpoch() const;

        int daysInMonth();
        StrDate firstDayInMonth();
        StrDate lastDayInMonth();

        StrDate addYears(int years);
        StrDate addMonths(int months);
        StrDate addWeeks(int weeks);
        StrDate addDays(int days);

        StrDate & operator=(const StrDate & rhs);
        StrDate & operator=(const string & rhs);
        StrDate & operator=(const char * rhs);

        bool operator==(StrDate & rh);
        bool operator==(string & rh);

        bool operator<(StrDate & rh);
        bool operator<(string & rh);

        bool operator<=(StrDate & rh);
        bool operator<=(string & rh);

        bool operator>(StrDate & rh);
        bool operator>(string & rh);

        bool operator>=(StrDate & rh);
        bool operator>=(string & rh);
};

#ifdef PFM_TEST_SUITE_ENABLED
class StrDateTest {
    private:
        static void testCreateString1() {
            string s = "2024-01-21";
            StrDate date(s);

            if (date.shortDate() != "2024-01-21") {
                throw pfm_error("testCreateString1: Test failed");
            }
            else {
                cout << "testCreateString1: Test passed" << endl;
            }
        }

        static void testCreateString2() {
            string s = "2024-02-29";
            StrDate date(s);

            if (date.shortDate() != "2024-02-29") {
                throw pfm_error("testCreateString2: Test failed");
            }
            else {
                cout << "testCreateString2: Test passed" << endl;
            }
        }

        static void testCreateString3() {
            string s = "2023-02-29";

            try {
                StrDate date(s);
            }
            catch (pfm_validation_error & e) {
                cout << "testCreateString3: Test passed" << endl;
                return;
            }

            throw pfm_error("testCreateString3: Test failed");
        }

        static void testCreateString4() {
            string s = "2023-04-31";

            try {
                StrDate date(s);
            }
            catch (pfm_validation_error & e) {
                cout << "testCreateString4: Test passed" << endl;
                return;
            }

            throw pfm_error("testCreateString4: Test failed");
        }

        static void testCreateString5() {
            string s = "23-11-01";

            try {
                StrDate date(s);
            }
            catch (pfm_validation_error & e) {
                cout << "testCreateString5: Test passed" << endl;
                return;
            }

            throw pfm_error("testCreateString5: Test failed");
        }

        static void testCreateChar1() {
            StrDate date("2024-01-21");

            if (date.shortDate() != "2024-01-21") {
                throw pfm_error("testCreateChar1: Test failed");
            }
            else {
                cout << "testCreateChar1: Test passed" << endl;
            }
        }

        static void testCreateChar2() {
            StrDate date("2024-02-29");

            if (date.shortDate() != "2024-02-29") {
                throw pfm_error("testCreateChar2: Test failed");
            }
            else {
                cout << "testCreateChar2: Test passed" << endl;
            }
        }

        static void testCreateChar3() {
            const char * s = "2023-02-29";

            try {
                StrDate date(s);
            }
            catch (pfm_validation_error & e) {
                cout << "testCreateChar3: Test passed" << endl;
                return;
            }

            throw pfm_error("testCreateChar3: Test failed");
        }

        static void testCreateChar4() {
            const char * s = "2023-04-31";

            try {
                StrDate date(s);
            }
            catch (pfm_validation_error & e) {
                cout << "testCreateChar4: Test passed" << endl;
                return;
            }

            throw pfm_error("testCreateChar4: Test failed");
        }

        static void testCreateChar5() {
            const char * s = "23-11-01";

            try {
                StrDate date(s);
            }
            catch (pfm_validation_error & e) {
                cout << "testCreateChar5: Test passed" << endl;
                return;
            }

            throw pfm_error("testCreateChar5: Test failed");
        }

        static void testCreateYMD1() {
            StrDate date(2024, 1, 21);

            if (date.shortDate() != "2024-01-21") {
                throw pfm_error("testCreateYMD1: Test failed");
            }
            else {
                cout << "testCreateYMD1: Test passed" << endl;
            }
        }

        static void testGetYear() {
            StrDate date(2024, 1, 21);

            if (date.year() != 2024) {
                throw pfm_error("testGetYear: Test failed");
            }
            else {
                cout << "testGetYear: Test passed" << endl;
            }
        }

        static void testGetMonth() {
            StrDate date(2024, 11, 21);

            if (date.month() != 11) {
                throw pfm_error("testGetMonth: Test failed");
            }
            else {
                cout << "testGetMonth: Test passed" << endl;
            }
        }

        static void testGetDay() {
            StrDate date(2024, 11, 21);

            if (date.day() != 21) {
                throw pfm_error("testGetDay: Test failed");
            }
            else {
                cout << "testGetDay: Test passed" << endl;
            }
        }

        static void testAddYears1() {
            StrDate date("2020-01-28");
            date.addYears(2);

            if (date.shortDate() != "2022-01-28") {
                throw pfm_error("testAddYears1: Test failed");
            }
            else {
                cout << "testAddYears1: Test passed" << endl;
            }
        }

        static void testAddYears2() {
            StrDate date("1970-03-24");
            date.addYears(54);

            if (date.shortDate() != "2024-03-24") {
                throw pfm_error("testAddYears2: Test failed");
            }
            else {
                cout << "testAddYears2: Test passed" << endl;
            }
        }

        static void testAddYears3() {
            StrDate date("1985-06-11");
            date.addYears(-3);

            if (date.shortDate() != "1982-06-11") {
                throw pfm_error("testAddYears3: Test failed");
            }
            else {
                cout << "testAddYears3: Test passed" << endl;
            }
        }

        static void testAddMonths1() {
            StrDate date("2020-01-28");
            date.addMonths(2);

            if (date.shortDate() != "2020-03-28") {
                throw pfm_error("testAddMonths1: Test failed");
            }
            else {
                cout << "testAddMonths1: Test passed" << endl;
            }
        }

        static void testAddMonths2() {
            StrDate date("1970-03-24");
            date.addMonths(27);

            if (date.shortDate() != "1972-06-24") {
                throw pfm_error("testAddMonths2: Test failed");
            }
            else {
                cout << "testAddMonths2: Test passed" << endl;
            }
        }

        static void testAddMonths3() {
            StrDate date("1985-06-11");
            date.addMonths(-3);

            if (date.shortDate() != "1985-03-11") {
                throw pfm_error("testAddMonths3: Test failed");
            }
            else {
                cout << "testAddMonths3: Test passed" << endl;
            }
        }

        static void testAddMonths4() {
            StrDate date("1985-06-11");
            date.addMonths(-24);

            if (date.shortDate() != "1983-06-11") {
                throw pfm_error("testAddMonths4: Test failed");
            }
            else {
                cout << "testAddMonths4: Test passed" << endl;
            }
        }

        static void testAddWeeks1() {
            StrDate date("2020-01-28");
            date.addWeeks(2);

            if (date.shortDate() != "2020-02-11") {
                throw pfm_error("testAddWeeks1: Test failed");
            }
            else {
                cout << "testAddWeeks1: Test passed" << endl;
            }
        }

        static void testAddWeeks2() {
            StrDate date("1970-03-24");
            date.addWeeks(27);

            if (date.shortDate() != "1970-09-29") {
                throw pfm_error("testAddWeeks2: Test failed");
            }
            else {
                cout << "testAddWeeks2: Test passed" << endl;
            }
        }

        static void testAddWeeks3() {
            StrDate date("1985-06-11");
            date.addWeeks(-3);

            if (date.shortDate() != "1985-05-21") {
                throw pfm_error("testAddWeeks3: Test failed");
            }
            else {
                cout << "testAddWeeks3: Test passed" << endl;
            }
        }

        static void testAddDays1() {
            StrDate date("2020-01-28");
            date.addDays(5);

            if (date.shortDate() != "2020-02-02") {
                throw pfm_error("testAddDays1: Test failed");
            }
            else {
                cout << "testAddDays1: Test passed" << endl;
            }
        }

        static void testAddDays2() {
            StrDate date("2023-03-24");
            date.addDays(365);

            if (date.shortDate() != "2024-03-23") {
                throw pfm_error("testAddDays2: Test failed");
            }
            else {
                cout << "testAddDays2: Test passed" << endl;
            }
        }

        static void testAddDays3() {
            StrDate date("1985-06-11");
            date.addDays(-3);

            if (date.shortDate() != "1985-06-08") {
                throw pfm_error("testAddDays3: Test failed");
            }
            else {
                cout << "testAddDays3: Test passed" << endl;
            }
        }

        static void testAddDays4() {
            StrDate date("2024-06-11");
            date.addDays(-365);

            if (date.shortDate() != "2023-06-12") {
                throw pfm_error("testAddDays4: Test failed");
            }
            else {
                cout << "testAddDays4: Test passed" << endl;
            }
        }

        static void testAssignment1() {
            StrDate d1("1985-06-11");
            StrDate d2 = d1;

            if (d2.shortDate() != "1985-06-11") {
                throw pfm_error("testAssignment1: Test failed");
            }
            else {
                cout << "testAssignment1: Test passed" << endl;
            }
        }

        static void testAssignment2() {
            string d1("1985-06-11");
            StrDate d2 = d1;

            if (d2.shortDate() != "1985-06-11") {
                throw pfm_error("testAssignment1: Test failed");
            }
            else {
                cout << "testAssignment1: Test passed" << endl;
            }
        }

        static void testIsEqual1() {
            StrDate d1("1985-06-11");
            StrDate d2("1985-06-11");

            if (!(d2 == d1)) {
                throw pfm_error("testIsEqual1: Test failed");
            }
            else {
                cout << "testIsEqual1: Test passed" << endl;
            }
        }

        static void testIsEqual2() {
            string d1("1985-06-11");
            StrDate d2("1985-06-11");

            if (!(d2 == d1)) {
                throw pfm_error("testIsEqual2: Test failed");
            }
            else {
                cout << "testIsEqual2: Test passed" << endl;
            }
        }

        static void testLessThan1() {
            StrDate d1("1985-06-11");
            StrDate d2("1985-06-10");

            if (!(d2 < d1)) {
                throw pfm_error("testLessThan1: Test failed");
            }
            else {
                cout << "testLessThan1: Test passed" << endl;
            }
        }

        static void testLessThan2() {
            string d1("1985-06-11");
            StrDate d2("1985-06-10");

            if (!(d2 < d1)) {
                throw pfm_error("testLessThan2: Test failed");
            }
            else {
                cout << "testLessThan2: Test passed" << endl;
            }
        }

        static void testLessThanOrEqual1() {
            StrDate d1("1985-06-11");
            StrDate d2("1985-06-10");

            if (!(d2 <= d1)) {
                throw pfm_error("testLessThanOrEqual1: Test failed");
            }
            else {
                cout << "testLessThanOrEqual1: Test passed" << endl;
            }
        }

        static void testLessThanOrEqual2() {
            string d1("1985-06-11");
            StrDate d2("1985-06-11");

            if (!(d2 <= d1)) {
                throw pfm_error("testLessThanOrEqual2: Test failed");
            }
            else {
                cout << "testLessThanOrEqual2: Test passed" << endl;
            }
        }

        static void testGreaterThan1() {
            StrDate d1("1985-06-10");
            StrDate d2("1985-06-11");

            if (!(d2 > d1)) {
                throw pfm_error("testGreaterThan1: Test failed");
            }
            else {
                cout << "testGreaterThan1: Test passed" << endl;
            }
        }

        static void testGreaterThan2() {
            string d1("1985-06-10");
            StrDate d2("1985-06-11");

            if (!(d2 > d1)) {
                throw pfm_error("testGreaterThan2: Test failed");
            }
            else {
                cout << "testGreaterThan2: Test passed" << endl;
            }
        }

        static void testGreaterThanOrEqual1() {
            StrDate d1("1985-06-10");
            StrDate d2("1985-06-11");

            if (!(d2 >= d1)) {
                throw pfm_error("testGreaterThanOrEqual1: Test failed");
            }
            else {
                cout << "testGreaterThanOrEqual1: Test passed" << endl;
            }
        }

        static void testGreaterThanOrEqual2() {
            string d1("1985-06-11");
            StrDate d2("1985-06-11");

            if (!(d2 >= d1)) {
                throw pfm_error("testGreaterThanOrEqual2: Test failed");
            }
            else {
                cout << "testGreaterThanOrEqual2: Test passed" << endl;
            }
        }

    public:
        static void run() {
            int numTestsPassed = 0;
            int numTestsFailed = 0;

            try {
                testCreateString1();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testCreateString2();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testCreateString3();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testCreateString4();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testCreateString5();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testCreateChar1();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testCreateChar2();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testCreateChar3();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testCreateChar4();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testCreateChar5();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testCreateYMD1();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testGetYear();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testGetMonth();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testGetYear();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testAddYears1();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testAddYears2();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testAddYears3();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testAddMonths1();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testAddMonths2();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testAddMonths3();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testAddMonths4();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testAddWeeks1();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testAddWeeks2();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testAddWeeks3();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testAddDays1();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testAddDays2();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testAddDays3();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testAddDays4();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testAssignment1();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testAssignment2();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testIsEqual1();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testIsEqual2();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testLessThan1();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testLessThan2();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testLessThanOrEqual1();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testLessThanOrEqual2();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testGreaterThan1();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testGreaterThan2();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testGreaterThanOrEqual1();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                testGreaterThanOrEqual2();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            cout << "Tests passed: " << numTestsPassed << ", tests failed: " << numTestsFailed << endl;
        }
};
#endif
#endif

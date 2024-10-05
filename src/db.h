#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdint.h>

#include <pthread.h>
#include <sqlcipher/sqlite3.h>

#include "pfm_error.h"
#include "logger.h"

using namespace std;

#ifndef __INCL_DB
#define __INCL_DB

#define SQLITE_ERROR_BUFFER_LEN                     512
#define SQL_STATEMENT_BUFFER_LEN                    512
#define SQL_ROW_LIMIT                                80

typedef enum {
    sort_descending,
    sort_ascending
}
db_sort_t;

typedef sqlite3_int64 pfm_id_t;

class DBColumn {
    private:
        string name;
        string value;

    public:
        DBColumn(const char * name, const char * value) {
            this->name = name;
            
            /*
            ** value will be NULL for blank NULLable columns...
            */
            if (value != NULL) {
                this->value = value;
            }
        }

        string getName() {
            return name;
        }

        string getValue() {
            return value;
        }

        double getDoubleValue() {
            return strtod(getValue().c_str(), NULL);
        }

        long getIntValue() {
            return strtol(getValue().c_str(), NULL, 10);
        }

        unsigned long getUnsignedIntValue() {
            return strtoul(getValue().c_str(), NULL, 10);
        }

        bool getBoolValue() {
            return ((value[0] == 'Y' || value[0] == 'y') ? true : false);
        }

        pfm_id_t getIDValue() {
            return strtoll(getValue().c_str(), NULL, 10);
        }
};

class DBRow {
    private:
        vector<DBColumn>  columns;

    public:
        DBRow(int numColumns, vector<DBColumn> & columnVector) {
            for (size_t i = 0;i < columnVector.size();i++) {
                columns.push_back(columnVector[i]);
            }
        }

        size_t getNumColumns() {
            return columns.size();
        }

        DBColumn getColumnAt(int i) {
            return columns[i];
        }
};

class PFM_DB {
    public:
        static PFM_DB & getInstance() {
            static PFM_DB instance;
            return instance;
        }

    private:
        sqlite3 * dbHandle;
        bool isTransactionActive;
        pthread_mutex_t mutex;
        Logger & log = Logger::getInstance();

        PFM_DB() {
            isTransactionActive = false;
        }

        void _executeSQLNoCallback(const char * sql);
        void _executeSQLCallback(const char * sql, vector<DBRow> * rows);

        bool getIsTransactionActive();
        void setIsTransactionActive();
        void clearIsTransactionActive();

        void createSchema();

        void createDefaultCategories();
        void createDefaultConfig();
        void createCurrencies();

    public:
        ~PFM_DB();

        bool open(string dbName);

        void createTable(const char * sql);
        void createView(const char * sql);

        int executeSelect(const char * statement, vector<DBRow> * rows);
        
        pfm_id_t executeInsert(const char * statement);

        void executeUpdate(const char * statement);
        void executeDelete(const char * statement);

        void begin();
        void commit();
        void rollback();
};

#ifdef PFM_TEST_SUITE_ENABLED
class EncryptionTest {
    private:
        static void test1() {
            string plainText = "This is a description!";

            string cipherText = DBColumn::encrypt(plainText);
            string decryptedText = DBColumn::decrypt(cipherText);

            cout << "'" << plainText << "' > '" << cipherText << "' > '" << decryptedText << "'" << endl;
            
            if (decryptedText.compare(plainText) != 0) {
                throw pfm_error("test1(): Test failed");
            }
            else {
                cout << "test1(): Test passed" << endl;
            }
        }

        static void test2() {
            string plainText = "    AAAAZZZZzzzz99990";

            string cipherText = DBColumn::encrypt(plainText);
            string decryptedText = DBColumn::decrypt(cipherText);

            cout << "'" << plainText << "' > '" << cipherText << "' > '" << decryptedText << "'" << endl;
            
            if (decryptedText.compare(plainText) != 0) {
                throw pfm_error("test2(): Test failed");
            }
            else {
                cout << "test2(): Test passed" << endl;
            }
        }

        static void test3() {
            string plainText = " !#$&()[]{}*+-/0123456789:;.,<=>?@^_|~ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

            string cipherText = DBColumn::encrypt(plainText);
            string decryptedText = DBColumn::decrypt(cipherText);

            cout << "'" << plainText << "' > '" << cipherText << "' > '" << decryptedText << "'" << endl;
            
            if (decryptedText.compare(plainText) != 0) {
                throw pfm_error("test3(): Test failed");
            }
            else {
                cout << "test3(): Test passed" << endl;
            }
        }

        static void test4() {
            string plainText = "The quick brown fox jumped over the lazy dog";

            string cipherText = DBColumn::encrypt(plainText);
            string decryptedText = DBColumn::decrypt(cipherText);

            cout << "'" << plainText << "' > '" << cipherText << "' > '" << decryptedText << "'" << endl;
            
            if (decryptedText.compare(plainText) != 0) {
                throw pfm_error("test4(): Test failed");
            }
            else {
                cout << "test4(): Test passed" << endl;
            }
        }

        static void test5() {
            string plainText = "a";

            string cipherText = DBColumn::encrypt(plainText);
            string decryptedText = DBColumn::decrypt(cipherText);

            cout << "'" << plainText << "' > '" << cipherText << "' > '" << decryptedText << "'" << endl;
            
            if (decryptedText.compare(plainText) != 0) {
                throw pfm_error("test5(): Test failed");
            }
            else {
                cout << "test5(): Test passed" << endl;
            }
        }

        static void test6() {
            string plainText = "285.23";

            string cipherText = DBColumn::encrypt(plainText);
            string decryptedText = DBColumn::decrypt(cipherText);

            cout << "'" << plainText << "' > '" << cipherText << "' > '" << decryptedText << "'" << endl;
            
            if (decryptedText.compare(plainText) != 0) {
                throw pfm_error("test6(): Test failed");
            }
            else {
                cout << "test6(): Test passed" << endl;
            }
        }

        static void test7() {
            string plainText = "-252.78";

            string cipherText = DBColumn::encrypt(plainText);
            string decryptedText = DBColumn::decrypt(cipherText);

            cout << "'" << plainText << "' > '" << cipherText << "' > '" << decryptedText << "'" << endl;
            
            if (decryptedText.compare(plainText) != 0) {
                throw pfm_error("test7(): Test failed");
            }
            else {
                cout << "test7(): Test passed" << endl;
            }
        }

        static void test8() {
            string plainText = "2024-08-16";

            string cipherText = DBColumn::encrypt(plainText);
            string decryptedText = DBColumn::decrypt(cipherText);

            cout << "'" << plainText << "' > '" << cipherText << "' > '" << decryptedText << "'" << endl;
            
            if (decryptedText.compare(plainText) != 0) {
                throw pfm_error("test8(): Test failed");
            }
            else {
                cout << "test8(): Test passed" << endl;
            }
        }

        static void test9() {
            string plainText = "2024-08-16 16:04:32";

            string cipherText = DBColumn::encrypt(plainText);
            string decryptedText = DBColumn::decrypt(cipherText);

            cout << "'" << plainText << "' > '" << cipherText << "' > '" << decryptedText << "'" << endl;
            
            if (decryptedText.compare(plainText) != 0) {
                throw pfm_error("test9(): Test failed");
            }
            else {
                cout << "test9(): Test passed" << endl;
            }
        }

    public:
        static void run() {
            Key & key = Key::getInstance();
            
            string password = "The quick brown fox jumped over the lazy dog";
            key.generate(password);

            int numTestsPassed = 0;
            int numTestsFailed = 0;

            try {
                test1();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                test2();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                test3();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                test4();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                test5();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                test6();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                test7();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                test8();
                numTestsPassed++;
            }
            catch (pfm_error & e) {
                cout << e.what() << endl;
                numTestsFailed++;
            }

            try {
                test9();
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

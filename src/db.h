#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdint.h>

#include <pthread.h>
#include <sqlite3.h>

#include "pfm_error.h"
#include "logger.h"
#include "keymgr.h"

using namespace std;

#ifndef __INCL_DB
#define __INCL_DB

#define SQLITE_ERROR_BUFFER_LEN                     512
#define SQL_STATEMENT_BUFFER_LEN                    512
#define SQL_ROW_LIMIT                                50

typedef enum {
    sort_descending,
    sort_ascending
}
db_sort_t;

typedef sqlite3_int64 pfm_id_t;

static const char * permittedCharset = " !#$&()[]{}*+-/0123456789:;.,<=>?@^_|~ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

class DBColumn {
    private:
        string name;
        string value;

        static uint8_t encodeCharFromASCII(char ch) {
            for (int i = 0;i < strlen(permittedCharset);i++) {
                if (ch == permittedCharset[i]) {
                    return i;
                }
            }

            throw pfm_validation_error(pfm_error::buildMsg("Cannot encode char [0x%02X]'%c' as it is not a permitted character", ch, ch));
        }

        static char decodeCharToASCII(uint8_t ch) {
            return permittedCharset[ch];
        }
        
        static void isCharPermitted(char ch) {
            encodeCharFromASCII(ch);
        }

        static char encryptChar(char ch, int keyBits) {
            int encodedChar = (int)DBColumn::encodeCharFromASCII(ch);
            int encryptedChar = encodedChar + keyBits;

            if (encryptedChar >= strlen(permittedCharset)) {
                encryptedChar = encryptedChar % strlen(permittedCharset);
            }

            char out = decodeCharToASCII((char)encryptedChar);

            // printf("Encrypted '%c' encoded to 0x%02X with key bits 0x%02X to '%c'\n", ch, (uint8_t)encodedChar, (uint8_t)keyBits, out);

            return out;
        }

        static char decryptChar(char ch, int keyBits) {
            int encodedChar = (int)DBColumn::encodeCharFromASCII(ch);
            int decryptedChar = encodedChar - keyBits;

            while (decryptedChar < 0) {
                decryptedChar += strlen(permittedCharset);
            }

            char out = decodeCharToASCII((char)decryptedChar);

            // printf("Decrypted '%c' encoded to 0x%02X with key bits 0x%02X to '%c'\n", ch, (uint8_t)encodedChar, (uint8_t)keyBits, out);

            return out;
        }

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
            return strtod(value.c_str(), NULL);
        }

        long getIntValue() {
            return strtol(value.c_str(), NULL, 10);
        }

        unsigned long getUnsignedIntValue() {
            return strtoul(value.c_str(), NULL, 10);
        }

        bool getBoolValue() {
            return ((value[0] == 'Y' || value[0] == 'y') ? true : false);
        }

        pfm_id_t getIDValue() {
            return strtoll(value.c_str(), NULL, 10);
        }

        static void validateStringValue(const string & value) {
            for (int i = 0;i < value.length();i++) {
                try {
                    isCharPermitted(value[i]);
                }
                catch (pfm_validation_error & e) {
                    throw pfm_error(pfm_error::buildMsg("Invalid char '%c' in value '%s' at index %d", value[i], value.c_str(), i));
                }
            }
        }

        static string encrypt(string & value) {
            string out = value;

            Key & key = Key::getInstance();

            for (int i = 0;i < value.length();i++) {
                out[i] = encryptChar(value[i], (int)key.getBits(i));
            }

            return out;
        }

        static string decrypt(string & value) {
            string out = value;

            Key & key = Key::getInstance();

            for (int i = 0;i < value.length();i++) {   
                out[i] = decryptChar(value[i], (int)key.getBits(i));
            }

            return out;
        }
};

class DBRow {
    private:
        vector<DBColumn>  columns;

    public:
        DBRow(int numColumns, vector<DBColumn> & columnVector) {
            for (int i = 0;i < columnVector.size();i++) {
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

    public:
        static void run() {
            Key & key = Key::getInstance();
            
            key.generate("The quick brown fox jumped over the lazy dog");

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

            cout << "Tests passed: " << numTestsPassed << ", tests failed: " << numTestsFailed << endl;
        }
};
#endif
#endif

#include <iostream>
#include <stdint.h>
#include <gcrypt.h>

using namespace std;

#ifndef __INCL_KEYMGR
#define __INCL_KEYMGR

#define NUM_WIPE_CYCLES                     7
#define KEY_BIT_SIZE                        6
#define KEY_BUFFER_LENGTH                  32

class Key {
    public:
        static Key & getInstance() {
            static Key instance;
            return instance;
        }

    private:
        uint8_t keyBuffer[KEY_BUFFER_LENGTH];

        Key() {
            wipeKey();
        }

        void wipeKey() {
            uint8_t wipeChar = 0x00;

            for (int j = 0;j < NUM_WIPE_CYCLES;j++) {
                wipeChar = (wipeChar == 0x00 ? 0xFF : 0x00);

                for (int i = 0;i < KEY_BUFFER_LENGTH;i++) {
                    keyBuffer[i] = wipeChar;
                }
            }
        }

    public:
        ~Key() {
            wipeKey();
        }

        void generate(const char * password) {
            gcry_md_hash_buffer(GCRY_MD_SHA3_256, keyBuffer, password, strlen(password));
        }

        uint8_t getBits(int index) {
            uint8_t result = 0;

            for (int i = 0; i < KEY_BIT_SIZE; i++) {
                int currentBitIndex = index + i;
                int currentByteIndex = currentBitIndex / 8;
                int currentBitOffset = currentBitIndex % 8;

                uint8_t bit = (keyBuffer[currentByteIndex] >> (7 - currentBitOffset)) & 1;

                result = (result << 1) | bit;
            }

            return result;
        }
};

#endif

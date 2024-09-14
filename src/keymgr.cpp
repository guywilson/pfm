#include <string>
#include <stdio.h>
#include <stdint.h>

#include <gcrypt.h>

#include "pfm_error.h"
#include "keymgr.h"

using namespace std;

void hexDump(void * buffer, uint32_t bufferLen) {
    int         i;
    int         j = 0;
    uint8_t *   buf;
    static char szASCIIBuf[17];

    buf = (uint8_t *)buffer;

    for (i = 0;i < bufferLen;i++) {
        if ((i % 16) == 0) {
            if (i != 0) {
                szASCIIBuf[j] = 0;
                j = 0;

                printf("  |%s|", szASCIIBuf);
            }
                
            printf("\n%08X\t", i);
        }

        if ((i % 2) == 0 && (i % 16) > 0) {
            printf(" ");
        }

        printf("%02X", buf[i]);
        szASCIIBuf[j++] = isprint(buf[i]) ? buf[i] : '.';
    }

    /*
    ** Print final ASCII block...
    */
    szASCIIBuf[j] = 0;
    printf("  |%s|\n", szASCIIBuf);
}

Key::Key() {
    wipeKey();
}

Key::~Key() {
    wipeKey();
}

void Key::wipeKey() {
    uint8_t wipeChar = 0x00;

    for (int j = 0;j < NUM_WIPE_CYCLES;j++) {
        wipeChar = (wipeChar == 0x00 ? 0xFF : 0x00);

        for (int i = 0;i < KEY_BUFFER_LENGTH;i++) {
            keyBuffer[i] = wipeChar;
        }
    }
}

void Key::generate(const string & password) {
    gcry_md_hash_buffer(GCRY_MD_SHA3_512, keyBuffer, password.c_str(), password.length());
#ifdef PFM_TEST_SUITE_ENABLED
    hexDump(keyBuffer, KEY_BUFFER_LENGTH);
#endif
}

uint8_t Key::getBits(int index) {
    uint8_t result = 0;

    for (int i = 0; i < KEY_BIT_SIZE; i++) {
        int currentBitIndex = index + i;
        int currentByteIndex = currentBitIndex / 8;
        int currentBitOffset = currentBitIndex % 8;

        if (currentByteIndex >= KEY_BUFFER_LENGTH) {
            throw pfm_fatal(
                    pfm_error::buildMsg(
                        "Key index out of range: got %d, max %d", 
                        currentByteIndex, 
                        KEY_BUFFER_LENGTH), 
                    __FILE__, 
                    __LINE__);
        }

        uint8_t bit = (keyBuffer[currentByteIndex] >> (7 - currentBitOffset)) & 1;

        result = (result << 1) | bit;
    }

    return result;
}

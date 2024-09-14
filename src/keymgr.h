#include <stdint.h>

using namespace std;

#ifndef __INCL_KEYMGR
#define __INCL_KEYMGR

#define NUM_WIPE_CYCLES                     7
#define KEY_BIT_SIZE                        7
#define KEY_BUFFER_LENGTH                  64

class Key {
    public:
        static Key & getInstance() {
            static Key instance;
            return instance;
        }

    private:
        uint8_t keyBuffer[KEY_BUFFER_LENGTH];

        Key();
        void wipeKey();

    public:
        ~Key();

        void generate(const char * password);
        uint8_t getBits(int index);
};

#endif

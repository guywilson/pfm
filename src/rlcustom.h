#include <stdio.h>
#include <stdint.h>

#ifndef __INCL_RLCUSTOM
#define __INCL_RLCUSTOM

#define DEFAULT_LINE_LENGTH                     128
#define CANCEL_READLINE_KEYSEQ                  "\\C-x"

class rl_utils {
    public:
        static void setup();
        static uint16_t getLineLength();
        static void setLineLength(uint16_t length);
};

#endif
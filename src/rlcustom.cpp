#include <stdio.h>
#include <stdint.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "pfm_error.h"
#include "rlcustom.h"

uint16_t _lineLength = DEFAULT_LINE_LENGTH;


static int handle_cancel_key(int count, int key) {
    throw pfm_field_cancel_error();
}

static int limited_insert(int count, int key) {
    char insertBuffer[2] = {0, 0};

    if (rl_end >= rl_utils::getLineLength()) {
        rl_ding();
        return 0;
    }
    
    // Insert the character normally
    insertBuffer[0] = (char)key;
    rl_insert_text(insertBuffer);
    return 0;
}

void rl_utils::setup() {
    // Bind printable characters to limited_insert
    for (int c = 32; c <= 126; c++) {
        rl_bind_key(c, limited_insert);
    }

    rl_bind_key('\t', rl_complete);
    rl_bind_keyseq(CANCEL_READLINE_KEYSEQ, handle_cancel_key);
    using_history();
}

uint16_t rl_utils::getLineLength() {
    return _lineLength;
}

void rl_utils::setLineLength(uint16_t length) {
    _lineLength = length;
}

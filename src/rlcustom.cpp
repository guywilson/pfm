#include <string>
#include <vector>
#include <unordered_map>
#include <stdio.h>
#include <stdint.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "pfm_error.h"
#include "rlcustom.h"

using namespace std;

uint16_t _lineLength = DEFAULT_LINE_LENGTH;
unordered_map<string, string> shortcuts;

static int handle_cancel_key(int count, int key) {
    throw pfm_field_cancel_error();
}

static int expand_shortcut(int count, int key) {
    int cursor = rl_point;

    int start = cursor - 1;
    while (start >= 0 && !isspace(static_cast<unsigned char>(rl_line_buffer[start]))) {
        --start;
    }
    start++; // move to first char of the word

    int end = cursor;

    if (start < end) {
        string word(rl_line_buffer + start, rl_line_buffer + end);

        auto it = shortcuts.find(word);
        if (it != shortcuts.end()) {
            const string & replacement = it->second;

            // Remove the original word
            rl_delete_text(start, end);
            rl_point = start; // move cursor to start of word

            // Insert the replacement text
            rl_insert_text(replacement.c_str());
            rl_point = start + replacement.size();
        }
    }

    rl_redisplay();

    return 0;
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

void rl_utils::loadShortcuts(vector<pair<string, string>> & shortcutPairs) {
    shortcuts.clear();
    
    for (pair<string, string> & p : shortcutPairs) {
        shortcuts.insert({p.first, p.second});
    }
}

void rl_utils::setup() {
    for (int c = 32; c <= 126; c++) {
        rl_bind_key(c, limited_insert);
    }

    rl_bind_key('\t', rl_complete);
    rl_bind_key('\\', expand_shortcut);
    rl_bind_keyseq(CANCEL_READLINE_KEYSEQ, handle_cancel_key);
    using_history();
}

uint16_t rl_utils::getLineLength() {
    return _lineLength;
}

void rl_utils::setLineLength(uint16_t length) {
    _lineLength = length;
}

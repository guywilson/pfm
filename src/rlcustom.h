#pragma once

#include <string>
#include <vector>
#include <stdio.h>
#include <stdint.h>


#define DEFAULT_LINE_LENGTH                     384
#define CANCEL_READLINE_KEYSEQ                  "\\C-x"

class rl_utils {
    public:
        static void loadShortcuts(std::vector<std::pair<std::string, std::string>> & shortcutPairs);
        static void setup();
        static uint16_t getLineLength();
        static void setLineLength(uint16_t length);
};


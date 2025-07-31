#include <iostream>
#include <string>
#include <stdint.h>

#include <sys/ioctl.h>

using namespace std;

#ifndef __INCL_TERMINAL
#define __INCL_TERMINAL

#define TERMINAL_MIN_WIDTH                   110
#define TERMINAL_MIN_HEIGHT                   24

class Terminal {
    public:
        static Terminal & getInstance() {
            static Terminal instance;
            return instance;
        }

    private:
        Terminal() {}

    public:
        uint16_t getWidth() {
            struct winsize w;
            ioctl(0, TIOCGWINSZ, &w);
            return w.ws_col;
        }

        uint16_t getHeight() {
            struct winsize w;
            ioctl(0, TIOCGWINSZ, &w);
            return w.ws_row;
        }
};

#endif

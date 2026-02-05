#include <iostream>
#include <string>
#include <stdint.h>

#include <sys/ioctl.h>

using namespace std;

#ifndef __INCL_TERMINAL
#define __INCL_TERMINAL

#define TERMINAL_MIN_WIDTH                    80U
#define TERMINAL_MIN_HEIGHT                   20U

class Terminal {
    public:
        static unsigned long getWidth() {
            struct winsize w;
            ioctl(0, TIOCGWINSZ, &w);
            return w.ws_col;
        }

        static unsigned long getHeight() {
            struct winsize w;
            ioctl(0, TIOCGWINSZ, &w);
            return w.ws_row;
        }

        static inline unsigned long fullWidthMinimum() {
            return TERMINAL_MIN_WIDTH;
        }

        static inline unsigned long fullHeightMinimum() {
            return TERMINAL_MIN_HEIGHT;
        }
};

#endif

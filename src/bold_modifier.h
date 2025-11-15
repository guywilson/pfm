#include <iostream>

using namespace std;

#ifndef __INCL_BOLD_MODIFIER
#define __INCL_BOLD_MODIFIER

#if defined(__APPLE__) || defined(__unix__)
const char ESC_CHAR = '\x1B';
inline std::ostream& bold_on(std::ostream& os) {
    return os << ESC_CHAR << "[1m";
}

inline std::ostream& bold_off(std::ostream& os) {
    return os << ESC_CHAR << "[0m";
}
#else
inline std::ostream& bold_on(std::ostream& os) {
    return os << "";
}

inline std::ostream& bold_off(std::ostream& os) {
    return os << "";
}
#endif

#endif

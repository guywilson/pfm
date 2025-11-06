#include "version.h"

#define __BDATE__      "2025-11-06 11:54:06"
#define __BVERSION__   "1.9.029"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

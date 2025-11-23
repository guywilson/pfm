#include "version.h"

#define __BDATE__      "2025-11-23 21:02:31"
#define __BVERSION__   "2.0.029"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

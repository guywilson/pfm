#include "version.h"

#define __BDATE__      "2025-10-28 21:57:36"
#define __BVERSION__   "1.9.022"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

#include "version.h"

#define __BDATE__      "2026-07-23 08:09:17"
#define __BVERSION__   "2.1.097"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

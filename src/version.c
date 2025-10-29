#include "version.h"

#define __BDATE__      "2025-10-29 20:26:58"
#define __BVERSION__   "1.9.024"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

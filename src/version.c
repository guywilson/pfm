#include "version.h"

#define __BDATE__      "2025-10-24 20:45:36"
#define __BVERSION__   "1.9.011"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

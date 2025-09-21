#include "version.h"

#define __BDATE__      "2025-09-21 18:16:41"
#define __BVERSION__   "1.8.039"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

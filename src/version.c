#include "version.h"

#define __BDATE__      "2025-10-25 16:31:23"
#define __BVERSION__   "1.9.016"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

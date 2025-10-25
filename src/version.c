#include "version.h"

#define __BDATE__      "2025-10-25 13:11:56"
#define __BVERSION__   "1.9.013"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

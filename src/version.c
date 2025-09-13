#include "version.h"

#define __BDATE__      "2025-09-13 14:16:32"
#define __BVERSION__   "1.8.035"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

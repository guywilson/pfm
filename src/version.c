#include "version.h"

#define __BDATE__      "2025-11-10 15:42:57"
#define __BVERSION__   "2.0.006"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

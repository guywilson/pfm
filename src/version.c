#include "version.h"

#define __BDATE__      "2026-03-13 08:16:16"
#define __BVERSION__   "2.1.082"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

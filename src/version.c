#include "version.h"

#define __BDATE__      "2026-08-21 16:15:13"
#define __BVERSION__   "2.3.006"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

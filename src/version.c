#include "version.h"

#define __BDATE__      "2026-08-20 23:25:53"
#define __BVERSION__   "2.3.004"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

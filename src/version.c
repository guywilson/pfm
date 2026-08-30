#include "version.h"

#define __BDATE__      "2026-08-30 21:01:24"
#define __BVERSION__   "2.4.005"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

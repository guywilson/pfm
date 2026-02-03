#include "version.h"

#define __BDATE__      "2026-02-03 22:14:13"
#define __BVERSION__   "2.1.051"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

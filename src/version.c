#include "version.h"

#define __BDATE__      "2026-06-06 17:40:23"
#define __BVERSION__   "2.1.087"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

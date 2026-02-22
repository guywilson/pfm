#include "version.h"

#define __BDATE__      "2026-02-22 16:42:18"
#define __BVERSION__   "2.1.072"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

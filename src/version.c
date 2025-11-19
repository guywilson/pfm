#include "version.h"

#define __BDATE__      "2025-11-19 16:35:56"
#define __BVERSION__   "2.0.022"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

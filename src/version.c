#include "version.h"

#define __BDATE__      "2025-09-14 09:34:19"
#define __BVERSION__   "1.8.037"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

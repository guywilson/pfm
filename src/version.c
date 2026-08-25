#include "version.h"

#define __BDATE__      "2026-08-25 13:42:43"
#define __BVERSION__   "2.3.013"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

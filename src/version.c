#include "version.h"

#define __BDATE__      "2026-02-02 12:01:06"
#define __BVERSION__   "2.1.043"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

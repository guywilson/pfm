#include "version.h"

#define __BDATE__      "2024-11-27 18:29:23"
#define __BVERSION__   "1.0.043"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

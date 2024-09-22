#include "version.h"

#define __BDATE__      "2024-09-22 11:34:23"
#define __BVERSION__   "0.2.002"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

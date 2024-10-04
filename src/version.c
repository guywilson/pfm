#include "version.h"

#define __BDATE__      "2024-10-04 10:09:16"
#define __BVERSION__   "0.8.015"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

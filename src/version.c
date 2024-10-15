#include "version.h"

#define __BDATE__      "2024-10-15 12:25:59"
#define __BVERSION__   "0.8.037"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

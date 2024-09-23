#include "version.h"

#define __BDATE__      "2024-09-23 10:02:23"
#define __BVERSION__   "0.4.003"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

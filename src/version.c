#include "version.h"

#define __BDATE__      "2024-09-29 20:59:23"
#define __BVERSION__   "0.8.006"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

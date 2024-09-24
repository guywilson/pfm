#include "version.h"

#define __BDATE__      "2024-09-24 13:51:15"
#define __BVERSION__   "0.5.002"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

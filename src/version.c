#include "version.h"

#define __BDATE__      "2024-09-28 22:54:58"
#define __BVERSION__   "0.8.003"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

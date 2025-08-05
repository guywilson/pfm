#include "version.h"

#define __BDATE__      "2025-08-05 15:23:23"
#define __BVERSION__   "1.8.005"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

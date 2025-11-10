#include "version.h"

#define __BDATE__      "2025-11-10 15:39:54"
#define __BVERSION__   "2.0.005"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

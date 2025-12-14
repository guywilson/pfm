#include "version.h"

#define __BDATE__      "2025-12-14 12:58:25"
#define __BVERSION__   "2.1.005"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

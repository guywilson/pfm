#include "version.h"

#define __BDATE__      "2025-07-04 13:31:24"
#define __BVERSION__   "1.1.005"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

#include "version.h"

#define __BDATE__      "2025-08-03 17:48:38"
#define __BVERSION__   "1.7.005"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

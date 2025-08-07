#include "version.h"

#define __BDATE__      "2025-08-07 17:52:53"
#define __BVERSION__   "1.8.011"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

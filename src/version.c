#include "version.h"

#define __BDATE__      "2025-08-31 20:53:53"
#define __BVERSION__   "1.8.024"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

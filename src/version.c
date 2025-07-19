#include "version.h"

#define __BDATE__      "2025-07-19 18:13:57"
#define __BVERSION__   "1.2.024"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

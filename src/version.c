#include "version.h"

#define __BDATE__      "2025-08-30 23:32:57"
#define __BVERSION__   "1.8.020"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

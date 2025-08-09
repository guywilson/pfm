#include "version.h"

#define __BDATE__      "2025-08-09 18:08:02"
#define __BVERSION__   "1.8.014"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

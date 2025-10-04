#include "version.h"

#define __BDATE__      "2025-10-04 13:47:08"
#define __BVERSION__   "1.8.040"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

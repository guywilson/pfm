#include "version.h"

#define __BDATE__      "2025-07-20 11:32:43"
#define __BVERSION__   "1.3.001"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

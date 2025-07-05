#include "version.h"

#define __BDATE__      "2025-07-05 16:28:38"
#define __BVERSION__   "1.2.006"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

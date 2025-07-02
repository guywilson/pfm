#include "version.h"

#define __BDATE__      "2025-07-02 11:46:16"
#define __BVERSION__   "1.1.001"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

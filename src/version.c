#include "version.h"

#define __BDATE__      "2025-08-02 17:09:04"
#define __BVERSION__   "1.7.001"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

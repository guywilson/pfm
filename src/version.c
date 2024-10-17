#include "version.h"

#define __BDATE__      "2024-10-17 18:53:46"
#define __BVERSION__   "1.0.039"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

#include "version.h"

#define __BDATE__      "2024-09-23 09:40:18"
#define __BVERSION__   "0.4.002"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

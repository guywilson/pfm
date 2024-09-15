#include "version.h"

#define __BDATE__      "2024-09-15 18:56:42"
#define __BVERSION__   "0.1.003"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

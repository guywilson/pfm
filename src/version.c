#include "version.h"

#define __BDATE__      "2025-09-05 22:21:54"
#define __BVERSION__   "1.8.029"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

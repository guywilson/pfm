#include "version.h"

#define __BDATE__      "2024-10-17 20:35:48"
#define __BVERSION__   "1.0.040"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

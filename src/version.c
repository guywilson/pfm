#include "version.h"

#define __BDATE__      "2024-10-10 08:36:08"
#define __BVERSION__   "0.8.024"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

#include "version.h"

#define __BDATE__      "2024-09-29 16:22:38"
#define __BVERSION__   "0.8.005"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

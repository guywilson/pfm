#include "version.h"

#define __BDATE__      "2024-09-15 22:20:20"
#define __BVERSION__   "0.1.005"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

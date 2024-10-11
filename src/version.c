#include "version.h"

#define __BDATE__      "2024-10-11 21:30:13"
#define __BVERSION__   "0.8.029"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

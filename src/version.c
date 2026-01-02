#include "version.h"

#define __BDATE__      "2026-01-02 18:09:21"
#define __BVERSION__   "2.1.029"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}

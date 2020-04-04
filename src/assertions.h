#ifndef ASSERTIONS_H
#define ASSERTIONS_H

#include <assert.h>

#include <iostream>

inline void non_release_mode() {
#ifdef NDEBUG
    return;
#endif
    static bool first(true);
    if (first) {
        printf("Non release mode detected. Assertions on.\n");
        first = false;
    }
}

#endif
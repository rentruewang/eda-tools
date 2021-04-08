#include "utils.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

void log_release_mode() {
#if defined(NDEBUG)
    return;
#endif
    static bool first = true;
    if (first) {
        printf("Non release mode detected. Assertions on.\n");
        first = false;
    }
}

int debug_printf(const char* format, ...) {
#if defined(NDEBUG)
    return 0;
#else
    va_list args;
    va_start(args, format);
    int result = vprintf(format, args);
    va_end(args);
    return result;
#endif
}

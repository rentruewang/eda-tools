#include "utils.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

void log_release_mode() {
#ifdef NDEBUG
    return;
#endif
    static bool first = true;
    if (first) {
        printf("Non release mode detected. Assertions on.\n");
        first = false;
    }
}

int debug_printf(const char* format, ...) {
#if !defined(SILENT)
    va_list args;
    va_start(args, format);
    int result = vprintf(format, args);
    va_end(args);
    return result;
#endif
    return 0;
}

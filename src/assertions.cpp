#include "assertions.h"

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

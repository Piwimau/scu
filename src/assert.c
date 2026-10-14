#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "scu/assert.h"

[[noreturn]]
void scu_assert_fail(
    const char* restrict expr,
    const char* restrict file,
    int line,
    const char* restrict func,
    const char* restrict fmt,
    ...
) {
    fprintf(
        stderr,
        "SCU: Assertion '%s' failed at %s:%d in %s().\n",
        expr,
        file,
        line,
        func
    );
    if (fmt) {
        fprintf(stderr, "Message: ");
        va_list args;
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        va_end(args);
    }
    fflush(stderr);
    abort();
}
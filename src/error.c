#include <stdarg.h>
#include <stdlib.h>
#include "scu/assert.h"
#include "scu/error.h"
#include "scu/io.h"

[[noreturn]]
void scu_fatal(
    const char* restrict file,
    int line,
    const char* restrict func,
    const char* restrict fmt,
    ...
) {
    SCU_ASSERT(file);
    SCU_ASSERT(line >= 0);
    SCU_ASSERT(func);
    SCU_ASSERT(fmt);
    scu_fprintf(
        SCU_STDERR,
        "SCU: Fatal error occurred at %s:%d in %s().\n",
        file,
        line,
        func
    );
    scu_fprintf(SCU_STDERR, "Message: ");
    va_list args;
    va_start(args, fmt);
    scu_vfprintf(SCU_STDERR, fmt, args);
    va_end(args);
    scu_fflush(SCU_STDERR);
    abort();
}
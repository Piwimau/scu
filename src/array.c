#define SCU_SHORT_ALIASES

#include <stdlib.h>
#include "scu/array.h"
#include "scu/assert.h"

SCU_API void scu_array_sort(
    void* array,
    isize count,
    isize elemSize,
    ScuCompareFunc* cmpFunc
) {
    SCU_ASSERT(count >= 0);
    SCU_ASSERT(elemSize > 0);
    SCU_ASSERT(cmpFunc);
    if (count == 0) {
        return;
    }
    SCU_ASSERT(array);
    qsort(array, (usize) count, (usize) elemSize, cmpFunc);
}
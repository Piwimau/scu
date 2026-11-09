#define SCU_SHORT_ALIASES

#include <stddef.h>
#include "scu/array.h"
#include "scu/assert.h"
#include "scu/list.h"
#include "scu/util.h"

/** @brief A header stored before the actual data of the list. */
typedef struct ScuListHeader {

    /** @brief The allocator used for memory management. */
    ScuAllocator* allocator;

    /** @brief The size of each element (in bytes). */
    isize elemSize;

    /**
     * @brief The maximum number of elements that can be stored before a
     * reallocation is required.
     */
    isize capacity;

    /** @brief The current number of elements. */
    isize count;

    /**
     * @brief The actual data (i.e., the elements) stored after the header.
     *
     * @note This is a flexible array member, which is aligned as strictly as
     * `max_align_t` to ensure proper alignment for any type with fundamental
     * alignment requirements.
     */
    alignas(max_align_t) byte data[];

} ScuListHeader;

/** @brief The default capacity of a list. */
static constexpr isize SCU_DEFAULT_CAPACITY = 8;

/** @brief The growth factor for increasing the capacity of a list. */
static constexpr isize SCU_GROWTH_FACTOR = 2;

SCU_API void* scu_list_new_impl(isize elemSize, ScuAllocator* allocator) {
    return scu_list_new_with_capacity_impl(
        elemSize,
        SCU_DEFAULT_CAPACITY,
        allocator
    );
}

SCU_API void* scu_list_new_with_capacity_impl(
    isize elemSize,
    isize capacity,
    ScuAllocator* allocator
) {
    SCU_ASSERT(elemSize > 0);
    SCU_ASSERT(capacity >= 0);
    SCU_ASSERT(allocator);
    ScuListHeader* header = scu_allocate(
        SCU_SIZEOF(ScuListHeader) + elemSize * capacity,
        SCU_ALIGNOF(ScuListHeader),
        allocator
    );
    if (!header) {
        return nullptr;
    }
    header->allocator = allocator;
    header->elemSize = elemSize;
    header->capacity = capacity;
    header->count = 0;
    return header->data;
}

/**
 * @brief Returns a pointer to the header of a list.
 *
 * @param[in] data A pointer to the actual data of the list.
 * @return A pointer to the header of the list.
 */
static inline ScuListHeader* scu_get_header(void* data) {
    SCU_ASSERT(data);
    return (ScuListHeader*) ((byte*) data - SCU_SIZEOF(ScuListHeader));
}

SCU_API void* scu_list_clone(const void* list) {
    const ScuListHeader* header = scu_get_header(
        SCU_CONST_CAST(void*, list)
    );
    ScuListHeader* clone = scu_allocate(
        SCU_SIZEOF(ScuListHeader) + header->elemSize * header->capacity,
        SCU_ALIGNOF(ScuListHeader),
        header->allocator
    );
    if (!clone) {
        return nullptr;
    }
    clone->allocator = header->allocator;
    clone->elemSize = header->elemSize;
    clone->capacity = header->capacity;
    clone->count = header->count;
    scu_memcpy(clone->data, header->data, clone->elemSize * clone->count);
    return clone->data;
}

SCU_API isize scu_list_capacity(const void* list) {
    const ScuListHeader* header = scu_get_header(
        SCU_CONST_CAST(void*, list)
    );
    return header->capacity;
}

SCU_API isize scu_list_count(const void* list) {
    const ScuListHeader* header = scu_get_header(
        SCU_CONST_CAST(void*, list)
    );
    return header->count;
}

SCU_API ScuError scu_list_ensure_capacity_impl(void** list, isize capacity) {
    SCU_ASSERT(list);
    SCU_ASSERT(capacity >= 0);
    ScuListHeader* header = scu_get_header(*list);
    if (header->capacity < capacity) {
        isize newCapacity = (header->capacity > 0) ? header->capacity : 1;
        while (newCapacity < capacity) {
            newCapacity *= SCU_GROWTH_FACTOR;
        }
        ScuListHeader* newHeader = scu_resize_or_move(
            header,
            SCU_SIZEOF(ScuListHeader) + header->elemSize * header->capacity,
            SCU_SIZEOF(ScuListHeader) + header->elemSize * newCapacity,
            SCU_ALIGNOF(ScuListHeader),
            header->allocator
        );
        if (!newHeader) {
            return SCU_ERROR_OUT_OF_MEMORY;
        }
        newHeader->capacity = newCapacity;
        *list = newHeader->data;
    }
    return SCU_ERROR_NONE;
}

SCU_API ScuError scu_list_add_impl(
    void** restrict list,
    const void* restrict elem
) {
    SCU_ASSERT(list);
    SCU_ASSERT(elem);
    ScuListHeader* header = scu_get_header(*list);
    ScuError error = scu_list_ensure_capacity_impl(list, header->count + 1);
    if (error != SCU_ERROR_NONE) {
        return error;
    }
    // Get the header again, as the list may have been reallocated.
    header = scu_get_header(*list);
    scu_memcpy(
        header->data + header->elemSize * header->count,
        elem,
        header->elemSize
    );
    header->count++;
    return SCU_ERROR_NONE;
}

SCU_API ScuError scu_list_insert_at_impl(
    void** restrict list,
    isize index,
    const void* restrict elem
) {
    SCU_ASSERT(list);
    ScuListHeader* header = scu_get_header(*list);
    SCU_ASSERT(index >= 0 && index <= header->count);
    SCU_ASSERT(elem);
    ScuError error = scu_list_ensure_capacity_impl(list, header->count + 1);
    if (error != SCU_ERROR_NONE) {
        return error;
    }
    // Get the header again, as the list may have been reallocated.
    header = scu_get_header(*list);
    scu_memmove(
        header->data + header->elemSize * (index + 1),
        header->data + header->elemSize * index,
        header->elemSize * (header->count - index)
    );
    scu_memcpy(
        header->data + header->elemSize * index,
        elem,
        header->elemSize
    );
    header->count++;
    return SCU_ERROR_NONE;
}

SCU_API void scu_list_remove_at(void* list, isize index) {
    ScuListHeader* header = scu_get_header(list);
    SCU_ASSERT(index >= 0 && index < header->count);
    scu_memmove(
        header->data + header->elemSize * index,
        header->data + header->elemSize * (index + 1),
        header->elemSize * (header->count - index - 1)
    );
    header->count--;
}

SCU_API void scu_list_clear(void* list) {
    ScuListHeader* header = scu_get_header(list);
    header->count = 0;
}

SCU_API ScuError scu_list_trim_excess_impl(void** list) {
    SCU_ASSERT(list);
    ScuListHeader* header = scu_get_header(*list);
    if (header->capacity > header->count) {
        isize newCapacity = header->count;
        ScuListHeader* newHeader = scu_resize_or_move(
            header,
            SCU_SIZEOF(ScuListHeader) + header->elemSize * header->capacity,
            SCU_SIZEOF(ScuListHeader) + header->elemSize * newCapacity,
            SCU_ALIGNOF(ScuListHeader),
            header->allocator
        );
        if (!newHeader) {
            return SCU_ERROR_OUT_OF_MEMORY;
        }
        newHeader->capacity = newCapacity;
        *list = newHeader->data;
    }
    return SCU_ERROR_NONE;
}

SCU_API void scu_list_sort(void* list, ScuCompareFunc* cmpFunc) {
    SCU_ASSERT(cmpFunc);
    ScuListHeader* header = scu_get_header(list);
    scu_array_sort(header->data, header->count, header->elemSize, cmpFunc);
}

SCU_API void scu_list_delete(void* list) {
    if (!list) {
        return;
    }
    ScuListHeader* header = scu_get_header(list);
    scu_deallocate(
        header,
        SCU_SIZEOF(ScuListHeader) + header->elemSize * header->capacity,
        SCU_ALIGNOF(ScuListHeader),
        header->allocator
    );
}

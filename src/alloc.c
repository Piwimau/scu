#define SCU_SHORT_ALIASES

#include <stdatomic.h>
#include <stddef.h>
#include <stdlib.h>
#include "scu/alloc.h"
#include "scu/assert.h"
#include "scu/memory.h"

/**
 * @brief A header stored before each allocated block of memory.
 *
 * The header stores a pointer to the original allocator the block was allocated
 * with, such that `scu_realloc()` and `scu_free()` use the correct allocator
 * even if a different global allocator is later set using
 * `scu_set_global_allocator()`.
 */
typedef struct ScuBlockHeader {

    /** @brief The allocator the block was allocated with. */
    const ScuAllocator* allocator;

    /**
     * @brief The actual data (i.e., the block returned to the user).
     *
     * @note This is a flexible array member, which is aligned as strictly as
     * `max_align_t` to ensure proper alignment for any type with fundamental
     * alignment requirements.
     */
    alignas(max_align_t) byte data[];

} ScuBlockHeader;

/**
 * @brief Allocates an uninitialized block of memory of at least `size`
 * contiguous bytes using `malloc()`.
 *
 * @param[in, out] context Unused.
 * @param[in]      size    The requested size (in bytes).
 * @return A pointer to an uninitialized block of memory of at least `size`
 * contiguous bytes, or `nullptr` on failure.
 */
static void* scu_default_malloc([[maybe_unused]] void* context, isize size) {
    return malloc((usize) size);
}

/**
 * @brief Allocates a zero-initialized block of memory of at least `count *
 * size` contiguous bytes using `calloc()`.
 *
 * @param[in, out] context  Unused.
 * @param[in]      count    The requested number of elements.
 * @param[in]      size     The size of each element (in bytes).
 * @return A pointer to a zero-initialized block of memory of at least `count *
 * size` contiguous bytes, or `nullptr` on failure.
 */
static void* scu_default_calloc(
    [[maybe_unused]] void* context,
    isize count,
    isize size
) {
    return calloc((usize) count, (usize) size);
}

/**
 * @brief Reallocates a block of memory to at least `newSize` contiguous bytes
 * using `realloc()`.
 *
 * @param[in, out] context Unused.
 * @param[in]      block   A pointer to a block of memory, or `nullptr`.
 * @param[in]      newSize The new requested size (in bytes).
 * @return A pointer to the reallocated (and possibly moved) block of memory of
 * at least `newSize` contiguous bytes, or `nullptr` on failure.
 */
static void* scu_default_realloc(
    [[maybe_unused]] void* context,
    void* block,
    isize newSize
) {
    return realloc(block, (usize) newSize);
}

/**
 * @brief Deallocates a block of memory using `free()`.
 *
 * @param[in, out] context Unused.
 * @param[in]      block   A pointer to a block of memory to deallocate. If
 *                         equal to `nullptr`, this function does nothing.
 */
static void scu_default_free([[maybe_unused]] void* context, void* block) {
    free(block);
}

/** @brief The default allocator provided by the C standard library. */
static const ScuAllocator SCU_DEFAULT_ALLOCATOR = {
    .malloc = scu_default_malloc,
    .calloc = scu_default_calloc,
    .realloc = scu_default_realloc,
    .free = scu_default_free
};

/** @brief The global allocator. */
static const ScuAllocator* _Atomic scuGlobalAllocator = &SCU_DEFAULT_ALLOCATOR;

const ScuAllocator* scu_get_global_allocator() {
    return atomic_load_explicit(&scuGlobalAllocator, memory_order_acquire);
}

void scu_set_global_allocator(const ScuAllocator* allocator) {
    atomic_store_explicit(
        &scuGlobalAllocator,
        allocator ? allocator : &SCU_DEFAULT_ALLOCATOR,
        memory_order_release
    );
}

void* scu_malloc(isize size) {
    SCU_ASSERT(size >= 0);
    const ScuAllocator* allocator = scu_get_global_allocator();
    ScuBlockHeader* header = allocator->malloc(
        allocator->context,
        SCU_SIZEOF(ScuBlockHeader) + size
    );
    if (!header) {
        return nullptr;
    }
    header->allocator = allocator;
    return header->data;
}

void* scu_calloc(isize count, isize size) {
    SCU_ASSERT(count >= 0);
    SCU_ASSERT(size >= 0);
    const ScuAllocator* allocator = scu_get_global_allocator();
    ScuBlockHeader* header = allocator->calloc(
        allocator->context,
        1,
        SCU_SIZEOF(ScuBlockHeader) + count * size
    );
    if (!header) {
        return nullptr;
    }
    header->allocator = allocator;
    return header->data;
}

/**
 * @brief Returns a pointer to the header of a block of memory.
 *
 * @param[in] data A pointer to the block of memory.
 * @return A pointer to the header of the block of memory.
 *
 * @warning The behavior is undefined if `data` is `nullptr`.
 */
static inline ScuBlockHeader* scu_get_header(void* data) {
    SCU_ASSERT(data);
    return (ScuBlockHeader*) ((byte*) data - SCU_SIZEOF(ScuBlockHeader));
}

void* scu_realloc(void* block, isize newSize) {
    SCU_ASSERT(newSize >= 0);
    if (!block) {
        return scu_malloc(newSize);
    }
    ScuBlockHeader* oldHeader = scu_get_header(block);
    const ScuAllocator* allocator = oldHeader->allocator;
    ScuBlockHeader* newHeader = allocator->realloc(
        allocator->context,
        oldHeader,
        SCU_SIZEOF(ScuBlockHeader) + newSize
    );
    return newHeader ? newHeader->data : nullptr;
}

void scu_free(void* block) {
    if (!block) {
        return;
    }
    ScuBlockHeader* header = scu_get_header(block);
    const ScuAllocator* allocator = header->allocator;
    allocator->free(allocator->context, header);
}
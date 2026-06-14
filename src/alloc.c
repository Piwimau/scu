#define SCU_SHORT_ALIASES

#include <stdatomic.h>
#include <stddef.h>
#include <stdlib.h>
#include "scu/alloc.h"
#include "scu/assert.h"
#include "scu/memory.h"

/**
 * @brief Represents a header stored before each allocated block of memory.
 *
 * The header stores a pointer to the original allocator the block of memory was
 * allocated with, such that `scu_realloc()` and `scu_free()` use the correct
 * allocator even if a different global allocator is later set using
 * `scu_set_global_allocator()`.
 */
typedef struct ScuAllocHeader {

    /** @brief The original allocator the block of memory was allocated with. */
    const ScuAllocator* allocator;

    /**
     * @brief The actual data (i.e., the block of memory returned to the user).
     *
     * @note This is a flexible array member, which is aligned as strictly as
     * `max_align_t` to ensure proper alignment for any type with fundamental
     * alignment requirements.
     */
    alignas(max_align_t) byte data[];

} ScuAllocHeader;

/**
 * @brief Allocates an uninitialized block of memory of at least `size`
 * contiguous bytes using the C standard `malloc()` function.
 *
 * @param[in, out] context Unused.
 * @param[in]      size    The requested size (in bytes).
 * @return A pointer to an uninitialized block of memory of at least `size`
 * contiguous bytes, or `nullptr` on failure.
 */
[[nodiscard]]
static void* scu_default_malloc([[maybe_unused]] void* context, isize size) {
    return malloc((usize) size);
}

/**
 * @brief Allocates a zero-initialized block of memory of at least `count *
 * size` contiguous bytes using the C standard `calloc()` function.
 *
 * @param[in, out] context  Unused.
 * @param[in]      count    The requested number of elements.
 * @param[in]      size     The size of each element (in bytes).
 * @return A pointer to a zero-initialized block of memory of at least `count *
 * size` contiguous bytes, or `nullptr` on failure.
 */
[[nodiscard]]
static void* scu_default_calloc(
    [[maybe_unused]] void* context,
    isize count,
    isize size
) {
    return calloc((usize) count, (usize) size);
}

/**
 * @brief Reallocates a block of memory to at least `newSize` contiguous bytes
 * using the C standard `realloc()` function.
 *
 * @param[in, out] context Unused.
 * @param[in]      block   A pointer to a block of memory, or a `nullptr`.
 * @param[in]      newSize The new requested size (in bytes).
 * @return A pointer to the reallocated (and possibly moved) block of memory of
 * at least `newSize` contiguous bytes, or `nullptr` on failure.
 */
[[nodiscard]]
static void* scu_default_realloc(
    [[maybe_unused]] void* context,
    void* block,
    isize newSize
) {
    return realloc(block, (usize) newSize);
}

/**
 * @brief Deallocates a block of memory using the C standard `free()` function.
 *
 * @param[in, out] context Unused.
 * @param[in]      block   A pointer to a block of memory to deallocate. If
 *                         equal to `nullptr`, this function does nothing.
 */
static void scu_default_free([[maybe_unused]] void* context, void* block) {
    free(block);
}

/** @brief The default allocator relying on the C standard library. */
static const ScuAllocator SCU_DEFAULT_ALLOCATOR = {
    .malloc = scu_default_malloc,
    .calloc = scu_default_calloc,
    .realloc = scu_default_realloc,
    .free = scu_default_free
};

/** @brief The global allocator (initially set to the default one). */
static const ScuAllocator* _Atomic scuGlobalAllocator = &SCU_DEFAULT_ALLOCATOR;

const ScuAllocator* scu_get_global_allocator() {
    return atomic_load_explicit(&scuGlobalAllocator, memory_order_acquire);
}

void scu_set_global_allocator(const ScuAllocator* allocator) {
    atomic_store_explicit(
        &scuGlobalAllocator,
        (allocator != nullptr) ? allocator : &SCU_DEFAULT_ALLOCATOR,
        memory_order_release
    );
}

[[nodiscard]]
void* scu_malloc(isize size) {
    SCU_ASSERT(size >= 0);
    const ScuAllocator* allocator = scu_get_global_allocator();
    ScuAllocHeader* header = allocator->malloc(
        allocator->context,
        SCU_SIZEOF(ScuAllocHeader) + size
    );
    if (header == nullptr) {
        return nullptr;
    }
    header->allocator = allocator;
    return header->data;
}

[[nodiscard]]
void* scu_calloc(isize count, isize size) {
    SCU_ASSERT(count >= 0);
    SCU_ASSERT(size >= 0);
    const ScuAllocator* allocator = scu_get_global_allocator();
    ScuAllocHeader* header = allocator->calloc(
        allocator->context,
        1,
        SCU_SIZEOF(ScuAllocHeader) + count * size
    );
    if (header == nullptr) {
        return nullptr;
    }
    header->allocator = allocator;
    return header->data;
}

/**
 * @brief Returns a pointer to the header of a block of memory given its data.
 *
 * @param[in] data A pointer to the actual data.
 * @return A pointer to the header of the block of memory.
 */
static inline ScuAllocHeader* scu_data_to_header(void* data) {
    SCU_ASSERT(data != nullptr);
    return (ScuAllocHeader*) ((byte*) data - SCU_SIZEOF(ScuAllocHeader));
}

[[nodiscard]]
void* scu_realloc(void* block, isize newSize) {
    SCU_ASSERT(newSize >= 0);
    if (block == nullptr) {
        return scu_malloc(newSize);
    }
    ScuAllocHeader* oldHeader = scu_data_to_header(block);
    const ScuAllocator* allocator = oldHeader->allocator;
    ScuAllocHeader* newHeader = allocator->realloc(
        allocator->context,
        oldHeader,
        SCU_SIZEOF(ScuAllocHeader) + newSize
    );
    return (newHeader == nullptr) ? nullptr : newHeader->data;
}

void scu_free(void* block) {
    if (block != nullptr) {
        ScuAllocHeader* header = scu_data_to_header(block);
        const ScuAllocator* allocator = header->allocator;
        allocator->free(allocator->context, header);
    }
}
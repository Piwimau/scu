#ifndef SCU_ALLOC_H
#define SCU_ALLOC_H

#include "scu/types.h"

/**
 * @brief An allocator's malloc-like operation.
 *
 * @param[in, out] context A user-provided context, which may be `nullptr` if
 *                         not required by the allocator.
 * @param[in]      size    The requested size (in bytes).
 * @return A pointer to an uninitialized block of memory of at least `size`
 * contiguous bytes, or `nullptr` on failure.
 *
 * @note Implementations should behave like `malloc(size)`, i.e., return a
 * pointer to an uninitialized block of memory of at least `size` contiguous
 * bytes, or `nullptr` on failure. The pointer returned should be suitably
 * aligned for any type with fundamental alignment requirements.
 */
typedef void* ScuMallocFunc(void* context, Scuisize size);

/**
 * @brief An allocator's calloc-like operation.
 *
 * @param[in, out] context A user-provided context, which may be `nullptr` if
 *                         not required by the allocator.
 * @param[in]      count   The requested number of elements.
 * @param[in]      size    The size of each element (in bytes).
 * @return A pointer to a zero-initialized block of memory of at least `count *
 * size` contiguous bytes, or `nullptr` on failure.
 *
 * @note Implementations should behave like `calloc(count, size)`, i.e., return
 * a pointer to a zero-initialized block of memory of at least `count * size`
 * contiguous bytes, or `nullptr` on failure. The pointer returned should be
 * suitably aligned for any type with fundamental alignment requirements.
 */
typedef void* ScuCallocFunc(void* context, Scuisize count, Scuisize size);

/**
 * @brief An allocator's realloc-like operation.
 *
 * @param[in, out] context A user-provided context, which may be `nullptr` if
 *                         not required by the allocator.
 * @param[in]      block   A pointer to a previously allocated block of memory,
 *                         or `nullptr`.
 * @param[in]      newSize The new requested size (in bytes).
 * @return A pointer to the reallocated (and possibly moved) block of memory of
 * at least `newSize` bytes, or `nullptr` on failure.
 *
 * @note Implementations should behave like `realloc(block, newSize)`, i.e.,
 * reallocate (and possibly move) the block and return a pointer to the new
 * block of memory of at least `newSize` contiguous bytes, or `nullptr` on
 * failure. Additionally, if `block` is `nullptr`, it should behave like
 * `malloc(newSize)`. The pointer returned should be suitably aligned for any
 * type with fundamental alignment requirements.
 *
 * Well-behaved realloc implementations leave the original block of memory
 * intact if an allocation failure occurs. However, if the reallocation succeeds
 * and the block is moved, the original block is usually invalidated and should
 * not be used anymore.
 */
typedef void* ScuReallocFunc(void* context, void* block, Scuisize newSize);

/**
 * @brief An allocator's free-like operation.
 *
 * @param[in, out] context A user-provided context, which may be `nullptr` if
 *                         not required by the allocator.
 * @param[in]      block   A pointer to a previously allocated block of memory,
 *                         or `nullptr`.
 *
 * @note Implementations should behave like `free(block)`, i.e., deallocate a
 * block of memory previously allocated by a corresponding `ScuMallocFunc`,
 * `ScuCallocFunc` or `ScuReallocFunc`. It should accept `nullptr`, in which
 * case the function should simply do nothing.
 */
typedef void ScuFreeFunc(void* context, void* block);

/** @brief A custom allocator. */
typedef struct ScuAllocator {

    /** @brief The allocator's malloc-like operation. */
    ScuMallocFunc* malloc;

    /** @brief The allocator's calloc-like operation. */
    ScuCallocFunc* calloc;

    /** @brief The allocator's realloc-like operation. */
    ScuReallocFunc* realloc;

    /** @brief The allocator's free-like operation. */
    ScuFreeFunc* free;

    /**
     * @brief A user-provided context passed to the allocator's operations.
     *
     * @note This pointer may be `nullptr` if not required by the allocator.
     */
    void* context;

} ScuAllocator;

/**
 * @brief Returns the global allocator.
 *
 * @return The global allocator.
 *
 * @warning Do not modify the returned allocator in place unless you synchronize
 * with other threads that use the same allocator. The default global allocator
 * must not be modified at all.
 *
 * @note SCU internally stores a pointer to a global allocator that is used by
 * the `scu_malloc()` and `scu_calloc()` functions (and possibly the
 * `scu_realloc()` and `scu_free()` functions as well, depending on whether a
 * custom allocator is later set using `scu_set_global_allocator()`). The
 * allocator must remain valid as long as these functions are used. SCU does not
 * attempt to modify or free any allocator passed to it.
 *
 * The default global allocator provided by SCU relies on the C standard library
 * functions `malloc()`, `calloc()`, `realloc()` and `free()`. Its context is a
 * `nullptr`, as it is not required.
 *
 * A custom allocator may be set using `scu_set_global_allocator()`, which will
 * be used in subsequent calls to `scu_malloc()` and `scu_calloc()`. Note that
 * `scu_realloc()` and `scu_free()` continue to use the original allocator a
 * block of memory was allocated with, to which a pointer is stored in a small
 * header before the actual block. It is therefore safe to change the global
 * allocator at runtime.
 *
 * This function is thread-safe. However, the thread safety of the allocator
 * itself depends on its implementation. The default global allocator provided
 * by SCU relies on the C standard library functions `malloc()`, `calloc()`,
 * `realloc()` and `free()`, which are guaranteed to be thread-safe.
 */
const ScuAllocator* scu_get_global_allocator();

/**
 * @brief Sets the global allocator.
 *
 * @param[in] allocator The allocator to set. If equal to `nullptr`, the default
 *                      global allocator relying on the C standard library is
 *                      restored.
 *
 * @warning Do not modify the passed allocator in place unless you synchronize
 * with other threads that use the same allocator.
 *
 * @note SCU internally stores a pointer to a global allocator that is used by
 * the `scu_malloc()` and `scu_calloc()` functions (and possibly the
 * `scu_realloc()` and `scu_free()` functions as well, depending on whether a
 * custom allocator is later set using this function). The allocator must remain
 * valid as long as these functions are used. SCU does not attempt to modify or
 * free any allocator passed to it.
 *
 * The default global allocator provided by SCU relies on the C standard library
 * functions `malloc()`, `calloc()`, `realloc()` and `free()`. Its context is a
 * `nullptr`, as it is not required.
 *
 * A custom allocator may be set using this function, which will be used in
 * subsequent calls to `scu_malloc()` and `scu_calloc()`. Note that
 * `scu_realloc()` and `scu_free()` continue to use the original allocator a
 * block of memory was allocated with, to which a pointer is stored in a small
 * header before the actual block. It is therefore safe to change the global
 * allocator at runtime.
 *
 * This function is thread-safe. However, the thread safety of the allocator
 * itself depends on its implementation. The default global allocator provided
 * by SCU relies on the C standard library functions `malloc()`, `calloc()`,
 * `realloc()` and `free()`, which are guaranteed to be thread-safe.
 */
void scu_set_global_allocator(const ScuAllocator* allocator);

/**
 * @brief Allocates an uninitialized block of memory of at least `size`
 * contiguous bytes.
 *
 * @param[in] size The requested size (in bytes).
 * @return A pointer to an uninitialized block of memory of at least `size`
 * contiguous bytes, or `nullptr` on failure.
 *
 * @warning The behavior is undefined if `size` is negative.
 *
 * The caller is responsible for freeing the memory using `scu_free()` when it
 * is no longer needed.
 *
 * @note This function uses the global allocator, which is either a default one
 * that relies on the C standard library functions `malloc()`, `calloc()`,
 * `realloc()` and `free()`, or a custom one set using
 * `scu_set_global_allocator()`. Its behavior therefore depends on the
 * implementation of the allocator. Generally speaking, it is expected to behave
 * like `malloc(size)`. See `ScuMallocFunc` for more information.
 *
 * This function is only thread-safe if the underlying allocator is.
 */
void* scu_malloc(Scuisize size);

/**
 * @brief Allocates a zero-initialized block of memory of at least `count *
 * size` contiguous bytes.
 *
 * @param[in] count The requested number of elements.
 * @param[in] size  The size of each element (in bytes).
 * @return A pointer to a zero-initialized block of memory of at least `count *
 * size` contiguous bytes, or `nullptr` on failure.
 *
 * @warning The behavior is undefined if `count` or `size` is negative.
 *
 * The caller is responsible for freeing the memory using `scu_free()` when it
 * is no longer needed.
 *
 * @note This function uses the global allocator, which is either a default one
 * that relies on the C standard library functions `malloc()`, `calloc()`,
 * `realloc()` and `free()`, or a custom one set using
 * `scu_set_global_allocator()`. Its behavior therefore depends on the
 * implementation of the allocator. Generally speaking, it is expected to behave
 * like `calloc(count, size)`. See `ScuCallocFunc` for more information.
 *
 * This function is only thread-safe if the underlying allocator is.
 */
void* scu_calloc(Scuisize count, Scuisize size);

/**
 * @brief Reallocates a block of memory to at least `newSize` contiguous bytes.
 *
 * @param[in] block   A pointer to a block of memory, or `nullptr`.
 * @param[in] newSize The new requested size (in bytes).
 * @return A pointer to the reallocated (and possibly moved) block of memory of
 * at least `newSize` contiguous bytes, or `nullptr` on failure.
 *
 * @warning The behavior is undefined if `newSize` is negative.
 *
 * The caller is responsible for freeing the memory using `scu_free()` when it
 * is no longer needed.
 *
 * @note If `block` is `nullptr`, this function uses the global allocator, which
 * is either a default one that relies on the C standard library functions
 * `malloc()`, `calloc()`, `realloc()` and `free()`, or a custom one set using
 * `scu_set_global_allocator()`. Its behavior therefore depends on the
 * implementation of the allocator. Generally speaking, it is expected to behave
 * like `malloc(newSize)`. See `ScuReallocFunc` for more information.
 *
 * If `block` is not `nullptr`, this function uses the original allocator the
 * block of memory was allocated with, to which a pointer is stored in a small
 * header before the actual block.
 *
 * It is not thread-safe to reallocate the same block of memory concurrently
 * from multiple threads. However, this function is thread-safe if `block` is
 * `nullptr` and the underlying allocator is.
 */
void* scu_realloc(void* block, Scuisize newSize);

/**
 * @brief Deallocates a block of memory previously allocated by a call to
 * `scu_malloc()`, `scu_calloc()` or `scu_realloc()`.
 *
 * @param[in] block A pointer to a block of memory to deallocate. If equal to
 *                  `nullptr`, this function does nothing.
 *
 * @note It is not thread-safe to deallocate the same block of memory
 * concurrently from multiple threads.
 */
void scu_free(void* block);

#endif
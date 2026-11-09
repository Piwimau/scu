#ifndef SCU_MEMORY_H
#define SCU_MEMORY_H

#include "scu/api.h"
#include "scu/types.h"

/** @brief A memory allocator. */
typedef struct ScuAllocator {

    /**
     * @brief Allocates an uninitialized block of memory.
     *
     * @param[in, out] context An optional user-defined context.
     * @param[in]      size    The desired size (in bytes).
     * @param[in]      align   The desired alignment (in bytes).
     * @return A pointer to the block of memory on success, or `nullptr` on
     * failure.
     *
     * @warning The behavior is undefined if `size` is less than or equal to
     * zero, or if `align` is not a power of two.
     *
     * The caller is responsible for eventually deallocating the memory using
     * the corresponding deallocation function.
     *
     * @note This is a mandatory operation that must be supported by custom
     * allocators (i.e., this field must not be `nullptr`).
     *
     * `context` may be `nullptr` if no user-defined context is required.
     */
    void* (*allocate)(void* context, Scuisize size, Scuisize align);

    /**
     * @brief Allocates a zero-initialized block of memory.
     *
     * @param[in, out] context An optional user-defined context.
     * @param[in]      size    The desired size (in bytes).
     * @param[in]      align   The desired alignment (in bytes).
     * @return A pointer to the block of memory on success, or `nullptr` on
     * failure.
     *
     * @warning The behavior is undefined if `size` is less than or equal to
     * zero, or if `align` is not a power of two.
     *
     * The caller is responsible for eventually deallocating the memory using
     * the corresponding deallocation function.
     *
     * @note This is an optional operation that may be supported by custom
     * allocators (i.e., this field may be `nullptr`).
     *
     * `context` may be `nullptr` if no user-defined context is required.
     */
    void* (*allocateZeroed)(void* context, Scuisize size, Scuisize align);

    /**
     * @brief Tries to resize a block of memory in-place.
     *
     * @param[in, out] context An optional user-defined context.
     * @param[in]      block   The block of memory to resize.
     * @param[in]      oldSize The current size (in bytes).
     * @param[in]      newSize The desired size (in bytes).
     * @param[in]      align   The desired alignment (in bytes).
     * @return `true` if the block of memory was successfully resized in-place,
     * `false` otherwise.
     *
     * @warning The behavior is undefined if `oldSize` or `newSize` is less than
     * or equal to zero, or if `align` is not a power of two.
     *
     * The caller is responsible for eventually deallocating the memory using
     * the corresponding deallocation function.
     *
     * @note This is an optional operation that may be supported by custom
     * allocators (i.e., this field may be `nullptr`). Implementations should
     * ensure that the pointer to the original block of memory remains valid if
     * the resize fails.
     *
     * `context` may be `nullptr` if no user-defined context is required.
     */
    bool (*resize)(
        void* context,
        void* block,
        Scuisize oldSize,
        Scuisize newSize,
        Scuisize align
    );

    /**
     * @brief Resizes a block of memory, moving it if an in-place resize is not
     * possible.
     *
     * @param[in, out] context An optional user-defined context.
     * @param[in]      block   The block of memory to resize.
     * @param[in]      oldSize The current size (in bytes).
     * @param[in]      newSize The desired size (in bytes).
     * @param[in]      align   The desired alignment (in bytes).
     * @return A pointer to the resized (and possibly moved) block of memory on
     * success, or `nullptr` on failure.
     *
     * @warning The behavior is undefined if `oldSize` is less than zero, if
     * `block` is `nullptr` and `oldSize` is not zero, if `newSize` is less than
     * or equal to zero, or if `align` is not a power of two.
     *
     * The caller is responsible for eventually deallocating the memory using
     * the corresponding deallocation function.
     *
     * @note This is an optional operation that may be supported by custom
     * allocators (i.e., this field may be `nullptr`). Implementations should
     * behave similarly to the standard `realloc()` function, i.e., the pointer
     * to the original block of memory should be considered invalid after a
     * successful resize. However, if the operation fails, the pointer to the
     * original block of memory should remain valid. Additionally, it should
     * accept a `nullptr` as the block to resize, in which case it should behave
     * like `allocate(context, newSize, align)`. In this case, `oldSize`
     * must be zero.
     *
     * `context` may be `nullptr` if no user-defined context is required.
     */
    void* (*resizeOrMove)(
        void* context,
        void* block,
        Scuisize oldSize,
        Scuisize newSize,
        Scuisize align
    );

    /**
     * @brief Deallocates a block of memory.
     *
     * @param[in, out] context An optional user-defined context.
     * @param[in]      block   The block of memory to deallocate.
     * @param[in]      size    The size of the block (in bytes).
     * @param[in]      align   The alignment of the block (in bytes).
     *
     * @warning The behavior is undefined if `size` is less than zero, or if
     * `align` is not a power of two.
     *
     * @note This is a mandatory operation that must be supported by custom
     * allocators (i.e., this field must not be `nullptr`). Implementations
     * should behave similarly to the standard `free()` function, i.e., accept
     * a `nullptr` as the block to deallocate, in which case no action is taken.
     *
     * `context` may be `nullptr` if no user-defined context is required.
     */
    void (*deallocate)(
        void* context,
        void* block,
        Scuisize size,
        Scuisize align
    );

    /**
     * @brief An optional user-defined context.
     *
     * @note This field may be `nullptr` if no user-defined context is required.
     */
    void* context;

} ScuAllocator;

/**
 * @brief Returns the default allocator.
 *
 * @return The default allocator.
 *
 * @warning This function returns a pointer to an internal data structure. The
 * behavior is undefined if the default allocator is modified or deallocated.
 */
SCU_API ScuAllocator* scu_default_allocator();

/**
 * @brief Allocates an uninitialized block of memory.
 *
 * @param[in] size      The desired size (in bytes).
 * @param[in] align     The desired alignment (in bytes).
 * @param[in] allocator The allocator to allocate the memory with.
 * @return A pointer to the block of memory on success, or `nullptr` on failure.
 *
 * @warning The behavior is undefined if `size` is less than or equal to zero,
 * or if `align` is not a power of two.
 *
 * The caller is responsible for eventually deallocating the memory using the
 * corresponding deallocation function.
 */
SCU_API void* scu_allocate(
    Scuisize size,
    Scuisize align,
    ScuAllocator* allocator
);

/**
 * @brief Allocates a zero-initialized block of memory.
 *
 * @param[in] size      The desired size (in bytes).
 * @param[in] align     The desired alignment (in bytes).
 * @param[in] allocator The allocator to allocate the memory with.
 * @return A pointer to the block of memory on success, or `nullptr` on failure.
 *
 * @warning The behavior is undefined if `size` is less than or equal to zero,
 * or if `align` is not a power of two.
 *
 * The caller is responsible for eventually deallocating the memory using the
 * corresponding deallocation function.
 */
SCU_API void* scu_allocate_zeroed(
    Scuisize size,
    Scuisize align,
    ScuAllocator* allocator
);

/**
 * @brief Tries to resize a block of memory in-place.
 *
 * @param[in] block     The block of memory to resize.
 * @param[in] oldSize   The current size (in bytes).
 * @param[in] newSize   The desired size (in bytes).
 * @param[in] align     The desired alignment (in bytes).
 * @param[in] allocator The allocator to resize the block of memory with.
 * @return `true` if the block of memory was successfully resized in-place,
 * `false` otherwise.
 *
 * @warning The behavior is undefined if `oldSize` or `newSize` is less than or
 * equal to zero, or if `align` is not a power of two.
 *
 * The caller is responsible for eventually deallocating the memory using the
 * corresponding deallocation function.
 *
 * @note The pointer to the original block of memory remains valid if the resize
 * fails.
 */
SCU_API bool scu_resize(
    void* block,
    Scuisize oldSize,
    Scuisize newSize,
    Scuisize align,
    ScuAllocator* allocator
);

/**
 * @brief Resizes a block of memory, moving it if an in-place resize is not
 * possible.
 *
 * @param[in] block     The block of memory to resize.
 * @param[in] oldSize   The current size (in bytes).
 * @param[in] newSize   The desired size (in bytes).
 * @param[in] align     The desired alignment (in bytes).
 * @param[in] allocator The allocator to resize the block of memory with.
 * @return A pointer to the resized (and possibly moved) block of memory on
 * success, or `nullptr` on failure.
 *
 * @warning The behavior is undefined if `oldSize` is less than zero, if `block`
 * is `nullptr` and `oldSize` is not zero, if `newSize` is less than or equal to
 * zero, or if `align` is not a power of two.
 *
 * The caller is responsible for eventually deallocating the memory using the
 * corresponding deallocation function.
 *
 * @note This function behaves similarly to the standard `realloc()` function,
 * i.e., the pointer to the original block of memory should be considered
 * invalid after a successful resize. However, if the operation fails, the
 * pointer to the original block of memory remains valid. Additionally, it
 * accepts a `nullptr` as the block to resize, in which case it behaves like
 * `scu_allocate(newSize, align, allocator)`. In this case, `oldSize` must
 * be zero.
 */
SCU_API void* scu_resize_or_move(
    void* block,
    Scuisize oldSize,
    Scuisize newSize,
    Scuisize align,
    ScuAllocator* allocator
);

/**
 * @brief Deallocates a block of memory.
 *
 * @param[in] block     The block of memory to deallocate.
 * @param[in] size      The size of the block (in bytes).
 * @param[in] align     The alignment of the block (in bytes).
 * @param[in] allocator The allocator to deallocate the block of memory with.
 *
 * @warning The behavior is undefined if `size` is less than zero, or if
 * `align` is not a power of two.
 *
 * @note This function behaves similarly to the standard `free()` function,
 * i.e., it accepts a `nullptr` as the block to deallocate, in which case no
 * action is taken.
 */
SCU_API void scu_deallocate(
    void* block,
    Scuisize size,
    Scuisize align,
    ScuAllocator* allocator
);

/**
 * @brief Returns the size of an expression (in bytes) as an `Scuisize`.
 *
 * @param[in] expr The expression to examine.
 * @return The size of the expression (in bytes) as an `Scuisize`.
 */
#define SCU_SIZEOF(expr) ((Scuisize) sizeof(expr))

/**
 * @brief Returns the alignment of an expression (in bytes) as an `Scuisize`.
 *
 * @param[in] expr The expression to examine.
 * @return The alignment of the expression (in bytes) as an `Scuisize`.
 */
#define SCU_ALIGNOF(expr) ((Scuisize) alignof(expr))

/**
 * @brief Aligns a value up to the nearest multiple of an alignment.
 *
 * @param[in] value The value to align.
 * @param[in] align The desired alignment.
 * @return The aligned value.
 *
 * @warning The behavior is undefined if `align` is not a power of two.
 */
#define SCU_ALIGN_UP(value, align) (((value) + (align) - 1) & ~((align) - 1))

/**
 * @brief Selects a provided allocator.
 *
 * @param[in] allocator The allocator to use.
 * @param[in] ...       Ignored.
 * @return The provided allocator.
 *
 * @note This macro is an implementation detail and should not be used directly.
 */
#define SCU_ALLOCATOR_OR_DEFAULT_IMPL(allocator, ...) allocator

/**
 * @brief Selects a provided allocator or falls back to the default allocator if
 * none is provided.
 *
 * @param[in] ... The allocator to use (if any).
 * @return An allocator, either the provided one or the default allocator.
 *
 * @note This macro is an implementation detail and should not be used directly.
 */
#define SCU_ALLOCATOR_OR_DEFAULT(...)                     \
    SCU_ALLOCATOR_OR_DEFAULT_IMPL(                        \
        __VA_ARGS__ __VA_OPT__(,) scu_default_allocator() \
    )

/**
 * @brief Allocates an uninitialized object of a specified type.
 *
 * @param[in] type The type of the object to allocate.
 * @param[in] ...  The allocator to allocate the object with.
 * @return A pointer to the object on success, or `nullptr` on failure.
 *
 * @warning The caller is responsible for eventually deallocating the memory
 * using the corresponding deallocation function.
 *
 * @note The last argument is optional and specifies the allocator to use. If
 * omitted, the default allocator will be used.
 */
#define SCU_NEW(type, ...)                    \
    scu_allocate(                             \
        SCU_SIZEOF(type),                     \
        SCU_ALIGNOF(type),                    \
        SCU_ALLOCATOR_OR_DEFAULT(__VA_ARGS__) \
    )

/**
 * @brief Allocates a zero-initialized object of a specified type.
 *
 * @param[in] type The type of the object to allocate.
 * @param[in] ...  The allocator to allocate the object with.
 * @return A pointer to the object on success, or `nullptr` on failure.
 *
 * @warning The caller is responsible for eventually deallocating the memory
 * using the corresponding deallocation function.
 *
 * @note The last argument is optional and specifies the allocator to use. If
 * omitted, the default allocator will be used.
 */
#define SCU_NEW_ZEROED(type, ...)             \
    scu_allocate_zeroed(                      \
        SCU_SIZEOF(type),                     \
        SCU_ALIGNOF(type),                    \
        SCU_ALLOCATOR_OR_DEFAULT(__VA_ARGS__) \
    )

/**
 * @brief Deallocates an object of a specified type.
 *
 * @param[in] ptr The pointer to the object to deallocate.
 * @param[in] ... The allocator to deallocate the object with.
 *
 * @note This macro behaves similarly to the standard `free()` function, i.e.,
 * it accepts a `nullptr` as the pointer to deallocate, in which case no action
 * is taken.
 *
 * The last argument is optional and specifies the allocator to use. If omitted,
 * the default allocator will be used.
 */
#define SCU_DELETE(ptr, ...)                  \
    scu_deallocate(                           \
        (ptr),                                \
        SCU_SIZEOF(*(ptr)),                   \
        SCU_ALIGNOF(typeof(*(ptr))),          \
        SCU_ALLOCATOR_OR_DEFAULT(__VA_ARGS__) \
    )

/**
 * @brief Allocates an array of uninitialized objects of a specified type.
 *
 * @param[in] type  The type of the objects to allocate.
 * @param[in] count The number of objects to allocate.
 * @param[in] ...   The allocator to allocate the array with.
 * @return A pointer to the array on success, or `nullptr` on failure.
 *
 * @warning The behavior is undefined if `count` is less than or equal to zero.
 *
 * The caller is responsible for eventually deallocating the memory using the
 * corresponding deallocation function.
 *
 * @note The last argument is optional and specifies the allocator to use. If
 * omitted, the default allocator will be used.
 */
#define SCU_ARRAY_NEW(type, count, ...)       \
    scu_allocate(                             \
        SCU_SIZEOF(type) * (count),           \
        SCU_ALIGNOF(type),                    \
        SCU_ALLOCATOR_OR_DEFAULT(__VA_ARGS__) \
    )

/**
 * @brief Allocates an array of zero-initialized objects of a specified type.
 *
 * @param[in] type  The type of the objects to allocate.
 * @param[in] count The number of objects to allocate.
 * @param[in] ...   The allocator to allocate the array with.
 * @return A pointer to the array on success, or `nullptr` on failure.
 *
 * @warning The behavior is undefined if `count` is less than or equal to zero.
 *
 * The caller is responsible for eventually deallocating the memory using the
 * corresponding deallocation function.
 *
 * @note The last argument is optional and specifies the allocator to use. If
 * omitted, the default allocator will be used.
 */
#define SCU_ARRAY_NEW_ZEROED(type, count, ...) \
    scu_allocate_zeroed(                       \
        SCU_SIZEOF(type) * (count),            \
        SCU_ALIGNOF(type),                     \
        SCU_ALLOCATOR_OR_DEFAULT(__VA_ARGS__)  \
    )

/**
 * @brief Tries to resize an array of objects of a specified type in-place.
 *
 * @param[in] ptr      The pointer to the array to resize.
 * @param[in] oldCount The current number of objects in the array.
 * @param[in] newCount The desired number of objects in the array.
 * @param[in] ...      The allocator to resize the array with.
 * @return `true` if the array was successfully resized in-place, `false`
 * otherwise.
 *
 * @warning The behavior is undefined if `oldCount` or `newCount` is less than
 * or equal to zero.
 *
 * The caller is responsible for eventually deallocating the memory using the
 * corresponding deallocation function.
 *
 * @note The pointer to the original array remains valid if the resize fails.
 *
 * The last argument is optional and specifies the allocator to use. If omitted,
 * the default allocator will be used.
 */
#define SCU_ARRAY_RESIZE(ptr, oldCount, newCount, ...) \
    scu_resize(                                        \
        (ptr),                                         \
        SCU_SIZEOF(*(ptr)) * (oldCount),               \
        SCU_SIZEOF(*(ptr)) * (newCount),               \
        SCU_ALIGNOF(typeof(*(ptr))),                   \
        SCU_ALLOCATOR_OR_DEFAULT(__VA_ARGS__)          \
    )

/**
 * @brief Resizes an array of objects of a specified type, moving it if an
 * in-place resize is not possible.
 *
 * @param[in] ptr      The pointer to the array to resize.
 * @param[in] oldCount The current number of objects in the array.
 * @param[in] newCount The desired number of objects in the array.
 * @param[in] ...      The allocator to resize the array with.
 * @return A pointer to the resized (and possibly moved) array on success, or
 * `nullptr` on failure.
 *
 * @warning The behavior is undefined if `oldCount` is less than zero, if `ptr`
 * is `nullptr` and `oldCount` is not zero, or if `newCount` is less than or
 * equal to zero.
 *
 * The caller is responsible for eventually deallocating the memory using the
 * corresponding deallocation function.
 *
 * @note This macro behaves similarly to the standard `realloc()` function,
 * i.e., the pointer to the original array should be considered invalid after a
 * successful resize. However, if the operation fails, the pointer to the
 * original array remains valid. Additionally, it accepts a `nullptr` as the
 * array to resize, in which case it behaves like `SCU_ARRAY_NEW(type, newCount,
 * ...)`. In this case, `oldCount` must be zero.
 *
 * The last argument is optional and specifies the allocator to use. If omitted,
 * the default allocator will be used.
 */
#define SCU_ARRAY_RESIZE_OR_MOVE(ptr, oldCount, newCount, ...) \
    scu_resize_or_move(                                        \
        (ptr),                                                 \
        SCU_SIZEOF(*(ptr)) * (oldCount),                       \
        SCU_SIZEOF(*(ptr)) * (newCount),                       \
        SCU_ALIGNOF(typeof(*(ptr))),                           \
        SCU_ALLOCATOR_OR_DEFAULT(__VA_ARGS__)                  \
    )

/**
 * @brief Deallocates an array of objects of a specified type.
 *
 * @param[in] ptr   The pointer to the array to deallocate.
 * @param[in] count The number of objects in the array.
 * @param[in] ...   The allocator to deallocate the array with.
 *
 * @note This macro behaves similarly to the standard `free()` function, i.e.,
 * it accepts a `nullptr` as the array to deallocate, in which case no action is
 * taken.
 *
 * The last argument is optional and specifies the allocator to use. If omitted,
 * the default allocator will be used.
 */
#define SCU_ARRAY_DELETE(ptr, count, ...)     \
    scu_deallocate(                           \
        (ptr),                                \
        SCU_SIZEOF(*(ptr)) * (count),         \
        SCU_ALIGNOF(typeof(*(ptr))),          \
        SCU_ALLOCATOR_OR_DEFAULT(__VA_ARGS__) \
    )

/**
 * @brief Finds the first occurrence of a specified byte in a block of memory.
 *
 * @param[in] block The block of memory to examine.
 * @param[in] c     The byte to search for.
 * @param[in] count The number of bytes to examine.
 * @return A pointer to the first occurrence of `c` in `block`, or `nullptr` if
 * no such byte is found.
 *
 * @warning The behavior is undefined if `block` is not a pointer to a block of
 * memory of at least `count` contiguous bytes.
 *
 * @note If `count` is zero, `block` is ignored (it may even be `nullptr`), and
 * the function returns `nullptr`.
 */
SCU_API void* scu_memchr(const void* block, Scubyte c, Scuisize count);

/**
 * @brief Finds the last occurrence of a specified byte in a block of memory.
 *
 * @param[in] block The block of memory to examine.
 * @param[in] c     The byte to search for.
 * @param[in] count The number of bytes to examine.
 * @return A pointer to the last occurrence of `c` in `block`, or `nullptr` if
 * no such byte is found.
 *
 * @warning The behavior is undefined if `block` is not a pointer to a block of
 * memory of at least `count` contiguous bytes.
 *
 * @note If `count` is zero, `block` is ignored (it may even be `nullptr`), and
 * the function returns `nullptr`.
 */
SCU_API void* scu_memrchr(const void* block, Scubyte c, Scuisize count);

/**
 * @brief Compares two blocks of memory lexicographically.
 *
 * @param[in] left  The first block of memory.
 * @param[in] right The second block of memory.
 * @param[in] count The number of bytes to compare.
 * @return A negative value if `left` appears before `right` in lexicographical
 * order, zero if they compare equal or `count` is zero, or a positive value if
 * `left` appears after `right`.
 *
 * @warning The behavior is undefined if `left` or `right` is not a pointer to a
 * block of memory of at least `count` contiguous bytes.
 *
 * This function compares the byte representation of the two blocks of memory,
 * not the values of the objects they may represent. It should not be used to
 * compare objects (e.g., structs, unions, floating-point numbers, etc.) for
 * equality, as there could be padding bits or multiple representations of the
 * same value. In such cases, the return value of this function may be
 * misleading and a member-wise or numerical comparison should be performed
 * instead.
 *
 * @note If `count` is zero, both `left` and `right` are ignored (each may even
 * be `nullptr`), and the function returns zero.
 */
SCU_API int scu_memcmp(const void* left, const void* right, Scuisize count);

/**
 * @brief Fills a block of memory with a specified byte.
 *
 * @param[out] dest  The block of memory to fill.
 * @param[in]  c     The byte to fill the memory with.
 * @param[in]  count The number of bytes to fill.
 * @return A copy of `dest`.
 *
 * @warning The behavior is undefined if `dest` is not a pointer to a block of
 * memory of at least `count` contiguous bytes.
 *
 * A call to this function may be optimized away if the block of memory is not
 * accessed again for the remainder of its lifetime. For this reason, it should
 * not be used to securely erase sensitive data (e.g., passwords, cryptographic
 * keys, etc.).
 *
 * @note If `count` is zero, no bytes are filled and the function immediately
 * returns `dest` (which may be `nullptr`).
 */
SCU_API void* scu_memset(void* dest, Scubyte c, Scuisize count);

/**
 * @brief Copies a non-overlapping block of memory from one location to another.
 *
 * @param[out] dest  The block of memory to copy to.
 * @param[in]  src   The block of memory to copy from.
 * @param[in]  count The number of bytes to copy.
 * @return A copy of `dest`.
 *
 * @warning The behavior is undefined if `dest` or `src` is not a pointer to a
 * block of memory of at least `count` contiguous bytes, or if `dest` and `src`
 * overlap.
 *
 * @note If `count` is zero, no bytes are copied and the function immediately
 * returns `dest` (which may be `nullptr`). The `src` pointer is ignored in this
 * case (and may be `nullptr` as well).
 *
 * If `dest` and `src` may overlap, use `scu_memmove()` instead, which handles
 * overlapping blocks of memory correctly (but may be less efficient).
 */
SCU_API void* scu_memcpy(
    void* restrict dest,
    const void* restrict src,
    Scuisize count
);

/**
 * @brief Copies a non-overlapping block of memory from one location to another,
 * stopping after a specified byte is copied.
 *
 * @param[out] dest  The block of memory to copy to.
 * @param[in]  src   The block of memory to copy from.
 * @param[in]  c     The byte to stop after copying.
 * @param[in]  count The maximum number of bytes to copy.
 * @return A pointer to the byte after the copy of the specified byte in the
 * destination block of memory, or `nullptr` if the specified byte was not found
 * in the first `count` bytes of the source block of memory.
 *
 * @warning The behavior is undefined if `dest` or `src` is not a pointer to a
 * block of memory of at least `count` contiguous bytes, or if `dest` and `src`
 * overlap.
 *
 * @note If `count` is zero, no bytes are copied and the function immediately
 * returns `nullptr`. The `dest` and `src` pointers are ignored in this case
 * (and may be `nullptr` each).
 *
 * If `dest` and `src` may overlap, use `scu_memmove()` instead, which handles
 * overlapping blocks of memory correctly (but may be less efficient).
 */
SCU_API void* scu_memccpy(
    void* restrict dest,
    const void* restrict src,
    Scubyte c,
    Scuisize count
);

/**
 * @brief Copies a non-overlapping block of memory from one location to another,
 * returning a pointer to the byte after the last byte written.
 *
 * @param[out] dest  The block of memory to copy to.
 * @param[in]  src   The block of memory to copy from.
 * @param[in]  count The number of bytes to copy.
 * @return A pointer to the byte after the last byte written in the destination
 * block of memory (i.e., `dest + count`).
 *
 * @warning The behavior is undefined if `dest` or `src` is not a pointer to a
 * block of memory of at least `count` contiguous bytes, or if `dest` and `src`
 * overlap.
 *
 * @note If `count` is zero, no bytes are copied and the function returns `dest`
 * (which may be `nullptr`). The `src` pointer is ignored in this case (and may
 * be `nullptr` as well).
 *
 * If `dest` and `src` may overlap, use `scu_memmove()` instead, which handles
 * overlapping blocks of memory correctly (but may be less efficient).
 */
SCU_API void* scu_mempcpy(
    void* restrict dest,
    const void* restrict src,
    Scuisize count
);

/**
 * @brief Copies a possibly overlapping block of memory from one location to
 * another.
 *
 * @param[out] dest  The block of memory to copy to.
 * @param[in]  src   The block of memory to copy from.
 * @param[in]  count The number of bytes to copy.
 * @return A copy of `dest`.
 *
 * @warning The behavior is undefined if `dest` or `src` is not a pointer to a
 * block of memory of at least `count` contiguous bytes.
 *
 * @note If `count` is zero, no bytes are copied and the function immediately
 * returns `dest` (which may be `nullptr`). The `src` pointer is ignored in this
 * case (and may be `nullptr` as well).
 *
 * If `dest` and `src` are known to be non-overlapping, use `scu_memcpy()`
 * instead, which is likely to be more efficient.
 */
SCU_API void* scu_memmove(void* dest, const void* src, Scuisize count);

/**
 * @brief Swaps the contents of two non-overlapping blocks of memory.
 *
 * @param[in, out] left  The first block of memory.
 * @param[in, out] right The second block of memory.
 * @param[in]      count The number of bytes to swap.
 *
 * @warning The behavior is undefined if `left` or `right` is not a pointer to a
 * block of memory of at least `count` contiguous bytes, or if `left` and
 * `right` overlap.
 *
 * @note If `count` is zero, no bytes are swapped and the function immediately
 * returns. Both `left` and `right` are ignored in this case (and may be
 * `nullptr` each).
 */
SCU_API void scu_memswap(
    void* restrict left,
    void* restrict right,
    Scuisize count
);

#endif

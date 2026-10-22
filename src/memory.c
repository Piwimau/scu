#define SCU_SHORT_ALIASES

#include <stdbit.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
    #include <malloc.h>
#endif
#include "scu/assert.h"
#include "scu/math.h"
#include "scu/memory.h"
#include "scu/util.h"

/**
 * @brief Allocates an uninitialized block of memory.
 *
 * @param[in, out] context Unused.
 * @param[in]      size    The desired size (in bytes).
 * @param[in]      align   The desired alignment (in bytes).
 * @return A pointer to the block of memory on success, or `nullptr` on failure.
 *
 * @warning The behavior is undefined if `size` is less than or equal to zero,
 * or if `align` is not a power of two.
 */
static void* scu_default_allocate(
    [[maybe_unused]] void* context,
    isize size,
    isize align
) {
    if (align <= SCU_ALIGNOF(max_align_t)) {
        return malloc((usize) size);
    }
#ifdef _WIN32
    return _aligned_malloc((usize) size, (usize) align);
#else
    if (size > ISIZE_MAX - (align - 1)) {
        return nullptr;
    }
    return aligned_alloc((usize) align, (usize) SCU_ALIGN_UP(size, align));
#endif
}

/**
 * @brief Allocates a zero-initialized block of memory.
 *
 * @param[in, out] context Unused.
 * @param[in]      size    The desired size (in bytes).
 * @param[in]      align   The desired alignment (in bytes).
 * @return A pointer to the block of memory on success, or `nullptr` on failure.
 *
 * @warning The behavior is undefined if `size` is less than or equal to zero,
 * or if `align` is not a power of two.
 */
static void* scu_default_allocate_zeroed(
    [[maybe_unused]] void* context,
    isize size,
    isize align
) {
    if (align <= SCU_ALIGNOF(max_align_t)) {
        return calloc(1, (usize) size);
    }
    void* block = scu_default_allocate(nullptr, size, align);
    if (block) {
        scu_memset(block, 0, size);
    }
    return block;
}

/**
 * @brief Deallocates a block of memory.
 *
 * @param[in, out] context Unused.
 * @param[in]      block   The block of memory to deallocate.
 * @param[in]      size    Unused.
 * @param[in]      align   The alignment of the block (in bytes).
 *
 * @note This function behaves similarly to the standard `free()` function,
 * i.e., it accepts a `nullptr` as the block to deallocate, in which case no
 * action is taken.
 */
static void scu_default_deallocate(
    [[maybe_unused]] void* context,
    void* block,
    [[maybe_unused]] isize size,
    [[maybe_unused]] isize align
) {
#ifdef _WIN32
    if (align > SCU_ALIGNOF(max_align_t)) {
        _aligned_free(block);
    }
#endif
    free(block);
}

/**
 * @brief Resizes a block of memory, moving it if an in-place resize is not
 * possible.
 *
 * @param[in, out] context Unused.
 * @param[in]      block   The block of memory to resize.
 * @param[in]      oldSize The current size (in bytes).
 * @param[in]      newSize The desired size (in bytes).
 * @param[in]      align   The desired alignment (in bytes).
 * @return A pointer to the resized (and possibly moved) block of memory on
 * success, or `nullptr` on failure.
 *
 * @warning The behavior is undefined if `oldSize` is less than zero, if `block`
 * is `nullptr` and `oldSize` is not zero, if `newSize` is less than or equal to
 * zero, or if `align` is not a power of two.
 *
 * @note This function behaves similarly to the standard `realloc()` function,
 * i.e., the pointer to the original block of memory should be considered
 * invalid after a successful resize. However, if the operation fails, the
 * pointer to the original block of memory remains valid. Additionally, it
 * accepts a `nullptr` as the block to resize, in which case it behaves like
 * `scu_default_allocate(context, newSize, align)`. In this case, `oldSize`
 * must be zero.
 */
static void* scu_default_resize_or_move(
    [[maybe_unused]] void* context,
    void* block,
    [[maybe_unused]] isize oldSize,
    isize newSize,
    isize align
) {
    if (align <= SCU_ALIGNOF(max_align_t)) {
        return realloc(block, (usize) newSize);
    }
#ifdef _WIN32
    if (!block) {
        return _aligned_malloc((usize) newSize, (usize) align);
    }
    return _aligned_realloc(block, (usize) newSize, (usize) align);
#else
    void* newBlock = scu_default_allocate(nullptr, newSize, align);
    if (newBlock && block) {
        scu_memcpy(newBlock, block, SCU_MIN(oldSize, newSize));
        scu_default_deallocate(nullptr, block, oldSize, align);
    }
    return newBlock;
#endif
}

/** @brief The default allocator. */
static ScuAllocator scuDefaultAllocator = {
    .allocate = scu_default_allocate,
    .allocateZeroed = scu_default_allocate_zeroed,
    .resize = nullptr,
    .resizeOrMove = scu_default_resize_or_move,
    .deallocate = scu_default_deallocate,
    .context = nullptr
};

SCU_API ScuAllocator* scu_default_allocator() {
    return &scuDefaultAllocator;
}

SCU_API void* scu_allocate(isize size, isize align, ScuAllocator* allocator) {
    SCU_ASSERT(size > 0);
    SCU_ASSERT(align > 0 && stdc_has_single_bit((usize) align));
    SCU_ASSERT(allocator);
    return allocator->allocate(allocator->context, size, align);
}

SCU_API void* scu_allocate_zeroed(
    isize size,
    isize align,
    ScuAllocator* allocator
) {
    SCU_ASSERT(size > 0);
    SCU_ASSERT(align > 0 && stdc_has_single_bit((usize) align));
    SCU_ASSERT(allocator);
    if (allocator->allocateZeroed) {
        return allocator->allocateZeroed(allocator->context, size, align);
    }
    void* block = allocator->allocate(allocator->context, size, align);
    if (block) {
        scu_memset(block, 0, size);
    }
    return block;
}

SCU_API bool scu_resize(
    void* block,
    isize oldSize,
    isize newSize,
    isize align,
    ScuAllocator* allocator
) {
    SCU_ASSERT(block);
    SCU_ASSERT(oldSize > 0);
    SCU_ASSERT(newSize > 0);
    SCU_ASSERT(align > 0 && stdc_has_single_bit((usize) align));
    SCU_ASSERT(allocator);
    if (allocator->resize) {
        return allocator->resize(
            allocator->context,
            block,
            oldSize,
            newSize,
            align
        );
    }
    return false;
}

SCU_API void* scu_resize_or_move(
    void* block,
    isize oldSize,
    isize newSize,
    isize align,
    ScuAllocator* allocator
) {
    SCU_ASSERT((!block && oldSize == 0) || oldSize > 0);
    SCU_ASSERT(newSize > 0);
    SCU_ASSERT(align > 0 && stdc_has_single_bit((usize) align));
    SCU_ASSERT(allocator);
    if (allocator->resizeOrMove) {
        return allocator->resizeOrMove(
            allocator->context,
            block,
            oldSize,
            newSize,
            align
        );
    }
    if (!block) {
        return allocator->allocate(allocator->context, newSize, align);
    }
    if (allocator->resize) {
        bool resized = allocator->resize(
            allocator->context,
            block,
            oldSize,
            newSize,
            align
        );
        if (resized) {
            return block;
        }
    }
    void* newBlock = allocator->allocate(allocator->context, newSize, align);
    if (newBlock) {
        scu_memcpy(newBlock, block, SCU_MIN(oldSize, newSize));
        allocator->deallocate(allocator->context, block, oldSize, align);
    }
    return newBlock;
}

SCU_API void scu_deallocate(
    void* block,
    isize size,
    isize align,
    ScuAllocator* allocator
) {
    SCU_ASSERT(size >= 0);
    SCU_ASSERT(align > 0 && stdc_has_single_bit((usize) align));
    SCU_ASSERT(allocator);
    if (block) {
        allocator->deallocate(allocator->context, block, size, align);
    }
}

SCU_API void* scu_memchr(const void* block, byte c, isize count) {
    SCU_ASSERT(count >= 0);
    if (count == 0) {
        return nullptr;
    }
    SCU_ASSERT(block);
    return SCU_CONST_CAST(void*, memchr(block, c, (usize) count));
}

SCU_API void* scu_memrchr(const void* block, byte c, isize count) {
    SCU_ASSERT(count >= 0);
    if (count == 0) {
        return nullptr;
    }
    SCU_ASSERT(block);
    const byte* p = block;
    for (isize i = count - 1; i >= 0; i--) {
        if (p[i] == c) {
            return SCU_CONST_CAST(void*, p + i);
        }
    }
    return nullptr;
}

SCU_API int scu_memcmp(const void* left, const void* right, isize count) {
    SCU_ASSERT(count >= 0);
    if (count == 0) {
        return 0;
    }
    SCU_ASSERT(left);
    SCU_ASSERT(right);
    int cmp = memcmp(left, right, (usize) count);
    return (cmp < 0) ? -1 : (cmp > 0) ? 1 : 0;
}

SCU_API void* scu_memset(void* dest, byte c, isize count) {
    SCU_ASSERT(count >= 0);
    if (count == 0) {
        return dest;
    }
    SCU_ASSERT(dest);
    return memset(dest, c, (usize) count);
}

SCU_API void* scu_memcpy(
    void* restrict dest,
    const void* restrict src,
    isize count
) {
    SCU_ASSERT(count >= 0);
    if (count == 0) {
        return dest;
    }
    SCU_ASSERT(dest);
    SCU_ASSERT(src);
    return memcpy(dest, src, (usize) count);
}

SCU_API void* scu_memccpy(
    void* restrict dest,
    const void* restrict src,
    byte c,
    isize count
) {
    SCU_ASSERT(count >= 0);
    if (count == 0) {
        return nullptr;
    }
    SCU_ASSERT(dest);
    SCU_ASSERT(src);
    return memccpy(dest, src, c, (usize) count);
}

SCU_API void* scu_mempcpy(
    void* restrict dest,
    const void* restrict src,
    isize count
) {
    SCU_ASSERT(count >= 0);
    if (count == 0) {
        return dest;
    }
    SCU_ASSERT(dest);
    SCU_ASSERT(src);
    return (byte*) memcpy(dest, src, (usize) count) + count;
}

SCU_API void* scu_memmove(void* dest, const void* src, isize count) {
    SCU_ASSERT(count >= 0);
    if (count == 0) {
        return dest;
    }
    SCU_ASSERT(dest);
    SCU_ASSERT(src);
    return memmove(dest, src, (usize) count);
}

SCU_API void scu_memswap(
    void* restrict left,
    void* restrict right,
    isize count
) {
    SCU_ASSERT(count >= 0);
    if (count == 0) {
        return;
    }
    SCU_ASSERT(left);
    SCU_ASSERT(right);
    byte* l = left;
    byte* r = right;
    for (isize i = 0; i < count; i++) {
        byte temp = l[i];
        l[i] = r[i];
        r[i] = temp;
    }
}
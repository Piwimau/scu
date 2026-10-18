#ifndef SCU_MEMORY_H
#define SCU_MEMORY_H

#include "scu/types.h"

/**
 * @brief Returns the size of an expression (in bytes) as an `Scuisize`.
 *
 * @param[in] expr The expression to evaluate the size of.
 * @return The size of the expression (in bytes) as an `Scuisize`.
 */
#define SCU_SIZEOF(expr) ((Scuisize) sizeof(expr))

/**
 * @brief Returns the alignment requirement of an expression (in bytes) as an
 * `Scuisize`.
 *
 * @param[in] expr The expression to evaluate the alignment requirement of.
 * @return The alignment requirement of the expression (in bytes) as an
 * `Scuisize`.
 */
#define SCU_ALIGNOF(expr) ((Scuisize) alignof(expr))

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
void* scu_memchr(const void* block, Scubyte c, Scuisize count);

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
void* scu_memrchr(const void* block, Scubyte c, Scuisize count);

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
int scu_memcmp(const void* left, const void* right, Scuisize count);

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
void* scu_memset(void* dest, Scubyte c, Scuisize count);

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
void* scu_memcpy(void* restrict dest, const void* restrict src, Scuisize count);

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
void* scu_memccpy(
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
void* scu_mempcpy(
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
void* scu_memmove(void* dest, const void* src, Scuisize count);

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
void scu_memswap(void* restrict left, void* restrict right, Scuisize count);

#endif
#ifndef SCU_HASH_H
#define SCU_HASH_H

#include "scu/types.h"

/**
 * @brief Returns a hash for a value.
 *
 * @param[in] value A pointer to the value to hash.
 * @return A hash for the value.
 */
typedef Scuusize ScuHashFunc(const void* value);

/**
 * @brief Returns a hash for a `bool` value.
 *
 * @param[in] value A pointer to the value to hash.
 * @return A hash for the value.
 *
 * @warning The behavior is undefined if `value` is not a pointer to a `bool`.
 */
Scuusize scu_hash_bool(const void* value);

/**
 * @brief Returns a hash for an `Scubyte` value.
 *
 * @param[in] value A pointer to the value to hash.
 * @return A hash for the value.
 *
 * @warning The behavior is undefined if `value` is not a pointer to an
 * `Scubyte`.
 */
Scuusize scu_hash_byte(const void* value);

/**
 * @brief Returns a hash for an `Scui8` value.
 *
 * @param[in] value A pointer to the value to hash.
 * @return A hash for the value.
 *
 * @warning The behavior is undefined if `value` is not a pointer to an `Scui8`.
 */
Scuusize scu_hash_i8(const void* value);

/**
 * @brief Returns a hash for an `Scuu8` value.
 *
 * @param[in] value A pointer to the value to hash.
 * @return A hash for the value.
 *
 * @warning The behavior is undefined if `value` is not a pointer to an `Scuu8`.
 */
Scuusize scu_hash_u8(const void* value);

/**
 * @brief Returns a hash for an `Scui16` value.
 *
 * @param[in] value A pointer to the value to hash.
 * @return A hash for the value.
 *
 * @warning The behavior is undefined if `value` is not a pointer to an
 * `Scui16`.
 */
Scuusize scu_hash_i16(const void* value);

/**
 * @brief Returns a hash for an `Scuu16` value.
 *
 * @param[in] value A pointer to the value to hash.
 * @return A hash for the value.
 *
 * @warning The behavior is undefined if `value` is not a pointer to an
 * `Scuu16`.
 */
Scuusize scu_hash_u16(const void* value);

/**
 * @brief Returns a hash for an `Scui32` value.
 *
 * @param[in] value A pointer to the value to hash.
 * @return A hash for the value.
 *
 * @warning The behavior is undefined if `value` is not a pointer to an
 * `Scui32`.
 */
Scuusize scu_hash_i32(const void* value);

/**
 * @brief Returns a hash for an `Scuu32` value.
 *
 * @param[in] value A pointer to the value to hash.
 * @return A hash for the value.
 *
 * @warning The behavior is undefined if `value` is not a pointer to an
 * `Scuu32`.
 */
Scuusize scu_hash_u32(const void* value);

/**
 * @brief Returns a hash for an `Scui64` value.
 *
 * @param[in] value A pointer to the value to hash.
 * @return A hash for the value.
 *
 * @warning The behavior is undefined if `value` is not a pointer to an
 * `Scui64`.
 */
Scuusize scu_hash_i64(const void* value);

/**
 * @brief Returns a hash for an `Scuu64` value.
 *
 * @param[in] value A pointer to the value to hash.
 * @return A hash for the value.
 *
 * @warning The behavior is undefined if `value` is not a pointer to an
 * `Scuu64`.
 */
Scuusize scu_hash_u64(const void* value);

/**
 * @brief Returns a hash for an `Scuiptr` value.
 *
 * @param[in] value A pointer to the value to hash.
 * @return A hash for the value.
 *
 * @warning The behavior is undefined if `value` is not a pointer to an
 * `Scuiptr`.
 */
Scuusize scu_hash_iptr(const void* value);

/**
 * @brief Returns a hash for an `Scuuptr` value.
 *
 * @param[in] value A pointer to the value to hash.
 * @return A hash for the value.
 *
 * @warning The behavior is undefined if `value` is not a pointer to an
 * `Scuuptr`.
 */
Scuusize scu_hash_uptr(const void* value);

/**
 * @brief Returns a hash for an `Scuisize` value.
 *
 * @param[in] value A pointer to the value to hash.
 * @return A hash for the value.
 *
 * @warning The behavior is undefined if `value` is not a pointer to an
 * `Scuisize`.
 */
Scuusize scu_hash_isize(const void* value);

/**
 * @brief Returns a hash for an `Scuusize` value.
 *
 * @param[in] value A pointer to the value to hash.
 * @return A hash for the value.
 *
 * @warning The behavior is undefined if `value` is not a pointer to an
 * `Scuusize`.
 */
Scuusize scu_hash_usize(const void* value);

/**
 * @brief Returns a hash for an `Scuf32` value.
 *
 * @param[in] value A pointer to the value to hash.
 * @return A hash for the value.
 *
 * @warning The behavior is undefined if `value` is not a pointer to an
 * `Scuf32`, or if `*value` is NaN.
 */
Scuusize scu_hash_f32(const void* value);

/**
 * @brief Returns a hash for an `Scuf64` value.
 *
 * @param[in] value A pointer to the value to hash.
 * @return A hash for the value.
 *
 * @warning The behavior is undefined if `value` is not a pointer to an
 * `Scuf64`, or if `*value` is NaN.
 */
Scuusize scu_hash_f64(const void* value);

/**
 * @brief Returns a hash for an `char` value.
 *
 * @param[in] value A pointer to the value to hash.
 * @return A hash for the value.
 *
 * @warning The behavior is undefined if `value` is not a pointer to a `char`.
 */
Scuusize scu_hash_char(const void* value);

/**
 * @brief Returns a hash for an `Scuchar8` value.
 *
 * @param[in] value A pointer to the value to hash.
 * @return A hash for the value.
 *
 * @warning The behavior is undefined if `value` is not a pointer to an
 * `Scuchar8`.
 */
Scuusize scu_hash_char8(const void* value);

/**
 * @brief Returns a hash for an `Scuchar16` value.
 *
 * @param[in] value A pointer to the value to hash.
 * @return A hash for the value.
 *
 * @warning The behavior is undefined if `value` is not a pointer to an
 * `Scuchar16`.
 */
Scuusize scu_hash_char16(const void* value);

/**
 * @brief Returns a hash for an `Scuchar32` value.
 *
 * @param[in] value A pointer to the value to hash.
 * @return A hash for the value.
 *
 * @warning The behavior is undefined if `value` is not a pointer to an
 * `Scuchar32`.
 */
Scuusize scu_hash_char32(const void* value);

/**
 * @brief Returns a hash for a block of memory.
 *
 * @param[in] block A pointer to the block of memory to hash.
 * @param[in] count The size of the block (in bytes).
 * @return A hash for the block of memory.
 *
 * @warning The behavior is undefined if `block` is not a pointer to a block of
 * memory at least `count` bytes.
 *
 * @note If `count` is zero, `block` is ignored and may be `nullptr`.
 */
Scuusize scu_hash_bytes(const void* block, Scuisize count);

/**
 * @brief Returns a hash for a null-terminated byte string.
 *
 * @param[in] value A pointer to a pointer to the null-terminated byte string to
 *                  hash.
 * @return A hash for the null-terminated byte string.
 *
 * @warning The behavior is undefined if `value` is not a pointer to a pointer
 * to a null-terminated byte string. Note that `value` must not be a direct
 * pointer to the string itself, but a pointer to a pointer (i.e., `value` is
 * interpreted as `const char* const*`).
 */
Scuusize scu_hash_str(const void* value);

/**
 * @brief Combines a hash with an accumulator hash.
 *
 * @param[in] seed The accumulator hash.
 * @param[in] hash The hash to mix into the accumulator.
 * @return The combined hash.
 *
 * @note This function is mainly useful for creating hash functions for complex
 * or aggregate data types (such as structs or arrays) by combining the hashes
 * of their individual members or elements into a single hash.
 *
 * For the initial call, zero may be passed as the seed, as shown in the
 * following example:
 *
 * ```c
 * typedef struct Vector {
 *     f32 x;
 *     f32 y;
 * } Vector;
 *
 * Scuusize hash_vector(const void* value) {
 *     SCU_ASSERT(value);
 *     const Vector* vector = (const Vector*) value;
 *     Scuusize hash = 0;
 *     hash = scu_hash_combine(hash, scu_hash_f32(&vector->x));
 *     hash = scu_hash_combine(hash, scu_hash_f32(&vector->y));
 *     return hash;
 * }
 * ```
 */
Scuusize scu_hash_combine(Scuusize seed, Scuusize hash);

#endif
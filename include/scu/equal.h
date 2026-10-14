#ifndef SCU_EQUAL_H
#define SCU_EQUAL_H

/**
 * @brief Determines whether two values are equal.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return `true` if `*lhs` and `*rhs` are equal, `false` otherwise.
 */
typedef bool ScuEqualFunc(const void* lhs, const void* rhs);

/**
 * @brief Determines whether two `bool` values are equal.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return `true` if `*lhs` and `*rhs` are equal, `false` otherwise.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to a
 * `bool`.
 */
bool scu_equal_bool(const void* lhs, const void* rhs);

/**
 * @brief Determines whether two `Scubyte` values are equal.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return `true` if `*lhs` and `*rhs` are equal, `false` otherwise.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scubyte`.
 */
bool scu_equal_byte(const void* lhs, const void* rhs);

/**
 * @brief Determines whether two `Scui8` values are equal.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return `true` if `*lhs` and `*rhs` are equal, `false` otherwise.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scui8`.
 */
bool scu_equal_i8(const void* lhs, const void* rhs);

/**
 * @brief Determines whether two `Scuu8` values are equal.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return `true` if `*lhs` and `*rhs` are equal, `false` otherwise.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scuu8`.
 */
bool scu_equal_u8(const void* lhs, const void* rhs);

/**
 * @brief Determines whether two `Scui16` values are equal.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return `true` if `*lhs` and `*rhs` are equal, `false` otherwise.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scui16`.
 */
bool scu_equal_i16(const void* lhs, const void* rhs);

/**
 * @brief Determines whether two `Scuu16` values are equal.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return `true` if `*lhs` and `*rhs` are equal, `false` otherwise.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scuu16`.
 */
bool scu_equal_u16(const void* lhs, const void* rhs);

/**
 * @brief Determines whether two `Scui32` values are equal.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return `true` if `*lhs` and `*rhs` are equal, `false` otherwise.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scui32`.
 */
bool scu_equal_i32(const void* lhs, const void* rhs);

/**
 * @brief Determines whether two `Scuu32` values are equal.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return `true` if `*lhs` and `*rhs` are equal, `false` otherwise.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scuu32`.
 */
bool scu_equal_u32(const void* lhs, const void* rhs);

/**
 * @brief Determines whether two `Scui64` values are equal.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return `true` if `*lhs` and `*rhs` are equal, `false` otherwise.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scui64`.
 */
bool scu_equal_i64(const void* lhs, const void* rhs);

/**
 * @brief Determines whether two `Scuu64` values are equal.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return `true` if `*lhs` and `*rhs` are equal, `false` otherwise.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scuu64`.
 */
bool scu_equal_u64(const void* lhs, const void* rhs);

/**
 * @brief Determines whether two `Scuiptr` values are equal.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return `true` if `*lhs` and `*rhs` are equal, `false` otherwise.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scuiptr`.
 */
bool scu_equal_iptr(const void* lhs, const void* rhs);

/**
 * @brief Determines whether two `Scuuptr` values are equal.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return `true` if `*lhs` and `*rhs` are equal, `false` otherwise.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scuuptr`.
 */
bool scu_equal_uptr(const void* lhs, const void* rhs);

/**
 * @brief Determines whether two `Scuisize` values are equal.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return `true` if `*lhs` and `*rhs` are equal, `false` otherwise.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scuisize`.
 */
bool scu_equal_isize(const void* lhs, const void* rhs);

/**
 * @brief Determines whether two `Scuusize` values are equal.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return `true` if `*lhs` and `*rhs` are equal, `false` otherwise.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scuusize`.
 */
bool scu_equal_usize(const void* lhs, const void* rhs);

/**
 * @brief Determines whether two `Scuf32` values are equal.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return `true` if `*lhs` and `*rhs` are equal, `false` otherwise.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scuf32`, or if either `*lhs` or `*rhs` is NaN.
 *
 * @note This function considers two `Scuf32` values equal if they are exactly
 * equal according to the `==` operator. It is intended for use in collections
 * that require an equality function, but not as a general-purpose (approximate)
 * floating-point comparison function.
 */
bool scu_equal_f32(const void* lhs, const void* rhs);

/**
 * @brief Determines whether two `Scuf64` values are equal.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return `true` if `*lhs` and `*rhs` are equal, `false` otherwise.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scuf64`, or if either `*lhs` or `*rhs` is NaN.
 *
 * @note This function considers two `Scuf64` values equal if they are exactly
 * equal according to the `==` operator. It is intended for use in collections
 * that require an equality function, but not as a general-purpose (approximate)
 * floating-point comparison function.
 */
bool scu_equal_f64(const void* lhs, const void* rhs);

/**
 * @brief Determines whether two `char` values are equal.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return `true` if `*lhs` and `*rhs` are equal, `false` otherwise.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to a
 * `char`.
 */
bool scu_equal_char(const void* lhs, const void* rhs);

/**
 * @brief Determines whether two `Scuchar8` values are equal.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return `true` if `*lhs` and `*rhs` are equal, `false` otherwise.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scuchar8`.
 */
bool scu_equal_char8(const void* lhs, const void* rhs);

/**
 * @brief Determines whether two `Scuchar16` values are equal.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return `true` if `*lhs` and `*rhs` are equal, `false` otherwise.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scuchar16`.
 */
bool scu_equal_char16(const void* lhs, const void* rhs);

/**
 * @brief Determines whether two `Scuchar32` values are equal.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return `true` if `*lhs` and `*rhs` are equal, `false` otherwise.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scuchar32`.
 */
bool scu_equal_char32(const void* lhs, const void* rhs);

/**
 * @brief Determines whether two null-terminated byte strings are equal.
 *
 * @param[in] lhs A pointer to a pointer to the first null-terminated byte
 *                string.
 * @param[in] rhs A pointer to a pointer to the second null-terminated byte
 *                string.
 * @return `true` if the null-terminated byte strings pointed to by `*lhs` and
 * `*rhs` are equal, `false` otherwise.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to a
 * pointer to a null-terminated byte string. Note that `lhs` and `rhs` must
 * not be direct pointers to the strings themselves, but pointers to pointers
 * (i.e., `lhs` and `rhs` are interpreted as `const char* const*`).
 *
 * @note This function compares the strings solely based on their byte
 * representation. For strings that use a multi-byte character encoding (e.g.,
 * UTF-8), the return value may be misleading, for example if certain characters
 * considered equal can be represented by different byte sequences.
 */
bool scu_equal_str(const void* lhs, const void* rhs);

#endif
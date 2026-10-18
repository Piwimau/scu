#ifndef SCU_COMPARE_H
#define SCU_COMPARE_H

/**
 * @brief Compares two values.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return A negative value if `*lhs` is less than `*rhs`, zero if they compare
 * equal, or a positive value if `*lhs` is greater than `*rhs`.
 */
typedef int ScuCompareFunc(const void* lhs, const void* rhs);

/**
 * @brief Compares two `bool` values.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return A negative value if `*lhs` is `false` and `*rhs` is `true`, zero if
 * they compare equal, or a positive value if `*lhs` is `true` and `*rhs` is
 * `false`.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to a
 * `bool`.
 */
int scu_compare_bool(const void* lhs, const void* rhs);

/**
 * @brief Compares two `Scubyte` values.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return A negative value if `*lhs` is less than `*rhs`, zero if they compare
 * equal, or a positive value if `*lhs` is greater than `*rhs`.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scubyte`.
 */
int scu_compare_byte(const void* lhs, const void* rhs);

/**
 * @brief Compares two `Scui8` values.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return A negative value if `*lhs` is less than `*rhs`, zero if they compare
 * equal, or a positive value if `*lhs` is greater than `*rhs`.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scui8`.
 */
int scu_compare_i8(const void* lhs, const void* rhs);

/**
 * @brief Compares two `Scuu8` values.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return A negative value if `*lhs` is less than `*rhs`, zero if they compare
 * equal, or a positive value if `*lhs` is greater than `*rhs`.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scuu8`.
 */
int scu_compare_u8(const void* lhs, const void* rhs);

/**
 * @brief Compares two `Scui16` values.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return A negative value if `*lhs` is less than `*rhs`, zero if they compare
 * equal, or a positive value if `*lhs` is greater than `*rhs`.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scui16`.
 */
int scu_compare_i16(const void* lhs, const void* rhs);

/**
 * @brief Compares two `Scuu16` values.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return A negative value if `*lhs` is less than `*rhs`, zero if they compare
 * equal, or a positive value if `*lhs` is greater than `*rhs`.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scuu16`.
 */
int scu_compare_u16(const void* lhs, const void* rhs);

/**
 * @brief Compares two `Scui32` values.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return A negative value if `*lhs` is less than `*rhs`, zero if they compare
 * equal, or a positive value if `*lhs` is greater than `*rhs`.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scui32`.
 */
int scu_compare_i32(const void* lhs, const void* rhs);

/**
 * @brief Compares two `Scuu32` values.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return A negative value if `*lhs` is less than `*rhs`, zero if they compare
 * equal, or a positive value if `*lhs` is greater than `*rhs`.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scuu32`.
 */
int scu_compare_u32(const void* lhs, const void* rhs);

/**
 * @brief Compares two `Scui64` values.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return A negative value if `*lhs` is less than `*rhs`, zero if they compare
 * equal, or a positive value if `*lhs` is greater than `*rhs`.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scui64`.
 */
int scu_compare_i64(const void* lhs, const void* rhs);

/**
 * @brief Compares two `Scuu64` values.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return A negative value if `*lhs` is less than `*rhs`, zero if they compare
 * equal, or a positive value if `*lhs` is greater than `*rhs`.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scuu64`.
 */
int scu_compare_u64(const void* lhs, const void* rhs);

/**
 * @brief Compares two `Scuiptr` values.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return A negative value if `*lhs` is less than `*rhs`, zero if they compare
 * equal, or a positive value if `*lhs` is greater than `*rhs`.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scuiptr`.
 */
int scu_compare_iptr(const void* lhs, const void* rhs);

/**
 * @brief Compares two `Scuuptr` values.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return A negative value if `*lhs` is less than `*rhs`, zero if they compare
 * equal, or a positive value if `*lhs` is greater than `*rhs`.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scuuptr`.
 */
int scu_compare_uptr(const void* lhs, const void* rhs);

/**
 * @brief Compares two `Scuisize` values.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return A negative value if `*lhs` is less than `*rhs`, zero if they compare
 * equal, or a positive value if `*lhs` is greater than `*rhs`.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scuisize`.
 */
int scu_compare_isize(const void* lhs, const void* rhs);

/**
 * @brief Compares two `Scuusize` values.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return A negative value if `*lhs` is less than `*rhs`, zero if they compare
 * equal, or a positive value if `*lhs` is greater than `*rhs`.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scuusize`.
 */
int scu_compare_usize(const void* lhs, const void* rhs);

/**
 * @brief Compares two `Scuf32` values.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return A negative value if `*lhs` is less than `*rhs`, zero if they compare
 * equal, or a positive value if `*lhs` is greater than `*rhs`.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scuf32`, or if either `*lhs` or `*rhs` is NaN.
 */
int scu_compare_f32(const void* lhs, const void* rhs);

/**
 * @brief Compares two `Scuf64` values.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return A negative value if `*lhs` is less than `*rhs`, zero if they compare
 * equal, or a positive value if `*lhs` is greater than `*rhs`.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scuf64`, or if either `*lhs` or `*rhs` is NaN.
 */
int scu_compare_f64(const void* lhs, const void* rhs);

/**
 * @brief Compares two `char` values.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return A negative value if `*lhs` is less than `*rhs`, zero if they compare
 * equal, or a positive value if `*lhs` is greater than `*rhs`.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to a
 * `char`.
 */
int scu_compare_char(const void* lhs, const void* rhs);

/**
 * @brief Compares two `Scuchar8` values.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return A negative value if `*lhs` is less than `*rhs`, zero if they compare
 * equal, or a positive value if `*lhs` is greater than `*rhs`.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scuchar8`.
 */
int scu_compare_char8(const void* lhs, const void* rhs);

/**
 * @brief Compares two `Scuchar16` values.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return A negative value if `*lhs` is less than `*rhs`, zero if they compare
 * equal, or a positive value if `*lhs` is greater than `*rhs`.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scuchar16`.
 */
int scu_compare_char16(const void* lhs, const void* rhs);

/**
 * @brief Compares two `Scuchar32` values.
 *
 * @param[in] lhs A pointer to the first value.
 * @param[in] rhs A pointer to the second value.
 * @return A negative value if `*lhs` is less than `*rhs`, zero if they compare
 * equal, or a positive value if `*lhs` is greater than `*rhs`.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to an
 * `Scuchar32`.
 */
int scu_compare_char32(const void* lhs, const void* rhs);

/**
 * @brief Compares two null-terminated byte strings lexicographically.
 *
 * @param[in] lhs A pointer to a pointer to the first null-terminated byte
 * string.
 * @param[in] rhs A pointer to a pointer to the second null-terminated byte
 *              string.
 * @return A negative value if `*lhs` appears before `*rhs` in lexicographical
 * order, zero if they compare equal, or a positive value if `*lhs` appears
 * after
 * `*rhs`.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to a
 * pointer to a null-terminated byte string. Note that `lhs` and `rhs` must not
 * be direct pointers to the strings themselves, but pointers to pointers (i.e.,
 * `lhs` and `rhs` are interpreted as `const char* const*`).
 *
 * @note This function compares the strings solely based on their byte
 * representation. For strings that use a multi-byte character encoding (e.g.,
 * UTF-8), the return value may be misleading, for example if certain characters
 * considered equal can be represented by different byte sequences.
 */
int scu_compare_str(const void* lhs, const void* rhs);

#endif
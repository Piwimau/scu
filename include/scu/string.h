#ifndef SCU_STRING_H
#define SCU_STRING_H

#include "scu/api.h"
#include "scu/types.h"

/**
 * @brief Returns the length of a null-terminated byte string.
 *
 * @param[in] s The null-terminated byte string to examine.
 * @return The number of bytes in `s`, excluding the terminating null byte.
 *
 * @warning The behavior is undefined if `s` is not a pointer to a
 * null-terminated byte string.
 *
 * @note The length is measured in bytes, not characters. For strings that use a
 * multi-byte character encoding (e.g., UTF-8), it does not necessarily
 * correspond to the actual number of characters in the string.
 */
SCU_API Scuisize scu_strlen(const char* s);

/**
 * @brief Returns the length of a byte string, or `count` if no null byte is
 * found within the first `count` bytes of the byte string.
 *
 * @param[in] s     The byte string to examine.
 * @param[in] count The maximum number of bytes to examine.
 * @return The number of bytes in `s`, excluding the terminating null byte, or
 * `count` if no null byte is found within the first `count` bytes.
 *
 * @warning The behavior is undefined if `s` is not a pointer to a byte string
 * with at least `count` bytes.
 *
 * @note The length is measured in bytes, not characters. For strings that use a
 * multi-byte character encoding (e.g., UTF-8), it does not necessarily
 * correspond to the actual number of characters in the string.
 *
 * If `count` is zero, `s` is ignored (it may even be `nullptr`), and the
 * function returns zero.
 */
SCU_API Scuisize scu_strnlen(const char* s, Scuisize count);

/**
 * @brief Compares two null-terminated byte strings lexicographically.
 *
 * @param[in] lhs The first null-terminated byte string.
 * @param[in] rhs The second null-terminated byte string.
 * @return A negative value if `lhs` appears before `rhs` in lexicographical
 * order, zero if they compare equal, or a positive value if `lhs` appears after
 * `rhs`.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to a
 * null-terminated byte string.
 *
 * @note This function compares the strings solely based on their byte
 * representation. For strings that use a multi-byte character encoding (e.g.,
 * UTF-8), the return value may be misleading, for example if certain characters
 * considered equal can be represented by different byte sequences.
 */
SCU_API int scu_strcmp(const char* lhs, const char* rhs);

/**
 * @brief Compares a span of two null-terminated byte strings lexicographically.
 *
 * @param[in] lhs   The first null-terminated byte string.
 * @param[in] rhs   The second null-terminated byte string.
 * @param[in] count The maximum number of bytes to compare.
 * @return A negative value if `lhs` appears before `rhs` in lexicographical
 * order, zero if they compare equal or if `count` is zero, or a positive value
 * if `lhs` appears after `rhs`.
 *
 * @warning The behavior is undefined if `lhs` or `rhs` is not a pointer to a
 * null-terminated byte string.
 *
 * @note This function compares the strings solely based on their byte
 * representation. For strings that use a multi-byte character encoding (e.g.,
 * UTF-8), the return value may be misleading, for example if certain characters
 * considered equal can be represented by different byte sequences.
 *
 * If `count` is zero, `lhs` and `rhs` are ignored (each may even be `nullptr`),
 * and the function returns zero.
 */
SCU_API int scu_strncmp(const char* lhs, const char* rhs, Scuisize count);

/**
 * @brief Returns the index of the first occurrence of a byte in a
 * null-terminated byte string.
 *
 * @param[in] s The null-terminated byte string to examine.
 * @param[in] c The byte to search for.
 * @return The zero-based index of the first occurrence of `c` in `s`, or `-1`
 * if `c` is not found.
 *
 * @warning The behavior is undefined if `s` is not a pointer to a
 * null-terminated byte string.
 *
 * @note The terminating null byte is not compared.
 */
SCU_API Scuisize scu_str_index_of_byte(const char* s, char c);

/**
 * @brief Returns the index of the first occurrence of a substring in a
 * null-terminated byte string.
 *
 * @param[in] s     The null-terminated byte string to examine.
 * @param[in] other The null-terminated byte string to search for.
 * @return The zero-based index of the first occurrence of `other` in `s`, or
 * `-1` if `other` is not found.
 *
 * @warning The behavior is undefined if `s` or `other` is not a pointer to a
 * null-terminated byte string.
 *
 * @note This function compares the strings solely based on their byte
 * representation. For strings that use a multi-byte character encoding (e.g.,
 * UTF-8), the return value may be misleading, for example if certain characters
 * considered equal can be represented by different byte sequences.
 *
 * The terminating null bytes are not compared.
 *
 * If `other` points to an empty byte string, zero is returned (as the empty
 * byte string is considered to be the prefix of any byte string).
 */
SCU_API Scuisize scu_str_index_of_str(const char* s, const char* other);

/**
 * @brief Returns the index of the first occurrence of a byte or substring in a
 * null-terminated byte string.
 *
 * @param[in] s     The null-terminated byte string to examine.
 * @param[in] other The byte or null-terminated byte string to search for.
 * @return The zero-based index of the first occurrence of `other` in `s`, or
 * `-1` if `other` is not found.
 *
 * @warning The behavior is undefined if `s` not a pointer to a null-terminated
 * byte string or if `other` is neither a `char`, an `int`, nor a pointer to a
 * null-terminated byte string.
 *
 * @note This is a type-generic macro that resolves to either
 * `scu_str_index_of_byte()` or `scu_str_index_of_str()` depending on the type
 * of `other`. For details on the semantics of these functions, see their
 * respective documentation.
 */
#define scu_str_index_of(s, other)   \
    _Generic(                        \
        (typeof_unqual(other)) { },  \
        char: scu_str_index_of_byte, \
        int: scu_str_index_of_byte,  \
        char*: scu_str_index_of_str  \
    )(s, other)

/**
 * @brief Returns the index of the last occurrence of a byte in a
 * null-terminated byte string.
 *
 * @param[in] s The null-terminated byte string to examine.
 * @param[in] c The byte to search for.
 * @return The zero-based index of the last occurrence of `c` in `s`, or `-1` if
 * `c` is not found.
 *
 * @warning The behavior is undefined if `s` is not a pointer to a
 * null-terminated byte string.
 *
 * @note The terminating null byte is not compared.
 */
SCU_API Scuisize scu_str_last_index_of_byte(const char* s, char c);

/**
 * @brief Returns the index of the last occurrence of a substring in a
 * null-terminated byte string.
 *
 * @param[in] s     The null-terminated byte string to examine.
 * @param[in] other The null-terminated byte string to search for.
 * @return The zero-based index of the last occurrence of `other` in `s`, or
 * `-1` if `other` is not found.
 *
 * @warning The behavior is undefined if `s` or `other` is not a pointer to a
 * null-terminated byte string.
 *
 * @note This function compares the strings solely based on their byte
 * representation. For strings that use a multi-byte character encoding (e.g.,
 * UTF-8), the return value may be misleading, for example if certain characters
 * considered equal can be represented by different byte sequences.
 *
 * The terminating null bytes are not compared.
 *
 * If `other` points to an empty byte string, the length of `s` (i.e.,
 * `scu_strlen(s)`) is returned (as the empty byte string is considered to be
 * the suffix of any byte string).
 */
SCU_API Scuisize scu_str_last_index_of_str(const char* s, const char* other);

/**
 * @brief Returns the index of the last occurrence of a byte or substring in a
 * null-terminated byte string.
 *
 * @param[in] s     The null-terminated byte string to examine.
 * @param[in] other The byte or null-terminated byte string to search for.
 * @return The zero-based index of the last occurrence of `other` in `s`, or
 * `-1` if `other` is not found.
 *
 * @warning The behavior is undefined if `s` not a pointer to a null-terminated
 * byte string or if `other` is neither a `char`, an `int`, nor a pointer to a
 * null-terminated byte string.
 *
 * @note This is a type-generic macro that resolves to either
 * `scu_str_last_index_of_byte()` or `scu_str_last_index_of_str()` depending on
 * the type of `other`. For details on the semantics of these functions, see
 * their respective documentation.
 */
#define scu_str_last_index_of(s, other)   \
    _Generic(                             \
        (typeof_unqual(other)) { },       \
        char: scu_str_last_index_of_byte, \
        int: scu_str_last_index_of_byte,  \
        char*: scu_str_last_index_of_str  \
    )(s, other)

/**
 * @brief Returns the index of the first occurrence of any byte from a
 * null-terminated byte string in another null-terminated byte string.
 *
 * @param[in] s     The null-terminated byte string to examine.
 * @param[in] anyOf The null-terminated byte string containing the bytes to
 *                  search for.
 * @return The zero-based index of the first occurrence of any byte from `anyOf`
 * in `s`, or `-1` if no such byte is found.
 *
 * @warning The behavior is undefined if `s` or `anyOf` is not a pointer to a
 * null-terminated byte string.
 *
 * @note The terminating null bytes are not compared.
 *
 * If `anyOf` points to an empty byte string, `-1` is returned (as there are no
 * bytes to find).
 */
SCU_API Scuisize scu_str_index_of_any(const char* s, const char* anyOf);

/**
 * @brief Returns the index of the last occurrence of any byte from a
 * null-terminated byte string in another null-terminated byte string.
 *
 * @param[in] s     The null-terminated byte string to examine.
 * @param[in] anyOf The null-terminated byte string containing the bytes to
 *                  search for.
 * @return The zero-based index of the last occurrence of any byte from `anyOf`
 * in `s`, or `-1` if no such byte is found.
 *
 * @warning The behavior is undefined if `s` or `anyOf` is not a pointer to a
 * null-terminated byte string.
 *
 * @note The terminating null bytes are not compared.
 *
 * If `anyOf` points to an empty byte string, `-1` is returned (as there are no
 * bytes to find).
 */
SCU_API Scuisize scu_str_last_index_of_any(const char* s, const char* anyOf);

/**
 * @brief Returns the index of the first occurrence of any byte within an
 * inclusive range in a null-terminated byte string.
 *
 * @param[in] s             The null-terminated byte string to examine.
 * @param[in] lowInclusive  The lower bound of the inclusive byte range.
 * @param[in] highInclusive The upper bound of the inclusive byte range, which
 *                          must be greater than or equal to `lowInclusive`.
 * @return The zero-based index of the first occurrence of any byte within the
 * inclusive range in `s`, or `-1` if no such byte is found.
 *
 * @warning The behavior is undefined if `s` is not a pointer to a
 * null-terminated byte string.
 *
 * @note The terminating null byte is not compared.
 */
SCU_API Scuisize scu_str_index_in_range(
    const char* s,
    char lowInclusive,
    char highInclusive
);

/**
 * @brief Returns the index of the last occurrence of any byte within an
 * inclusive range in a null-terminated byte string.
 *
 * @param[in] s             The null-terminated byte string to examine.
 * @param[in] lowInclusive  The lower bound of the inclusive byte range.
 * @param[in] highInclusive The upper bound of the inclusive byte range, which
 *                          must be greater than or equal to `lowInclusive`.
 * @return The zero-based index of the last occurrence of any byte within the
 * inclusive range in `s`, or `-1` if no such byte is found.
 *
 * @warning The behavior is undefined if `s` is not a pointer to a
 * null-terminated byte string.
 *
 * @note The terminating null byte is not compared.
 */
SCU_API Scuisize scu_str_last_index_in_range(
    const char* s,
    char lowInclusive,
    char highInclusive
);

/**
 * @brief Determines whether a null-terminated byte string starts with a
 * specified prefix byte.
 *
 * @param[in] s The null-terminated byte string to examine.
 * @param[in] c The byte to find as a prefix.
 * @return `true` if `s` starts with `c`, `false` otherwise.
 *
 * @warning The behavior is undefined if `s` is not a pointer to a
 * null-terminated byte string.
 *
 * @note The terminating null byte is not compared.
 */
SCU_API bool scu_str_starts_with_byte(const char* s, char c);

/**
 * @brief Determines whether a null-terminated byte string starts with a
 * specified prefix string.
 *
 * @param[in] s      The null-terminated byte string to examine.
 * @param[in] prefix The null-terminated byte string to find as a prefix.
 * @return `true` if `s` starts with `prefix`, `false` otherwise.
 *
 * @warning The behavior is undefined if `s` or `prefix` is not a pointer to a
 * null-terminated byte string.
 *
 * @note This function compares the strings solely based on their byte
 * representation. For strings that use a multi-byte character encoding (e.g.,
 * UTF-8), the return value may be misleading, for example if certain characters
 * considered equal can be represented by different byte sequences.
 *
 * If `prefix` points to an empty byte string, `true` is returned (as the empty
 * byte string is considered to be the prefix of any byte string).
 */
SCU_API bool scu_str_starts_with_str(const char* s, const char* prefix);

/**
 * @brief Determines whether a null-terminated byte string starts with a
 * specified byte or prefix string.
 *
 * @param[in] s      The null-terminated byte string to examine.
 * @param[in] prefix The byte or null-terminated byte string to find as a
 *                   prefix.
 * @return `true` if `s` starts with `prefix`, `false` otherwise.
 *
 * @warning The behavior is undefined if `s` is not a pointer to a
 * null-terminated byte string or if `prefix` is neither a `char`, an `int`, nor
 * a pointer to a null-terminated byte string.
 *
 * @note This is a type-generic macro that resolves to either
 * `scu_str_starts_with_byte()` or `scu_str_starts_with_str()` depending on the
 * type of `prefix`. For details on the semantics of these functions, see their
 * respective documentation.
 */
#define scu_str_starts_with(s, prefix)  \
    _Generic(                           \
        (typeof_unqual(prefix)) { },    \
        char: scu_str_starts_with_byte, \
        int: scu_str_starts_with_byte,  \
        char*: scu_str_starts_with_str  \
    )(s, prefix)

/**
 * @brief Determines whether a null-terminated byte string ends with a specified
 * suffix byte.
 *
 * @param[in] s The null-terminated byte string to examine.
 * @param[in] c The byte to find as a suffix.
 * @return `true` if `s` ends with `c`, `false` otherwise.
 *
 * @warning The behavior is undefined if `s` is not a pointer to a
 * null-terminated byte string.
 *
 * @note The terminating null byte is not compared.
 */
SCU_API bool scu_str_ends_with_byte(const char* s, char c);

/**
 * @brief Determines whether a null-terminated byte string ends with a specified
 * suffix string.
 *
 * @param[in] s      The null-terminated byte string to examine.
 * @param[in] suffix The null-terminated byte string to find as a suffix.
 * @return `true` if `s` ends with `suffix`, `false` otherwise.
 *
 * @warning The behavior is undefined if `s` or `suffix` is not a pointer to a
 * null-terminated byte string.
 *
 * @note This function compares the strings solely based on their byte
 * representation. For strings that use a multi-byte character encoding (e.g.,
 * UTF-8), the return value may be misleading, for example if certain characters
 * considered equal can be represented by different byte sequences.
 *
 * If `suffix` points to an empty byte string, `true` is returned (as the empty
 * byte string is considered to be the suffix of any byte string).
 */
SCU_API bool scu_str_ends_with_str(const char* s, const char* suffix);

/**
 * @brief Determines whether a null-terminated byte string ends with a specified
 * byte or suffix string.
 *
 * @param[in] s      The null-terminated byte string to examine.
 * @param[in] suffix The byte or null-terminated byte string to find as a
 *                   suffix.
 * @return `true` if `s` ends with `suffix`, `false` otherwise.
 *
 * @warning The behavior is undefined if `s` is not a pointer to a
 * null-terminated byte string or if `suffix` is neither a `char`, an `int`, nor
 * a pointer to a null-terminated byte string.
 *
 * @note This is a type-generic macro that resolves to either
 * `scu_str_ends_with_byte()` or `scu_str_ends_with_str()` depending on the type
 * of `suffix`. For details on the semantics of these functions, see their
 * respective documentation.
 */
#define scu_str_ends_with(s, suffix)  \
    _Generic(                         \
        (typeof_unqual(suffix)) { },  \
        char: scu_str_ends_with_byte, \
        int: scu_str_ends_with_byte,  \
        char*: scu_str_ends_with_str  \
    )(s, suffix)

/**
 * @brief Returns a pointer to a dynamically allocated copy of a null-terminated
 * byte string.
 *
 * @param[in] src The null-terminated byte string to duplicate.
 * @return A pointer to the dynamically allocated copy of the null-terminated
 * byte string, or `nullptr` on failure.
 *
 * @warning The behavior is undefined if `src` is not a pointer to a
 * null-terminated byte string.
 *
 * The caller is responsible for deallocating the memory with the default
 * allocator when it is no longer needed. The simplest way to accomplish this
 * is by calling `SCU_ARRAY_DELETE(s, scu_strlen(s) + 1)`, where `s` is the
 * pointer returned by this function.
 */
SCU_API char* scu_strdup(const char* src);

/**
 * @brief Returns a pointer to a dynamically allocated copy of a span of a
 * null-terminated byte string.
 *
 * This function returns a pointer to a dynamically allocated null-terminated
 * byte string, which contains up to `count` bytes copied from the
 * null-terminated byte string pointed to by `src`. If the length of `src`
 * (i.e., `scu_strlen(src)`) is less than `count`, the entire string is copied,
 * including the terminating null byte. Otherwise, `count` bytes are copied, and
 * a terminating null byte is appended to the end of the new byte string.
 *
 * @param[in] src   The null-terminated byte string to duplicate a span of.
 * @param[in] count The maximum number of bytes to copy from `src`.
 * @return A pointer to the dynamically allocated copy of the span of the
 * null-terminated byte string, or `nullptr` on failure.
 *
 * @warning The behavior is undefined if `src` is not a pointer to a
 * null-terminated byte string.
 *
 * The caller is responsible for deallocating the memory with the default
 * allocator when it is no longer needed. The simplest way to accomplish this
 * is by calling `SCU_ARRAY_DELETE(s, scu_strlen(s) + 1)`, where `s` is the
 * pointer returned by this function.
 *
 * @note If `count` is zero, `src` is ignored (it may even be `nullptr`), and
 * the function returns a pointer to a dynamically allocated empty byte string.
 */
SCU_API char* scu_strndup(const char* src, Scuisize count);

/**
 * @brief Copies at most `min(size - 1, count)` bytes from a null-terminated
 * byte string to a destination buffer.
 *
 * This function copies bytes from the null-terminated byte string pointed to by
 * `src` to the buffer pointed to by `dest`. It stops when either `size - 1` or
 * `count` bytes have been copied (whichever is smaller), or when a null byte is
 * encountered in `src`. If `size` is greater than zero, a terminating null byte
 * is appended to `dest`.
 *
 * @param[out] dest  The destination buffer to copy to.
 * @param[in]  size  The size of the destination buffer (in bytes, including
 *                   space for the terminating null byte).
 * @param[in]  src   The null-terminated byte string to copy from.
 * @param[in]  count The maximum number of bytes to copy from `src`.
 * @return The number of bytes copied from `src` to `dest`, excluding the
 * terminating null byte.
 *
 * @warning The behavior is undefined if `dest` is not a pointer to a buffer
 * with a capacity of at least `size` bytes, if `src` is not a pointer to a
 * null-terminated byte string, or if `dest` and `src` overlap.
 *
 * @note If `size` is zero, `dest` and `src` are ignored (each may even be
 * `nullptr`), and the function returns zero.
 *
 * If `count` is zero, `src` is ignored (it may even be `nullptr`), and the
 * function writes a terminating null byte to `dest[0]` if `size` is greater
 * than zero. In this case, zero is returned as well.
 *
 * If the null-terminated byte string pointed to by `src` is larger than the
 * destination buffer pointed to by `dest`, the copy of `src` is truncated to
 * guarantee that `dest` is always null-terminated.
 */
SCU_API Scuisize scu_strncpy(
    char* restrict dest,
    Scuisize size,
    const char* restrict src,
    Scuisize count
);

/**
 * @brief Appends at most `min(size - scu_strlen(dest) - 1, count)` bytes from a
 * null-terminated byte string to the end of another null-terminated byte
 * string.
 *
 * This function appends bytes from the null-terminated byte string pointed to
 * by `src` to the end of the null-terminated byte string pointed to by `dest`.
 * It stops when either `size - scu_strlen(dest) - 1` or `count` bytes have been
 * appended (whichever is smaller), or when a null byte is encountered in `src`.
 * If `size` is greater than zero and any bytes were appended, a terminating
 * null byte is appended to `dest`.
 *
 * @param[out] dest  The destination buffer to append to, which must contain a
 *                   null-terminated byte string.
 * @param[in]  size  The size of the destination buffer (in bytes, including
 *                   space for the terminating null byte).
 * @param[in]  src   The null-terminated byte string to append from.
 * @param[in]  count The maximum number of bytes to append from `src`.
 * @return The number of bytes appended from `src` to `dest`, excluding the
 * terminating null byte.
 *
 * @warning The behavior is undefined if `dest` is not a pointer to a buffer
 * with a capacity of at least `size` bytes, if `dest` or `src` is not a pointer
 * to a null-terminated byte string, or if `dest` and `src` overlap.
 *
 * @note If `size` or `count` is zero, `dest` and `src` are ignored (each may
 * even be `nullptr`), and the function returns zero.
 *
 * If the null-terminated byte string pointed to by `src` is larger than the
 * remaining space in the destination buffer pointed to by `dest`, the copy of
 * `src` is truncated to guarantee that `dest` is always null-terminated.
 */
SCU_API Scuisize scu_strncat(
    char* restrict dest,
    Scuisize size,
    const char* restrict src,
    Scuisize count
);

#endif

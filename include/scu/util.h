#ifndef SCU_UTIL_H
#define SCU_UTIL_H

/**
 * @brief Converts an expression to a string literal.
 *
 * @param[in] expr The expression to convert.
 * @return A string literal representation of `expr`.
 *
 * @note In contrast to `SCU_XSTRINGIFY()`, this macro does not expand `expr`
 * before stringifying it.
 */
#define SCU_STRINGIFY(expr) #expr

/**
 * @brief Expands an expression and converts it to a string literal.
 *
 * @param[in] expr The expression to expand and convert.
 * @return A string literal representation of the expanded `expr`.
 *
 * @note In contrast to `SCU_STRINGIFY()`, this macro expands `expr` before
 * stringifying it.
 */
#define SCU_XSTRINGIFY(expr) SCU_STRINGIFY(expr)

/**
 * @brief Concatenates two tokens to a single token.
 *
 * @param[in] lhs The first token.
 * @param[in] rhs The second token.
 * @return The concatenated token.
 *
 * @note In contrast to `SCU_XCONCAT()`, this macro does not expand `lhs` and
 * `rhs` before concatenating them.
 */
#define SCU_CONCAT(lhs, rhs) lhs##rhs

/**
 * @brief Expands two tokens and concatenates them to a single token.
 *
 * @param[in] lhs The first token.
 * @param[in] rhs The second token.
 * @return The concatenated token after expanding `lhs` and `rhs`.
 *
 * @note In contrast to `SCU_CONCAT()`, this macro expands `lhs` and `rhs`
 * before concatenating them.
 */
#define SCU_XCONCAT(lhs, rhs) SCU_CONCAT(lhs, rhs)

#ifdef _MSC_VER
    #define SCU_CONST_CAST_IMPL(type, ptr)                    \
        __pragma(warning(push))                               \
        __pragma(warning(disable : 4090))                     \
        (type) (ptr)                                          \
        __pragma(warning(pop))
#elif defined(__GNUC__) || defined(__clang__)
    #define SCU_CONST_CAST_IMPL(type, ptr)                    \
        __extension__({                                       \
            _Pragma("GCC diagnostic push")                    \
            _Pragma("GCC diagnostic ignored \"-Wcast-qual\"") \
            (type) (ptr);                                     \
            _Pragma("GCC diagnostic pop")                     \
        })
#else
    #define SCU_CONST_CAST_IMPL(type, ptr) ((type) (ptr))
#endif

/**
 * @brief Casts away the `const` qualifier from a pointer.
 *
 * This macro is intended to be used in situations where you need to cast away
 * the `const` qualifier from a pointer for whatever reason. The following
 * example demonstrates the basic usage of this macro:
 *
 * ```c
 * const char* s = "Hello, World!";
 * some_func(SCU_CONST_CAST(char*, s), ...);
 * ```
 *
 * @note On three of the most common compilers (MSVC, GCC, and Clang), this
 * macro additionally suppresses the compiler warning that would be emitted
 * under normal circumstances when casting away a `const` qualifier.
 *
 * @warning Needless to say, this macro should be used sparingly and with
 * caution, as it can lead to undefined behavior if the resulting pointer is
 * used to modify an object that was originally declared `const`. It exists
 * solely for reasons of interfacing with legacy or non-const-correct APIs and
 * draws attention to the fact that something unusual is going on.
 *
 * @param[in] type The (non-const) type to cast to.
 * @param[in] ptr  The pointer to cast.
 */
#define SCU_CONST_CAST(type, ptr) SCU_CONST_CAST_IMPL(type, ptr)

#endif

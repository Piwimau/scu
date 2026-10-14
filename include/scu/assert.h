#ifndef SCU_ASSERT_H
#define SCU_ASSERT_H

/**
 * @brief Declares a compile-time assertion.
 *
 * This macro can be used to check a condition at compile-time. If the condition
 * is not true, the compilation will fail and a diagnostic message is generated.
 *
 * ```c
 * SCU_STATIC_ASSERT(sizeof(int) * CHAR_WIDTH == 32);
 * ```
 *
 * Optionally, a message may be passed as the second argument to provide
 * additional information, such as a helpful explanation.
 *
 * ```c
 * SCU_STATIC_ASSERT(
 *     sizeof(int) * CHAR_WIDTH == 32,
 *     "Expected int to be 32 bits wide."
 * );
 * ```
 *
 * @param[in] expr The expression to evaluate at compile-time.
 * @param[in] ...  An optional message to provide additional information if the
 *                 assertion fails.
 *
 * @note As of C23, `static_assert` became a keyword. This macro is provided for
 * reasons of completeness and consistency with the `SCU_ASSERT()` macro.
 */
#define SCU_STATIC_ASSERT(expr, ...)              \
    static_assert(expr __VA_OPT__(,) __VA_ARGS__)

/**
 * @brief Handles a runtime assertion failure.
 *
 * @param[in] expr The expression which caused the assertion failure.
 * @param[in] file The name of the file in which the assertion failure occurred.
 * @param[in] line The line in which the assertion failure occurred.
 * @param[in] func The name of the function in which the assertion failure
 *                 occurred.
 * @param[in] fmt  An optional `printf()`-style format string to provide
 *                 additional information. If equal to `nullptr`, no additional
 *                 message is displayed and the variable arguments are ignored.
 * @param[in] ...  Variable arguments to be formatted according to the `fmt`
 *                 string.
 *
 * @warning This function terminates the program abnormally and does not return.
 *
 * @note This function is an implementation detail and not meant to be called
 * directly. Use the `SCU_ASSERT()` macro instead.
 */
[[noreturn]]
void scu_assert_fail(
    const char* restrict expr,
    const char* restrict file,
    int line,
    const char* restrict func,
    const char* restrict fmt,
    ...
);

#ifdef NDEBUG
    /**
     * @brief Does nothing, as `NDEBUG` is defined.
     *
     * @param[in] expr Unused.
     */
    #define SCU_ASSERT(expr, ...) ((void) 0)
#else
    /**
     * @brief Declares a runtime assertion.
     *
     * This macro can be used to check a condition at runtime. If the condition
     * is not true, a diagnostic message is printed to the standard error stream
     * and the program is terminated abnormally.
     *
     * A `printf()`-style format string and variable arguments may optionally be
     * passed to provide additional information.
     *
     * ```c
     * void example(int* values, int length) {
     *     SCU_ASSERT(values);
     *     SCU_ASSERT(length >= 0, "Length must be positive (was %d).", length);
     *     ...
     * }
     * ```
     *
     * @param[in] expr The expression to test.
     * @param[in] ...  Variable arguments to provide additional information. If
     *                 present, the first argument must be a null-terminated
     *                 `printf()`-style format string, followed by any number of
     *                 additional arguments to be formatted.
     *
     * @warning Runtime assertions are disabled when the `NDEBUG` macro is
     * defined before this header is directly or transitively included.
     */
    #define SCU_ASSERT(expr, ...)                                              \
        do {                                                                   \
            if (!(expr)) {                                                     \
                __VA_OPT__(                                                    \
                    scu_assert_fail(                                           \
                        #expr,                                                 \
                        __FILE__,                                              \
                        __LINE__,                                              \
                        __func__,                                              \
                        __VA_ARGS__                                            \
                    );                                                         \
                )                                                              \
                scu_assert_fail(#expr, __FILE__, __LINE__, __func__, nullptr); \
            }                                                                  \
        }                                                                      \
        while (false)
#endif

#endif
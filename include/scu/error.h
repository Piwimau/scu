#ifndef SCU_ERROR_H
#define SCU_ERROR_H

#include "scu/api.h"

/** @brief An error code used to signal success or failure. */
typedef enum ScuError {

    /** @brief No error occurred. */
    SCU_ERROR_NONE,

    /** @brief An out-of-memory condition occurred. */
    SCU_ERROR_OUT_OF_MEMORY,

    /** @brief A file could not be (re-)opened. */
    SCU_ERROR_OPENING_FILE,

    /**
     * @brief The end-of-file condition occurred before any data could be read
     * from a file.
     */
    SCU_ERROR_END_OF_FILE,

    /** @brief An error occurred while reading from a file. */
    SCU_ERROR_READING_FILE,

    /** @brief An error occurred while writing to a file. */
    SCU_ERROR_WRITING_FILE,

    /** @brief An error occurred while writing to a buffer. */
    SCU_ERROR_WRITING_BUFFER,

    /** @brief An error occurred while flushing a file. */
    SCU_ERROR_FLUSHING_FILE,

    /**
     * @brief An error occurred while setting the file position indicator of a
     * file.
     */
    SCU_ERROR_SEEKING_FILE,

    /** @brief An error occurred while closing a file. */
    SCU_ERROR_CLOSING_FILE,

    /** @brief An error occurred while removing a file. */
    SCU_ERROR_REMOVING_FILE,

    /** @brief An error occurred while renaming a file. */
    SCU_ERROR_RENAMING_FILE,

    /** @brief Something is already present. */
    SCU_ERROR_ALREADY_PRESENT,

    /** @brief Something did not have an expected format. */
    SCU_ERROR_INVALID_FORMAT,

    /** @brief An error occurred while measuring some time. */
    SCU_ERROR_TIMING_FAILED

} ScuError;

/**
 * @brief Prints a diagnostic message to the standard error stream and
 * terminates the program abnormally.
 *
 * @param[in] file The name of the file in which the error occurred.
 * @param[in] line The line in which the error occurred.
 * @param[in] func The name of the function in which the error occurred.
 * @param[in] fmt  A `printf()`-style format string containing the error
 *                 message.
 * @param[in] ...  Variable arguments to be formatted according to the `fmt`
 *                 string.
 *
 * @warning The behavior is undefined if `file`, `func`, or `fmt` are `nullptr`,
 * or if `line` is negative.
 *
 * This function terminates the program abnormally and does not return.
 *
 * @note This function is an implementation detail and not meant to be called
 * directly. Use the `SCU_FATAL()` macro instead.
 */
[[noreturn]]
SCU_API void scu_fatal(
    const char* restrict file,
    int line,
    const char* restrict func,
    const char* restrict fmt,
    ...
);

/**
 * @brief Prints a diagnostic message to the standard error stream and
 * terminates the program abnormally.
 *
 * This macro accepts a `printf()`-style format string and variable arguments to
 * provide additional information. It wraps `scu_fatal()`, automatically
 * including the file, line and function in which the error occurred.
 *
 * ```c
 * size_t length = 1024;
 * char* buffer = malloc(length);
 * if (!buffer) {
 *     SCU_FATAL("Failed to allocate %zu bytes of memory.\n", length);
 * }
 * ```
 *
 * @param[in] fmt A `printf()`-style format string containing the error message.
 * @param[in] ... Variable arguments to be formatted according to the `fmt`
 *                string.
 *
 * @warning This macro terminates the program abnormally.
 */
#define SCU_FATAL(fmt, ...)                                                \
    scu_fatal(__FILE__, __LINE__, __func__, fmt __VA_OPT__(,) __VA_ARGS__)

/**
 * @brief Marks a code path as unreachable.
 *
 * @warning If executed, this macro invokes `SCU_FATAL()`, which prints a
 * diagnostic message to the standard error stream and terminates the program
 * abnormally.
 */
#define SCU_UNREACHABLE()                                     \
    SCU_FATAL("Reached a code path marked as unreachable.\n")

#endif
#ifndef SCU_IO_H
#define SCU_IO_H

#include <stdarg.h>
#include "scu/api.h"
#include "scu/error.h"
#include "scu/types.h"

/** @brief A file stream used to input and output data. */
typedef struct ScuFile ScuFile;

/** @brief An origin for seeking within a file stream. */
typedef enum ScuSeekOrigin {

    /** @brief Seek from the beginning of the file stream. */
    SCU_SEEK_ORIGIN_SET,

    /** @brief Seek from the current position of the file stream. */
    SCU_SEEK_ORIGIN_CUR,

    /** @brief Seek from the end of the file stream. */
    SCU_SEEK_ORIGIN_END

} ScuSeekOrigin;

/**
 * @brief Returns a pointer to a file stream associated with the standard input
 * stream.
 *
 * @return A pointer to a file stream associated with the standard input stream.
 *
 * @warning The returned file stream must not be modified or passed to
 * `scu_fclose()`.
 *
 * @note This function is an implementation detail and is not meant to be called
 * directly. Use the `SCU_STDIN` macro instead.
 */
SCU_API ScuFile* scu_stdin();

/**
 * @brief Expands to an expression of type `ScuFile*` that is associated with
 * the standard input stream.
 *
 * @warning The file stream must not be modified or passed to `scu_fclose()`.
 */
#define SCU_STDIN scu_stdin()

/**
 * @brief Returns a pointer to a file stream associated with the standard output
 * stream.
 *
 * @return A pointer to a file stream associated with the standard output
 * stream.
 *
 * @warning The returned file stream must not be modified or passed to
 * `scu_fclose()`.
 *
 * @note This function is an implementation detail and is not meant to be called
 * directly. Use the `SCU_STDOUT` macro instead.
 */
SCU_API ScuFile* scu_stdout();

/**
 * @brief Expands to an expression of type `ScuFile*` that is associated with
 * the standard output stream.
 *
 * @warning The file stream must not be modified or passed to `scu_fclose()`.
 */
#define SCU_STDOUT scu_stdout()

/**
 * @brief Returns a pointer to a file stream associated with the standard error
 * stream.
 *
 * @return A pointer to a file stream associated with the standard error stream.
 *
 * @warning The returned file stream must not be modified or passed to
 * `scu_fclose()`.
 *
 * @note This function is an implementation detail and is not meant to be called
 * directly. Use the `SCU_STDERR` macro instead.
 */
SCU_API ScuFile* scu_stderr();

/**
 * @brief Expands to an expression of type `ScuFile*` that is associated with
 * the standard error stream.
 *
 * @warning The file stream must not be modified or passed to `scu_fclose()`.
 */
#define SCU_STDERR scu_stderr()

/**
 * @brief Opens a file with a specified name and mode.
 *
 * The `mode` string must be a valid `fopen()`-style mode string. The following
 * modes are supported:
 *
 * - `"r"`: Opens a file for reading. If the file does not exist, a failure
 *   occurs.
 *
 * - `"r+"`: Opens a file for reading and writing. If the file does not exist, a
 *   failure occurs.
 *
 * - `"w"`: Opens a file for writing. If the file already exists, its contents
 *   are overwritten. Otherwise, a new file is created.
 *
 * - `"w+"`: Opens a file for reading and writing. If the file already exists,
 *   its contents are overwritten. Otherwise, a new file is created.
 *
 * - `"a"`: Opens a file for appending. If the file does not exist, a new file
 *   is created.
 *
 * - `"a+"`: Opens a file for reading and appending. If the file does not exist,
 *   a new file is created.
 *
 * The mode flag `"b"` may optionally be specified to open the file in binary
 * mode, which disables special handling of certain bytes (e.g., newline
 * translation on Windows). Additionally, the mode flag `"x"` may optionally be
 * appended to `"w"` or `"w+"` specifiers, which causes this function to fail if
 * the file already exists (instead of simply overwriting its contents).
 *
 * @param[out] file A pointer to a file stream associated with the opened file,
 *                  or `nullptr` on failure.
 * @param[in]  name The name of the file to open.
 * @param[in]  mode An `fopen()`-style mode string that specifies the mode in
 *                  which to open the file.
 * @return `SCU_ERROR_OPENING_FILE` if the file could not be opened,
 * `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred, or
 * `SCU_ERROR_NONE` on success.
 *
 * @warning If the operation succeeds, the file stream returned via `*file` must
 * be closed with `scu_fclose()` to properly release any associated resources.
 *
 * The behavior is undefined if `mode` is not one of the supported mode strings
 * listed above.
 *
 * @note The exact format of `name` is implementation-defined, and it does not
 * necessarily have to correspond to an actual file in the filesystem.
 */
SCU_API ScuError scu_fopen(
    ScuFile* restrict* restrict file,
    const char* restrict name,
    const char* restrict mode
);

/**
 * @brief Creates and opens a temporary file.
 *
 * This function creates and opens a temporary file in binary update mode (as if
 * by calling `scu_fopen()` with the mode `"wb+"`). The name of the file is
 * guaranteed to be unique within the filesystem.
 *
 * @param[out] file A pointer to a file stream associated with the temporary
 *                  file, or `nullptr` on failure.
 * @return `SCU_ERROR_OPENING_FILE` if an error occurred while creating or
 * opening the temporary file, `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory
 * condition occurred, or `SCU_ERROR_NONE` on success.
 *
 * @warning If the operation succeeds, the file stream returned via `*file` must
 * be closed with `scu_fclose()` to properly release any associated resources.
 */
SCU_API ScuError scu_fopentmp(ScuFile** file);

/**
 * @brief Reopens a file stream with a specified name and mode.
 *
 * This function first attempts to close the file stream (ignoring any errors).
 * Then, if `name` is not `nullptr`, it attempts to open the file with the
 * specified `name` and `mode` (as if by calling `scu_fopen()`). Otherwise, it
 * attempts to reopen the file associated with the file stream.
 *
 * The `mode` string must be a valid `fopen()`-style mode string. The following
 * modes are supported:
 *
 * - `"r"`: Opens a file for reading. If the file does not exist, a failure
 *   occurs.
 *
 * - `"r+"`: Opens a file for reading and writing. If the file does not exist, a
 *   failure occurs.
 *
 * - `"w"`: Opens a file for writing. If the file already exists, its contents
 *   are overwritten. Otherwise, a new file is created.
 *
 * - `"w+"`: Opens a file for reading and writing. If the file already exists,
 *   its contents are overwritten. Otherwise, a new file is created.
 *
 * - `"a"`: Opens a file for appending. If the file does not exist, a new file
 *   is created.
 *
 * - `"a+"`: Opens a file for reading and appending. If the file does not exist,
 *   a new file is created.
 *
 * The mode flag `"b"` may optionally be specified to open the file in binary
 * mode, which disables special handling of certain bytes (e.g., newline
 * translation on Windows). Additionally, the mode flag `"x"` may optionally be
 * appended to `"w"` or `"w+"` specifiers, which causes this function to fail if
 * the file already exists (instead of simply overwriting its contents).
 *
 * @param[in, out] file The file stream to reopen.
 * @param[in]      name The name of the file to open. If equal to `nullptr`,
 *                      the function will attempt to reopen the file associated
 *                      with the file stream.
 * @param[in]      mode An `fopen()`-style mode string that specifies the mode
 *                      in which to open the file.
 * @return `SCU_ERROR_OPENING_FILE` if the file stream could not be reopened, or
 * `SCU_ERROR_NONE` on success.
 *
 * @warning If the operation succeeds, and `file` is not one of the standard
 * streams `SCU_STDIN`, `SCU_STDOUT` or `SCU_STDERR`, the file stream must be
 * closed with `scu_fclose()` to properly release any associated resources.
 *
 * If the operation fails, `file` is invalidated and must not be used anymore
 * (and especially not closed with `scu_fclose()`).
 *
 * The behavior is undefined if `mode` is not one of the supported mode strings
 * listed above.
 *
 * @note The exact format of `name` is implementation-defined, and it does not
 * necessarily have to correspond to an actual file in the filesystem.
 * Additionally, it is implementation-defined which mode changes are allowed
 * when `name` is `nullptr`.
 *
 * This function does not allocate a new `ScuFile` object, but modifies the
 * file stream in-place.
 */
SCU_API ScuError scu_freopen(
    ScuFile* restrict file,
    const char* restrict name,
    const char* restrict mode
);

/**
 * @brief Closes a file stream and releases any associated resources.
 *
 * @param[in, out] file The file stream to close.
 * @return `SCU_ERROR_CLOSING_FILE` if the file stream could not be closed, or
 * `SCU_ERROR_NONE` on success.
 *
 * @warning The behavior is undefined if `file` is used after calling this
 * function.
 */
SCU_API ScuError scu_fclose(ScuFile* file);

/**
 * @brief Writes any buffered data to the underlying file associated with a
 * file stream.
 *
 * @param[in, out] file The file stream to flush. If equal to `nullptr`, all
 *                      open output streams are flushed.
 * @return `SCU_ERROR_FLUSHING_FILE` if the file stream could not be flushed, or
 * `SCU_ERROR_NONE` on success.
 *
 * @warning The behavior is undefined if `file` is an input stream or an update
 * stream on which the last operation was input.
 */
SCU_API ScuError scu_fflush(ScuFile* file);

/**
 * @brief Removes a file with a specified name.
 *
 * @param[in] name The name of the file to remove.
 * @return `SCU_ERROR_REMOVING_FILE` if an error occurred while removing the
 * file, or `SCU_ERROR_NONE` on success.
 *
 * @note If the file is currently opened by any process, the behavior is
 * implementation-defined.
 */
SCU_API ScuError scu_fremove(const char* name);

/**
 * @brief Renames a file.
 *
 * @param[in] oldName The current name of the file.
 * @param[in] newName The new name of the file.
 * @return `SCU_ERROR_RENAMING_FILE` if an error occurred while renaming the
 * file, or `SCU_ERROR_NONE` on success.
 *
 * @note If a file with the new name already exists, the behavior is
 * implementation-defined.
 */
SCU_API ScuError scu_frename(const char* oldName, const char* newName);

/**
 * @brief Reads `count` objects of size `size` from a file stream into a buffer.
 *
 * @param[in, out] file   The file stream to read from.
 * @param[out]     buffer The buffer to write the data to.
 * @param[in]      count  The number of objects to read.
 * @param[in]      size   The size of each object (in bytes).
 * @return The number of objects read, which may be less than `count` if the
 * end-of-file condition is reached or an error occurs while reading from the
 * file stream.
 *
 * @warning If an error occurs while reading from the file stream, the value of
 * the file position indicator is indeterminate. The same holds true for the
 * value of an object that is only partially read.
 *
 * @note The `buffer` is reinterpreted as a contiguous array of `count * size`
 * bytes.
 *
 * If `count` or `size` is zero, `buffer` is ignored (it may even be `nullptr`),
 * and the function returns zero.
 */
SCU_API Scuisize scu_fread(
    ScuFile* restrict file,
    void* restrict buffer,
    Scuisize count,
    Scuisize size
);

/**
 * @brief Reads `count` objects of size `size` from the standard input stream
 * into a buffer.
 *
 * @param[out] buffer The buffer to write the data to.
 * @param[in]  count  The number of objects to read.
 * @param[in]  size   The size of each object (in bytes).
 * @return The number of objects read, which may be less than `count` if the
 * end-of-file condition is reached or an error occurs while reading from the
 * standard input stream.
 *
 * @warning If an error occurs while reading from the standard input stream, the
 * value of the file position indicator is indeterminate. The same holds true
 * for the value of an object that is only partially read.
 *
 * @note The `buffer` is reinterpreted as a contiguous array of `count * size`
 * bytes.
 *
 * If `count` or `size` is zero, `buffer` is ignored (it may even be `nullptr`),
 * and the function returns zero.
 *
 * Calling this function is equivalent to calling `scu_fread(SCU_STDIN, buffer,
 * count, size)`.
 */
SCU_API Scuisize scu_read(void* buffer, Scuisize count, Scuisize size);

/**
 * @brief Writes `count` objects of size `size` from a buffer to a file stream.
 *
 * @param[in, out] file   The file stream to write to.
 * @param[in]      buffer The buffer to read the data from.
 * @param[in]      count  The number of objects to write.
 * @param[in]      size   The size of each object (in bytes).
 * @return The number of objects written, which may be less than `count` if an
 * error occurs while writing to the file stream.
 *
 * @warning If an error occurs, the value of the file position indicator of the
 * file stream is indeterminate.
 *
 * @note The `buffer` is reinterpreted as a contiguous array of `count * size`
 * bytes.
 *
 * If `count` or `size` is zero, `buffer` is ignored (it may even be `nullptr`),
 * and the function returns zero.
 */
SCU_API Scuisize scu_fwrite(
    ScuFile* restrict file,
    const void* restrict buffer,
    Scuisize count,
    Scuisize size
);

/**
 * @brief Writes `count` objects of size `size` from a buffer to the standard
 * output stream.
 *
 * @param[in] buffer The buffer to read the data from.
 * @param[in] count  The number of objects to write.
 * @param[in] size   The size of each object (in bytes).
 * @return The number of objects written, which may be less than `count` if an
 * error occurs while writing to the standard output stream.
 *
 * @warning If an error occurs, the value of the file position indicator of the
 * standard output stream is indeterminate.
 *
 * @note The `buffer` is reinterpreted as a contiguous array of `count * size`
 * bytes.
 *
 * If `count` or `size` is zero, `buffer` is ignored (it may even be `nullptr`),
 * and the function returns zero.
 *
 * Calling this function is equivalent to calling `scu_fwrite(SCU_STDOUT,
 * buffer, count, size)`.
 */
SCU_API Scuisize scu_write(const void* buffer, Scuisize count, Scuisize size);

/**
 * @brief Reads a single byte from a file stream.
 *
 * @param[in, out] file The file stream to read from.
 * @param[out]     c    The byte read from the file stream, or a null byte
 *                      ('\0') on failure.
 * @return `SCU_ERROR_END_OF_FILE` if the end-of-file condition is reached
 * before a byte is read, `SCU_ERROR_READING_FILE` if an error occurred while
 * reading from the file stream, or `SCU_ERROR_NONE` on success.
 */
SCU_API ScuError scu_freadc(ScuFile* restrict file, char* restrict c);

/**
 * @brief Reads a single byte from the standard input stream.
 *
 * Calling this function is equivalent to calling `scu_freadc(SCU_STDIN, c)`.
 *
 * @param[out] c The byte read from the standard input stream, or a null byte
 *               ('\0') on failure.
 * @return `SCU_ERROR_END_OF_FILE` if the end-of-file condition is reached
 * before a byte is read, `SCU_ERROR_READING_FILE` if an error occurred while
 * reading from the standard input stream, or `SCU_ERROR_NONE` on success.
 */
SCU_API ScuError scu_readc(char* c);

/**
 * @brief Writes a single byte to a file stream.
 *
 * @param[in, out] file The file stream to write to.
 * @param[in]      c    The byte to write.
 * @return `SCU_ERROR_WRITING_FILE` if an error occurred while writing to the
 * file stream, or `SCU_ERROR_NONE` on success.
 */
SCU_API ScuError scu_fwritec(ScuFile* file, char c);

/**
 * @brief Writes a single byte to the standard output stream.
 *
 * @param[in] c The byte to write.
 * @return `SCU_ERROR_WRITING_FILE` if an error occurred while writing to the
 * standard output stream, or `SCU_ERROR_NONE` on success.
 *
 * @note Calling this function is equivalent to calling `scu_fwritec(SCU_STDOUT,
 * c)`.
 */
SCU_API ScuError scu_writec(char c);

/**
 * @brief Pushes a single byte back into a file stream, such that
 * a subsequent read operation will return that byte.
 *
 * @param[in, out] file The file stream to push the byte back into.
 * @param[in]      c    The byte to push back.
 * @return `SCU_ERROR_WRITING_FILE` if the byte could not be pushed back, or
 * `SCU_ERROR_NONE` on success.
 *
 * @warning The behavior is undefined if the file position indicator is zero
 * (i.e., no bytes have been read from the file stream yet).
 *
 * @note Calls to repositioning functions such as `scu_fseek()` and
 * `scu_frewind()` discard the effects of `scu_funreadc()`.
 *
 * Multiple calls to `scu_funreadc()` without an intervening read operation or
 * repositioning of the file stream may fail, as the size of the pushback buffer
 * is implementation-defined and only guaranteed to be at least one. If multiple
 * calls are successful, subsequent reads will return the pushed-back bytes in
 * reverse order.
 *
 * A successful call to `scu_funreadc()` clears the end-of-file indicator for
 * the file stream. Additionally, for binary streams, the file position
 * indicator is decremented by one. For text streams, the file position
 * indicator is decremented in an unspecified manner. However, it is guaranteed
 * that after all pushed-back bytes have been read, the file position indicator
 * will be equal to the original position before the call to `scu_funreadc()`.
 */
SCU_API ScuError scu_funreadc(ScuFile* file, char c);

/**
 * @brief Reads a byte string from a file stream.
 *
 * This function reads up to `size - 1` bytes from the file stream into `buffer`
 * and terminates it with a null byte.
 *
 * @param[in, out] file   The file stream to read from.
 * @param[out]     buffer The buffer to write the data to.
 * @param[in]      size   The size of the buffer (in bytes, including the
 *                        terminating null byte).
 * @return `SCU_ERROR_END_OF_FILE` if the end-of-file condition is reached
 * before any bytes are read, `SCU_ERROR_READING_FILE` if an error occurred
 * while reading from the file stream, or `SCU_ERROR_NONE` on success.
 *
 * @note If `size` is zero, `buffer` is ignored (it may even be `nullptr`), and
 * the function returns `SCU_ERROR_NONE`.
 *
 * If `size` is greater than or equal to one, `buffer` is guaranteed to be
 * null-terminated, even if the end-of-file condition is reached before any
 * bytes are read or if an error occurs while reading from the file stream.
 */
SCU_API ScuError scu_freads(
    ScuFile* restrict file,
    char* restrict buffer,
    Scuisize size
);

/**
 * @brief Reads a byte string from the standard input stream.
 *
 * This function reads up to `size - 1` bytes from the standard input stream
 * into `buffer` and terminates it with a null byte.
 *
 * @param[out] buffer The buffer to write the data to.
 * @param[in]  size   The size of the buffer (in bytes, including the
 *                    terminating null byte).
 * @return `SCU_ERROR_END_OF_FILE` if the end-of-file condition is reached
 * before any bytes are read, `SCU_ERROR_READING_FILE` if an error occurred
 * while reading from the standard input stream, or `SCU_ERROR_NONE` on success.
 *
 * @note If `size` is zero, `buffer` is ignored (it may even be `nullptr`), and
 * the function returns `SCU_ERROR_NONE`.
 *
 * If `size` is greater than or equal to one, `buffer` is guaranteed to be
 * null-terminated, even if the end-of-file condition is reached before any
 * bytes are read or if an error occurs while reading from the file stream.
 *
 * Calling this function is equivalent to calling `scu_freads(SCU_STDIN, buffer,
 * size)`.
 */
SCU_API ScuError scu_reads(char* buffer, Scuisize size);

/**
 * @brief Writes a byte string to a file stream.
 *
 * This function writes the null-terminated byte string pointed to by `buffer`
 * to the file stream, excluding the terminating null byte.
 *
 * @param[in, out] file   The file stream to write to.
 * @param[in]      buffer The null-terminated byte string to write.
 * @return `SCU_ERROR_WRITING_FILE` if an error occurred while writing to the
 * file stream, or `SCU_ERROR_NONE` on success.
 */
SCU_API ScuError scu_fwrites(
    ScuFile* restrict file,
    const char* restrict buffer
);

/**
 * @brief Writes a byte string to the standard output stream.
 *
 * This function writes the null-terminated byte string pointed to by `buffer`
 * to the standard output stream, excluding the terminating null byte.
 *
 * @param[in] buffer The null-terminated byte string to write.
 * @return `SCU_ERROR_WRITING_FILE` if an error occurred while writing to the
 * standard output stream, or `SCU_ERROR_NONE` on success.
 *
 * @note Calling this function is equivalent to calling `scu_fwrites(SCU_STDOUT,
 * buffer)`.
 */
SCU_API ScuError scu_writes(const char* buffer);

/**
 * @brief Reads a single line from a file stream.
 *
 * This function reads a single line from the file stream into `*buffer` and
 * terminates it with a null byte. The buffer is dynamically reallocated to fit
 * the entire line (including the newline (if present) and the terminating null
 * byte). The new size is reflected in `*size`.
 *
 * @param[in, out] file   The file stream to read from.
 * @param[in, out] buffer A pointer to a buffer to write the data to.
 * @param[in, out] size   A pointer to the size of the buffer (in bytes,
 *                        including the terminating null byte).
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred,
 * `SCU_ERROR_END_OF_FILE` if the end-of-file condition is reached before any
 * bytes are read, `SCU_ERROR_READING_FILE` if an error occurred while reading
 * from the file stream, or `SCU_ERROR_NONE` on success.
 *
 * @warning If `*buffer` is not `nullptr`, it must have been allocated using
 * the default allocator. The caller is responsible for deallocating `*buffer`
 * when it is no longer needed.
 *
 * @note If `*size` is zero, `*buffer` must be `nullptr` (and vice versa). In
 * this case, the function allocates a buffer with an unspecified initial size.
 *
 * If `*size` is greater than or equal to one and no out-of-memory condition
 * occurs, `*buffer` is guaranteed to be null-terminated, even if the
 * end-of-file condition is reached before any bytes are read or if an error
 * occurs while reading from the file stream.
 */
SCU_API ScuError scu_freadln(
    ScuFile* restrict file,
    char* restrict* restrict buffer,
    Scuisize* restrict size
);

/**
 * @brief Reads a single line from the standard input stream.
 *
 * This function reads a single line from the standard input stream into
 * `*buffer` and terminates it with a null byte. The buffer is dynamically
 * reallocated to fit the entire line (including the newline (if present) and
 * the terminating null byte). The new size is reflected in `*size`.
 *
 * @param[in, out] buffer A pointer to a buffer to write the data to.
 * @param[in, out] size   A pointer to the size of the buffer (in bytes,
 *                        including the terminating null byte).
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred,
 * `SCU_ERROR_END_OF_FILE` if the end-of-file condition is reached before any
 * bytes are read, `SCU_ERROR_READING_FILE` if an error occurred while reading
 * from the standard input stream, or `SCU_ERROR_NONE` on success.
 *
 * @warning If `*buffer` is not `nullptr`, it must have been allocated using
 * the default allocator. The caller is responsible for deallocating `*buffer`
 * when it is no longer needed.
 *
 * @note If `*size` is zero, `*buffer` must be `nullptr` (and vice versa). In
 * this case, the function allocates a buffer with an unspecified initial size.
 *
 * If `*size` is greater than or equal to one and no out-of-memory condition
 * occurs, `*buffer` is guaranteed to be null-terminated, even if the
 * end-of-file condition is reached before any bytes are read or if an error
 * occurs while reading from the file stream.
 *
 * Calling this function is equivalent to calling `scu_freadln(SCU_STDIN,
 * buffer, size)`.
 */
SCU_API ScuError scu_readln(
    char* restrict* restrict buffer,
    Scuisize* restrict size
);

/**
 * @brief Writes a byte string followed by a newline to a file stream.
 *
 * This function writes the null-terminated byte string pointed to by `buffer`
 * to the file stream, excluding the terminating null byte. Additionally, a
 * newline byte is written after the string.
 *
 * @param[in, out] file   The file stream to write to.
 * @param[in]      buffer The null-terminated byte string to write.
 * @return `SCU_ERROR_WRITING_FILE` if an error occurred while writing to the
 * file stream, or `SCU_ERROR_NONE` on success.
 */
SCU_API ScuError scu_fwriteln(
    ScuFile* restrict file,
    const char* restrict buffer
);

/**
 * @brief Writes a byte string followed by a newline to the standard output
 * stream.
 *
 * This function writes the null-terminated byte string pointed to by `buffer`
 * to the standard output stream, excluding the terminating null byte.
 * Additionally, a newline byte is written after the string.
 *
 * @param[in] buffer The null-terminated byte string to write.
 * @return `SCU_ERROR_WRITING_FILE` if an error occurred while writing to the
 * standard output stream, or `SCU_ERROR_NONE` on success.
 *
 * @note Calling this function is equivalent to calling
 * `scu_fwriteln(SCU_STDOUT, buffer)`.
 */
SCU_API ScuError scu_writeln(const char* buffer);

/**
 * @brief Reads all remaining bytes from a file stream into a dynamically
 * allocated buffer.
 *
 * This function reads all remaining bytes from the file stream into `*buffer`
 * and terminates it with a null byte. The buffer is dynamically reallocated to
 * fit the entire contents (including the terminating null byte). The new size
 * is reflected in `*size`.
 *
 * @param[in, out] file   The file stream to read from.
 * @param[in, out] buffer A pointer to a buffer to write the data to.
 * @param[in, out] size   A pointer to the size of the buffer (in bytes,
 *                        including the terminating null byte).
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred,
 * `SCU_ERROR_END_OF_FILE` if the end-of-file condition is reached before any
 * bytes are read, `SCU_ERROR_READING_FILE` if an error occurred while reading
 * from the file stream, or `SCU_ERROR_NONE` on success.
 *
 * @warning If `*buffer` is not `nullptr`, it must have been allocated using
 * the default allocator. The caller is responsible for deallocating `*buffer`
 * when it is no longer needed.
 *
 * @note If `*size` is zero, `*buffer` must be `nullptr` (and vice versa). In
 * this case, the function allocates a buffer with an unspecified initial size.
 *
 * If `*size` is greater than or equal to one and no out-of-memory condition
 * occurs, `*buffer` is guaranteed to be null-terminated, even if the
 * end-of-file condition is reached before any bytes are read or if an error
 * occurs while reading from the file stream.
 */
SCU_API ScuError scu_freadall(
    ScuFile* restrict file,
    char* restrict* restrict buffer,
    Scuisize* restrict size
);

/**
 * @brief Reads all remaining bytes from the standard input stream into a
 * dynamically allocated buffer.
 *
 * This function reads all remaining bytes from the standard input stream into
 * `*buffer` and terminates it with a null byte. The buffer is dynamically
 * reallocated to fit the entire contents (including the terminating null byte).
 * The new size is reflected in `*size`.
 *
 * @param[in, out] buffer A pointer to a buffer to write the data to.
 * @param[in, out] size   A pointer to the size of the buffer (in bytes,
 *                        including the terminating null byte).
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred,
 * `SCU_ERROR_END_OF_FILE` if the end-of-file condition is reached before any
 * bytes are read, `SCU_ERROR_READING_FILE` if an error occurred while reading
 * from the standard input stream, or `SCU_ERROR_NONE` on success.
 *
 * @warning If `*buffer` is not `nullptr`, it must have been allocated using
 * the default allocator. The caller is responsible for deallocating `*buffer`
 * when it is no longer needed.
 *
 * @note If `*size` is zero, `*buffer` must be `nullptr` (and vice versa). In
 * this case, the function allocates a buffer with an unspecified initial size.
 *
 * If `*size` is greater than or equal to one and no out-of-memory condition
 * occurs, `*buffer` is guaranteed to be null-terminated, even if the
 * end-of-file condition is reached before any bytes are read or if an error
 * occurs while reading from the file stream.
 *
 * Calling this function is equivalent to calling `scu_freadall(SCU_STDIN,
 * buffer, size)`.
 */
SCU_API ScuError scu_readall(
    char* restrict* restrict buffer,
    Scuisize* restrict size
);

/**
 * @brief Reads formatted input from a file stream.
 *
 * This function reads data from the file stream and interprets it according to
 * a `vfscanf()`-style format string. The read values are stored in the
 * locations pointed to by the variable argument list.
 *
 * @param[in, out] file   The file stream to read from.
 * @param[in]      format A `vfscanf()`-style format string that specifies how
 *                        to interpret the input.
 * @param[out]     args   A variable argument list containing pointers to
 *                        locations where the read values should be stored.
 * @return The number of arguments read, which may be less than the number of
 * expected arguments if the end-of-file condition is reached or an error occurs
 * while reading from the file stream. If an error occurs before any argument is
 * read, `-1` is returned.
 *
 * @warning The caller is expected to call `va_end()` on `args` after this
 * function returns.
 */
SCU_API Scuisize scu_vfscanf(
    ScuFile* restrict file,
    const char* restrict format,
    va_list args
);

/**
 * @brief Reads formatted input from a file stream.
 *
 * This function reads data from the file stream and interprets it according to
 * an `fscanf()`-style format string. The read values are stored in the
 * locations pointed to by the variable argument list.
 *
 * @param[in, out] file   The file stream to read from.
 * @param[in]      format An `fscanf()`-style format string that specifies how
 *                        to interpret the input.
 * @param[out]     ...    A variable argument list containing pointers to
 *                        locations where the read values should be stored.
 * @return The number of arguments read, which may be less than the number of
 * expected arguments if the end-of-file condition is reached or an error occurs
 * while reading from the file stream. If an error occurs before any argument is
 * read, `-1` is returned.
 */
SCU_API Scuisize scu_fscanf(
    ScuFile* restrict file,
    const char* restrict format,
    ...
);

/**
 * @brief Reads formatted input from the standard input stream.
 *
 * This function reads data from the standard input stream and interprets it
 * according to a `vscanf()`-style format string. The read values are stored in
 * the locations pointed to by the variable argument list.
 *
 * @param[in]  format A `vscanf()`-style format string that specifies how to
 *                    interpret the input.
 * @param[out] args   A variable argument list containing pointers to locations
 *                    where the read values should be stored.
 * @return The number of arguments read, which may be less than the number of
 * expected arguments if the end-of-file condition is reached or an error occurs
 * while reading from the standard input stream. If an error occurs before any
 * argument is read, `-1` is returned.
 *
 * @warning The caller is expected to call `va_end()` on `args` after this
 * function returns.
 *
 * @note Calling this function is equivalent to calling `scu_vfscanf(SCU_STDIN,
 * format, args)`.
 */
SCU_API Scuisize scu_vscanf(const char* restrict format, va_list args);

/**
 * @brief Reads formatted input from the standard input stream.
 *
 * This function reads data from the standard input stream and interprets it
 * according to a `scanf()`-style format string. The read values are stored in
 * the locations pointed to by the variable argument list.
 *
 * @param[in]  format A `scanf()`-style format string that specifies how to
 *                    interpret the input.
 * @param[out] ...    A variable argument list containing pointers to locations
 *                    where the read values should be stored.
 * @return The number of arguments read, which may be less than the number of
 * expected arguments if the end-of-file condition is reached or an error occurs
 * while reading from the standard input stream. If an error occurs before any
 * argument is read, `-1` is returned.
 *
 * @note Calling this function is equivalent to calling `scu_fscanf(SCU_STDIN,
 * format, ...)`.
 */
SCU_API Scuisize scu_scanf(const char* restrict format, ...);

/**
 * @brief Reads formatted input from a byte string.
 *
 * This function reads data from the byte string and interprets it according to
 * a `vsscanf()`-style format string. The read values are stored in the
 * locations pointed to by the variable argument list.
 *
 * @param[in]  buffer The null-terminated byte string to read from.
 * @param[in]  format A `vsscanf()`-style format string that specifies how to
 *                    interpret the input.
 * @param[out] args   A variable argument list containing pointers to locations
 *                    where the read values should be stored.
 * @return The number of arguments read, which may be less than the number of
 * expected arguments if the end of the byte string is reached or an error
 * occurs. If an error occurs before any argument is read, `-1` is returned.
 *
 * @warning The caller is expected to call `va_end()` on `args` after this
 * function returns.
 */
SCU_API Scuisize scu_vsscanf(
    const char* restrict buffer,
    const char* restrict format,
    va_list args
);

/**
 * @brief Reads formatted input from a byte string.
 *
 * This function reads data from the byte string and interprets it according to
 * an `sscanf()`-style format string. The read values are stored in the
 * locations pointed to by the variable argument list.
 *
 * @param[in]  buffer The null-terminated byte string to read from.
 * @param[in]  format An `sscanf()`-style format string that specifies how to
 *                    interpret the input.
 * @param[out] ...    A variable argument list containing pointers to locations
 *                    where the read values should be stored.
 * @return The number of arguments read, which may be less than the number of
 * expected arguments if the end of the byte string is reached or an error
 * occurs. If an error occurs before any argument is read, `-1` is returned.
 */
SCU_API Scuisize scu_sscanf(
    const char* restrict buffer,
    const char* restrict format,
    ...
);

/**
 * @brief Writes formatted output to a file stream.
 *
 * This function writes data to the file stream according to a
 * `vfprintf()`-style format string. The values to be written are taken from the
 * variable argument list.
 *
 * @param[in, out] file   The file stream to write to.
 * @param[in]      format A `vfprintf()`-style format string that specifies how
 *                        to format the output.
 * @param[in]      args   A variable argument list containing the values to be
 *                        written.
 * @return The number of bytes written to the file stream, or `-1` on failure.
 *
 * @warning The caller is expected to call `va_end()` on `args` after this
 * function returns.
 */
SCU_API Scuisize scu_vfprintf(
    ScuFile* restrict file,
    const char* restrict format,
    va_list args
);

/**
 * @brief Writes formatted output to a file stream.
 *
 * This function writes data to the file stream according to an
 * `fprintf()`-style format string. The values to be written are taken from the
 * variable argument list.
 *
 * @param[in, out] file   The file stream to write to.
 * @param[in]      format An `fprintf()`-style format string that specifies how
 *                        to format the output.
 * @param[in]      ...    A variable argument list containing the values to be
 *                        written.
 * @return The number of bytes written to the file stream, or `-1` on failure.
 */
SCU_API Scuisize scu_fprintf(
    ScuFile* restrict file,
    const char* restrict format,
    ...
);

/**
 * @brief Writes formatted output to the standard output stream.
 *
 * This function writes data to the standard output stream according to a
 * `vprintf()`-style format string. The values to be written are taken from the
 * variable argument list.
 *
 * @param[in] format A `vprintf()`-style format string that specifies how to
 *                   format the output.
 * @param[in] args   A variable argument list containing the values to be
 *                   written.
 * @return The number of bytes written to the standard output stream, or `-1` on
 * failure.
 *
 * @warning The caller is expected to call `va_end()` on `args` after this
 * function returns.
 *
 * @note Calling this function is equivalent to calling
 * `scu_vfprintf(SCU_STDOUT, format, args)`.
 */
SCU_API Scuisize scu_vprintf(const char* restrict format, va_list args);

/**
 * @brief Writes formatted output to the standard output stream.
 *
 * This function writes data to the standard output stream according to a
 * `printf()`-style format string. The values to be written are taken from the
 * variable argument list.
 *
 * @param[in] format A `printf()`-style format string that specifies how to
 *                   format the output.
 * @param[in] ...    A variable argument list containing the values to be
 *                   written.
 * @return The number of bytes written to the standard output stream, or `-1` on
 * failure.
 *
 * @note Calling this function is equivalent to calling `scu_fprintf(SCU_STDOUT,
 * format, ...)`.
 */
SCU_API Scuisize scu_printf(const char* restrict format, ...);

/**
 * @brief Writes formatted output to a buffer.
 *
 * This function writes up to `size - 1` bytes to the buffer pointed to by
 * `buffer` and terminates it with a null byte. The output is formatted
 * according to a `vsnprintf()`-style format string. The values to be written
 * are taken from the variable argument list.
 *
 * @param[out] buffer The buffer to write the data to.
 * @param[in]  size   The size of the buffer (in bytes, including the
 *                    terminating null byte).
 * @param[in]  format A `vsnprintf()`-style format string that specifies how to
 *                    format the output.
 * @param[in]  args   A variable argument list containing the values to be
 *                    written.
 * @return The number of bytes written (excluding the terminating null byte), or
 * the number of bytes that would have been written if `size` had been
 * sufficiently large. If an error occurs while writing to the buffer, `-1` is
 * returned.
 *
 * @warning The caller is expected to call `va_end()` on `args` after this
 * function returns.
 *
 * @note If `size` is zero, `buffer` is ignored (it may even be `nullptr`). In
 * this case, the function still computes the number of bytes (excluding the
 * terminating null byte) that would have been written if `size` had been
 * sufficiently large.
 *
 * If `size` is greater than or equal to one, `buffer` is guaranteed to be
 * null-terminated, even if an error occurs.
 */
SCU_API Scuisize scu_vsnprintf(
    char* restrict buffer,
    Scuisize size,
    const char* restrict format,
    va_list args
);

/**
 * @brief Writes formatted output to a buffer.
 *
 * This function writes up to `size - 1` bytes to the buffer pointed to by
 * `buffer` and terminates it with a null byte. The output is formatted
 * according to an `snprintf()`-style format string. The values to be written
 * are taken from the variable argument list.
 *
 * @param[out] buffer The buffer to write the data to.
 * @param[in]  size   The size of the buffer (in bytes, including the
 *                    terminating null byte).
 * @param[in]  format An `snprintf()`-style format string that specifies how to
 *                    format the output.
 * @param[in]  ...    A variable argument list containing the values to be
 *                    written.
 * @return The number of bytes written (excluding the terminating null byte), or
 * the number of bytes that would have been written if `size` had been
 * sufficiently large. If an error occurs while writing to the buffer, `-1` is
 * returned.
 *
 * @note If `size` is zero, `buffer` is ignored (it may even be `nullptr`). In
 * this case, the function still computes the number of bytes (excluding
 * the terminating null byte) that would have been written if `size` had
 * been sufficiently large.
 *
 * If `size` is greater than or equal to one, `buffer` is guaranteed to be
 * null-terminated, even if an error occurs.
 */
SCU_API Scuisize scu_snprintf(
    char* restrict buffer,
    Scuisize size,
    const char* restrict format,
    ...
);

/**
 * @brief Writes formatted output to a dynamically allocated buffer.
 *
 * This function writes a formatted string to a dynamically allocated buffer
 * according to a `vsnprintf()`-style format string. The buffer is reallocated
 * to fit the entire output (including the terminating null byte). The pointer
 * to the buffer is stored in `*buffer`, and its size (in bytes, including the
 * terminating null byte) is stored in `*size`.
 *
 * @param[in, out] buffer A pointer to a buffer to write the data to.
 * @param[in, out] size   A pointer to the size of the buffer (in bytes,
 *                        including the terminating null byte).
 * @param[in]      format A `vsnprintf()`-style format string that specifies how
 *                        to format the output.
 * @param[in]      args   A variable argument list containing the values to be
 *                        written.
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred,
 * `SCU_ERROR_WRITING_BUFFER` if an error occurred while writing to the buffer,
 * or `SCU_ERROR_NONE` on success.
 *
 * @warning If `*buffer` is not `nullptr`, it must have been allocated using
 * the default allocator. The caller is responsible for deallocating `*buffer`
 * when it is no longer needed.
 *
 * The caller is expected to call `va_end()` on `args` after this function
 * returns.
 *
 * @note If `*size` is zero, `*buffer` must be `nullptr` (and vice versa). In
 * this case, the function allocates a buffer.
 *
 * If `*size` is greater than or equal to one and no out-of-memory condition
 * occurs, `*buffer` is guaranteed to be null-terminated.
 */
SCU_API ScuError scu_vrsnprintf(
    char* restrict* restrict buffer,
    Scuisize* restrict size,
    const char* restrict format,
    va_list args
);

/**
 * @brief Writes formatted output to a dynamically allocated buffer.
 *
 * This function writes a formatted string to a dynamically allocated buffer
 * according to an `snprintf()`-style format string. The buffer is reallocated
 * to fit the entire output (including the terminating null byte). The pointer
 * to the buffer is stored in `*buffer`, and its size (in bytes, including the
 * terminating null byte) is stored in `*size`.
 *
 * @param[in, out] buffer A pointer to a buffer to write the data to.
 * @param[in, out] size   A pointer to the size of the buffer (in bytes,
 *                        including the terminating null byte).
 * @param[in]      format An `snprintf()`-style format string that specifies how
 *                        to format the output.
 * @param[in]      ...    A variable argument list containing the values to be
 *                        written.
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred,
 * `SCU_ERROR_WRITING_BUFFER` if an error occurred while writing to the buffer,
 * or `SCU_ERROR_NONE` on success.
 *
 * @warning If `*buffer` is not `nullptr`, it must have been allocated using
 * the default allocator. The caller is responsible for deallocating `*buffer`
 * when it is no longer needed.
 *
 * @note If `*size` is zero, `*buffer` must be `nullptr` (and vice versa). In
 * this case, the function allocates a buffer.
 *
 * If `*size` is greater than or equal to one and no out-of-memory condition
 * occurs, `*buffer` is guaranteed to be null-terminated.
 */
SCU_API ScuError scu_rsnprintf(
    char* restrict* restrict buffer,
    Scuisize* restrict size,
    const char* restrict format,
    ...
);

/**
 * @brief Appends formatted output to a dynamically allocated buffer.
 *
 * This function appends a formatted string to a dynamically allocated,
 * null-terminated buffer according to a `vsnprintf()`-style format string. The
 * buffer is reallocated to fit the entire output (including the terminating
 * null byte). The pointer to the buffer is stored in `*buffer`, and its size
 * (in bytes, including the terminating null byte) is stored in `*size`.
 *
 * @param[in, out] buffer A pointer to a null-terminated buffer to append the
 *                        data to.
 * @param[in, out] size   A pointer to the size of the buffer (in bytes,
 *                        including the terminating null byte).
 * @param[in]      format A `vsnprintf()`-style format string that specifies how
 *                        to format the output.
 * @param[in]      args   A variable argument list containing the values to be
 *                        written.
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred,
 * `SCU_ERROR_WRITING_BUFFER` if an error occurred while appending to the
 * buffer, or `SCU_ERROR_NONE` on success.
 *
 * @warning If `*buffer` is not `nullptr`, it must have been allocated using
 * the default allocator and be null-terminated. The caller is responsible for
 * deallocating `*buffer` when it is no longer needed.
 *
 * The caller is expected to call `va_end()` on `args` after this function
 * returns.
 *
 * @note If `*size` is zero, `*buffer` must be `nullptr` (and vice versa). In
 * this case, the function allocates a buffer.
 *
 * If `*size` is greater than or equal to one and no out-of-memory condition
 * occurs, `*buffer` is guaranteed to be null-terminated.
 */
SCU_API ScuError scu_vrasnprintf(
    char* restrict* restrict buffer,
    Scuisize* restrict size,
    const char* restrict format,
    va_list args
);

/**
 * @brief Appends formatted output to a dynamically allocated buffer.
 *
 * This function appends a formatted string to a dynamically allocated,
 * null-terminated buffer according to an `snprintf()`-style format string. The
 * buffer is reallocated to fit the entire output (including the terminating
 * null byte). The pointer to the buffer is stored in `*buffer`, and its size
 * (in bytes, including the terminating null byte) is stored in `*size`.
 *
 * @param[in, out] buffer A pointer to a null-terminated buffer to append the
 *                        data to.
 * @param[in, out] size   A pointer to the size of the buffer (in bytes,
 *                        including the terminating null byte).
 * @param[in]      format An `snprintf()`-style format string that specifies how
 *                        to format the output.
 * @param[in]      ...    A variable argument list containing the values to be
 *                        written.
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred,
 * `SCU_ERROR_WRITING_BUFFER` if an error occurred while appending to the
 * buffer, or `SCU_ERROR_NONE` on success.
 *
 * @warning If `*buffer` is not `nullptr`, it must have been allocated using
 * the default allocator and be null-terminated. The caller is responsible for
 * deallocating `*buffer` when it is no longer needed.
 *
 * @note If `*size` is zero, `*buffer` must be `nullptr` (and vice versa). In
 * this case, the function allocates a buffer.
 *
 * If `*size` is greater than or equal to one and no out-of-memory condition
 * occurs, `*buffer` is guaranteed to be null-terminated.
 */
SCU_API ScuError scu_rasnprintf(
    char* restrict* restrict buffer,
    Scuisize* restrict size,
    const char* restrict format,
    ...
);

/**
 * @brief Returns the file position indicator of a file stream.
 *
 * @param[in] file The file stream to check.
 * @return The file position indicator of the file stream, or `-1` on failure.
 *
 * @note For binary streams, the value returned by this function is the number
 * of bytes from the beginning of the file. For text streams, the value is
 * unspecified and only meaningful as an input to `scu_fseek()`.
 */
SCU_API Scuisize scu_ftell(ScuFile* file);

/**
 * @brief Sets the file position indicator of a file stream.
 *
 * @param[in, out] file       The file stream to modify.
 * @param[in]      seekOrigin The origin from which to set the file position
 *                            indicator.
 * @param[in]      offset     The offset from `seekOrigin` to set the file
 *                            position indicator.
 * @return `SCU_ERROR_SEEKING_FILE` if an error occurred while setting the file
 * position indicator, or `SCU_ERROR_NONE` on success.
 *
 * @note For binary streams, the new position is exactly `offset` bytes from
 * `seekOrigin`, which is either from the beginning of the file
 * (`SCU_SEEK_ORIGIN_SET`), from the current position (`SCU_SEEK_ORIGIN_CUR`),
 * or from the end of the file (`SCU_SEEK_ORIGIN_END`). Binary streams are not
 * required to support seeking from `SCU_SEEK_ORIGIN_END`, in particular if
 * additional null bytes are output.
 *
 * For text streams, the only supported values for `offset` are zero (works with
 * any `seekOrigin`) and a value returned by a previous call to `scu_ftell()` on
 * a file stream associated with the same file (works with `SCU_SEEK_ORIGIN_SET`
 * only).
 *
 * A call to this function undoes the effects of previous calls to `funreadc()`
 * and clears the end-of-file indicator of the file stream.
 */
SCU_API ScuError scu_fseek(
    ScuFile* file,
    ScuSeekOrigin seekOrigin,
    Scuisize offset
);

/**
 * @brief Sets the file position indicator of a file stream to the  beginning of
 * the file.
 *
 * @param[in, out] file The file stream to modify.
 *
 * @note Calling this function is equivalent to calling `scu_fseek(file,
 * SCU_SEEK_ORIGIN_SET, 0)`, except that both the end-of-file and error
 * indicators of the file stream are cleared.
 *
 * A call to this function also undoes the effects of previous calls to
 * `funreadc()`.
 */
SCU_API void scu_frewind(ScuFile* file);

/**
 * @brief Clears the end-of-file and error indicators of a file stream.
 *
 * @param[in, out] file The file stream to modify.
 */
SCU_API void scu_fclear(ScuFile* file);

/**
 * @brief Determines if the end of a file stream has been reached.
 *
 * @param[in] file The file stream to check.
 * @return `true` if the end of the file stream has been reached, `false`
 * otherwise.
 *
 * @note As this function only returns the end-of-file indicator as reported by
 * the last I/O operation, it only makes sense to call it **after** such an
 * operation failed to distinguish between an end-of-file condition and an
 * actual error.
 */
SCU_API bool scu_feof(ScuFile* file);

/**
 * @brief Determines if an error has occurred on a file stream.
 *
 * @param[in] file The file stream to check.
 * @return `true` if an error has occurred on the file stream, `false`
 * otherwise.
 */
SCU_API bool scu_ferror(ScuFile* file);

#endif
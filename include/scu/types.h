#ifndef SCU_TYPES_H
#define SCU_TYPES_H

#include <float.h>
#include <inttypes.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <uchar.h>
#include "scu/assert.h"

/** @brief An unsigned integer for raw memory access. */
typedef unsigned char Scubyte;

/** @brief An 8-bit signed integer. */
typedef int8_t Scui8;

/** @brief An 8-bit unsigned integer. */
typedef uint8_t Scuu8;

/** @brief A 16-bit signed integer. */
typedef int16_t Scui16;

/** @brief A 16-bit unsigned integer. */
typedef uint16_t Scuu16;

/** @brief A 32-bit signed integer. */
typedef int32_t Scui32;

/** @brief A 32-bit unsigned integer. */
typedef uint32_t Scuu32;

/** @brief A 64-bit signed integer. */
typedef int64_t Scui64;

/** @brief A 64-bit unsigned integer. */
typedef uint64_t Scuu64;

/** @brief A signed integer capable of holding a pointer. */
typedef intptr_t Scuiptr;

/** @brief An unsigned integer capable of holding a pointer. */
typedef uintptr_t Scuuptr;

/** @brief A signed integer for sizes, counts and indices. */
typedef ptrdiff_t Scuisize;

/** @brief An unsigned integer for sizes, counts and indices. */
typedef size_t Scuusize;

/** @brief A 32-bit floating point number. */
typedef float Scuf32;

/** @brief A 64-bit floating point number. */
typedef double Scuf64;

/** @brief An unsigned integer used for UTF-8. */
typedef unsigned char Scuchar8;

/** @brief An unsigned integer used for UTF-16. */
typedef char16_t Scuchar16;

/** @brief An unsigned integer used for UTF-32. */
typedef char32_t Scuchar32;

/** @brief The bit width of the `Scubyte` type. */
#define SCU_BYTE_WIDTH UCHAR_WIDTH

/** @brief The bit width of the `Scui8` type. */
#define SCU_I8_WIDTH INT8_WIDTH

/** @brief The bit width of the `Scuu8` type. */
#define SCU_U8_WIDTH UINT8_WIDTH

/** @brief The bit width of the `Scui16` type. */
#define SCU_I16_WIDTH INT16_WIDTH

/** @brief The bit width of the `Scuu16` type. */
#define SCU_U16_WIDTH UINT16_WIDTH

/** @brief The bit width of the `Scui32` type. */
#define SCU_I32_WIDTH INT32_WIDTH

/** @brief The bit width of the `Scuu32` type. */
#define SCU_U32_WIDTH UINT32_WIDTH

/** @brief The bit width of the `Scui64` type. */
#define SCU_I64_WIDTH INT64_WIDTH

/** @brief The bit width of the `Scuu64` type. */
#define SCU_U64_WIDTH UINT64_WIDTH

#ifdef INTPTR_WIDTH
    /** @brief The bit width of the `Scuiptr` type. */
    #define SCU_IPTR_WIDTH INTPTR_WIDTH
#elif INTPTR_MAX == INT32_MAX
    /** @brief The bit width of the `Scuiptr` type. */
    #define SCU_IPTR_WIDTH 32
#elif INTPTR_MAX == INT64_MAX
    /** @brief The bit width of the `Scuiptr` type. */
    #define SCU_IPTR_WIDTH 64
#else
    #error "Unexpected intptr_t width."
#endif

#ifdef UINTPTR_WIDTH
    /** @brief The bit width of the `Scuuptr` type. */
    #define SCU_UPTR_WIDTH UINTPTR_WIDTH
#elif UINTPTR_MAX == UINT32_MAX
    /** @brief The bit width of the `Scuuptr` type. */
    #define SCU_UPTR_WIDTH 32
#elif UINTPTR_MAX == UINT64_MAX
    /** @brief The bit width of the `Scuuptr` type. */
    #define SCU_UPTR_WIDTH 64
#else
    #error "Unexpected uintptr_t width."
#endif

#ifdef PTRDIFF_WIDTH
    /** @brief The bit width of the `Scuisize` type. */
    #define SCU_ISIZE_WIDTH PTRDIFF_WIDTH
#elif PTRDIFF_MAX == INT32_MAX
    /** @brief The bit width of the `Scuisize` type. */
    #define SCU_ISIZE_WIDTH 32
#elif PTRDIFF_MAX == INT64_MAX
    /** @brief The bit width of the `Scuisize` type. */
    #define SCU_ISIZE_WIDTH 64
#else
    #error "Unexpected ptrdiff_t width."
#endif

#ifdef SIZE_WIDTH
    /** @brief The bit width of the `Scuusize` type. */
    #define SCU_USIZE_WIDTH SIZE_WIDTH
#elif SIZE_MAX == UINT32_MAX
    /** @brief The bit width of the `Scuusize` type. */
    #define SCU_USIZE_WIDTH 32
#elif SIZE_MAX == UINT64_MAX
    /** @brief The bit width of the `Scuusize` type. */
    #define SCU_USIZE_WIDTH 64
#else
    #error "Unexpected size_t width."
#endif

/** @brief The bit width of the `Scuf32` type. */
#define SCU_F32_WIDTH 32

/** @brief The bit width of the `Scuf64` type. */
#define SCU_F64_WIDTH 64

/** @brief The bit width of the `Scuchar8` type. */
#define SCU_CHAR8_WIDTH SCU_BYTE_WIDTH

/** @brief The bit width of the `Scuchar16` type. */
#define SCU_CHAR16_WIDTH UINT_LEAST16_WIDTH

/** @brief The bit width of the `Scuchar32` type. */
#define SCU_CHAR32_WIDTH UINT_LEAST32_WIDTH

/** @brief The minimum value of the `Scubyte` type. */
#define SCU_BYTE_MIN 0

/** @brief The minimum value of the `Scui8` type. */
#define SCU_I8_MIN INT8_MIN

/** @brief The minimum value of the `Scuu8` type. */
#define SCU_U8_MIN 0

/** @brief The minimum value of the `Scui16` type. */
#define SCU_I16_MIN INT16_MIN

/** @brief The minimum value of the `Scuu16` type. */
#define SCU_U16_MIN 0

/** @brief The minimum value of the `Scui32` type. */
#define SCU_I32_MIN INT32_MIN

/** @brief The minimum value of the `Scuu32` type. */
#define SCU_U32_MIN 0

/** @brief The minimum value of the `Scui64` type. */
#define SCU_I64_MIN INT64_MIN

/** @brief The minimum value of the `Scuu64` type. */
#define SCU_U64_MIN 0

/** @brief The minimum value of the `Scuiptr` type. */
#define SCU_IPTR_MIN INTPTR_MIN

/** @brief The minimum value of the `Scuuptr` type. */
#define SCU_UPTR_MIN 0

/** @brief The minimum value of the `Scuisize` type. */
#define SCU_ISIZE_MIN PTRDIFF_MIN

/** @brief The minimum value of the `Scuusize` type. */
#define SCU_USIZE_MIN 0

/** @brief The minimum normalized positive value of the `Scuf32` type. */
#define SCU_F32_MIN FLT_MIN

/** @brief The minimum positive value of the `Scuf32` type. */
#define SCU_F32_TRUE_MIN FLT_TRUE_MIN

/** @brief The lowest (most negative) value of the `Scuf32` type. */
#define SCU_F32_LOWEST (-FLT_MAX)

/**
 * @brief The absolute difference between one and the next value representable
 * by the `Scuf32` type.
 */
#define SCU_F32_EPSILON FLT_EPSILON

/** @brief The minimum normalized positive value of the `Scuf64` type. */
#define SCU_F64_MIN DBL_MIN

/** @brief The minimum positive value of the `Scuf64` type. */
#define SCU_F64_TRUE_MIN DBL_TRUE_MIN

/** @brief The lowest (most negative) value of the `Scuf64` type. */
#define SCU_F64_LOWEST (-DBL_MAX)

/**
 * @brief The absolute difference between one and the next value representable
 * by the `Scuf64` type.
 */
#define SCU_F64_EPSILON DBL_EPSILON

/** @brief The minimum value of the `Scuchar8` type. */
#define SCU_CHAR8_MIN SCU_BYTE_MIN

/** @brief The minimum value of the `Scuchar16` type. */
#define SCU_CHAR16_MIN 0

/** @brief The minimum value of the `Scuchar32` type. */
#define SCU_CHAR32_MIN 0

/** @brief The maximum value of the `Scubyte` type. */
#define SCU_BYTE_MAX UCHAR_MAX

/** @brief The maximum value of the `Scui8` type. */
#define SCU_I8_MAX INT8_MAX

/** @brief The maximum value of the `Scuu8` type. */
#define SCU_U8_MAX UINT8_MAX

/** @brief The maximum value of the `Scui16` type. */
#define SCU_I16_MAX INT16_MAX

/** @brief The maximum value of the `Scuu16` type. */
#define SCU_U16_MAX UINT16_MAX

/** @brief The maximum value of the `Scui32` type. */
#define SCU_I32_MAX INT32_MAX

/** @brief The maximum value of the `Scuu32` type. */
#define SCU_U32_MAX UINT32_MAX

/** @brief The maximum value of the `Scui64` type. */
#define SCU_I64_MAX INT64_MAX

/** @brief The maximum value of the `Scuu64` type. */
#define SCU_U64_MAX UINT64_MAX

/** @brief The maximum value of the `Scuiptr` type. */
#define SCU_IPTR_MAX INTPTR_MAX

/** @brief The maximum value of the `Scuuptr` type. */
#define SCU_UPTR_MAX UINTPTR_MAX

/** @brief The maximum value of the `Scuisize` type. */
#define SCU_ISIZE_MAX PTRDIFF_MAX

/** @brief The maximum value of the `Scuusize` type. */
#define SCU_USIZE_MAX SIZE_MAX

/** @brief The maximum value of the `Scuf32` type. */
#define SCU_F32_MAX FLT_MAX

/** @brief The maximum value of the `Scuf64` type. */
#define SCU_F64_MAX DBL_MAX

/** @brief The maximum value of the `Scuchar8` type. */
#define SCU_CHAR8_MAX SCU_BYTE_MAX

/** @brief The maximum value of the `Scuchar16` type. */
#define SCU_CHAR16_MAX UINT_LEAST16_MAX

/** @brief The maximum value of the `Scuchar32` type. */
#define SCU_CHAR32_MAX UINT_LEAST32_MAX

/**
 * @brief The format specifier for scanning an `Scubyte` value in octal
 * representation.
 */
#define SCU_BYTE_SCNO "hho"

/**
 * @brief The format specifier for scanning an `Scubyte` value in decimal
 * representation.
 */
#define SCU_BYTE_SCNU "hhu"

/**
 * @brief The format specifier for scanning an `Scubyte` value in hexadecimal
 * representation.
 */
#define SCU_BYTE_SCNX "hhX"

/**
 * @brief The format specifier for scanning an `Scui8` value in decimal
 * representation.
 */
#define SCU_I8_SCND SCNd8

/**
 * @brief The format specifier for scanning an `Scui8` value in octal, decimal
 * or hexadecimal representation.
 */
#define SCU_I8_SCNI SCNi8

/**
 * @brief The format specifier for scanning an `Scuu8` value in octal
 * representation.
 */
#define SCU_U8_SCNO SCNo8

/**
 * @brief The format specifier for scanning an `Scuu8` value in decimal
 * representation.
 */
#define SCU_U8_SCNU SCNu8

/**
 * @brief The format specifier for scanning an `Scuu8` value in hexadecimal
 * representation.
 */
#define SCU_U8_SCNX SCNx8

/**
 * @brief The format specifier for scanning an `Scui16` value in decimal
 * representation.
 */
#define SCU_I16_SCND SCNd16

/**
 * @brief The format specifier for scanning an `Scui16` value in octal, decimal
 * or hexadecimal representation.
 */
#define SCU_I16_SCNI SCNi16

/**
 * @brief The format specifier for scanning an `Scuu16` value in octal
 * representation.
 */
#define SCU_U16_SCNO SCNo16

/**
 * @brief The format specifier for scanning an `Scuu16` value in decimal
 * representation.
 */
#define SCU_U16_SCNU SCNu16

/**
 * @brief The format specifier for scanning an `Scuu16` value in hexadecimal
 * representation.
 */
#define SCU_U16_SCNX SCNx16

/**
 * @brief The format specifier for scanning an `Scui32` value in decimal
 * representation.
 */
#define SCU_I32_SCND SCNd32

/**
 * @brief The format specifier for scanning an `Scui32` value in octal, decimal
 * or hexadecimal representation.
 */
#define SCU_I32_SCNI SCNi32

/**
 * @brief The format specifier for scanning an `Scuu32` value in octal
 * representation.
 */
#define SCU_U32_SCNO SCNo32

/**
 * @brief The format specifier for scanning an `Scuu32` value in decimal
 * representation.
 */
#define SCU_U32_SCNU SCNu32

/**
 * @brief The format specifier for scanning an `Scuu32` value in hexadecimal
 * representation.
 */
#define SCU_U32_SCNX SCNx32

/**
 * @brief The format specifier for scanning an `Scui64` value in decimal
 * representation.
 */
#define SCU_I64_SCND SCNd64

/**
 * @brief The format specifier for scanning an `Scui64` value in octal, decimal
 * or hexadecimal representation.
 */
#define SCU_I64_SCNI SCNi64

/**
 * @brief The format specifier for scanning an `Scuu64` value in octal
 * representation.
 */
#define SCU_U64_SCNO SCNo64

/**
 * @brief The format specifier for scanning an `Scuu64` value in decimal
 * representation.
 */
#define SCU_U64_SCNU SCNu64

/**
 * @brief The format specifier for scanning an `Scuu64` value in hexadecimal
 * representation.
 */
#define SCU_U64_SCNX SCNx64

/**
 * @brief The format specifier for scanning an `Scuiptr` value in decimal
 * representation.
 */
#define SCU_IPTR_SCND SCNdPTR

/**
 * @brief The format specifier for scanning an `Scuiptr` value in octal,
 * decimal or hexadecimal representation.
 */
#define SCU_IPTR_SCNI SCNiPTR

/**
 * @brief The format specifier for scanning an `Scuuptr` value in octal
 * representation.
 */
#define SCU_UPTR_SCNO SCNoPTR

/**
 * @brief The format specifier for scanning an `Scuuptr` value in decimal
 * representation.
 */
#define SCU_UPTR_SCNU SCNuPTR

/**
 * @brief The format specifier for scanning an `Scuuptr` value in hexadecimal
 * representation.
 */
#define SCU_UPTR_SCNX SCNxPTR

/**
 * @brief The format specifier for scanning an `Scuisize` value in decimal
 * representation.
 */
#define SCU_ISIZE_SCND "td"

/**
 * @brief The format specifier for scanning an `Scuisize` value in octal,
 * decimal or hexadecimal representation.
 */
#define SCU_ISIZE_SCNI "ti"

/**
 * @brief The format specifier for scanning an `Scuisize` value for the number
 * of bytes read or written so far.
 */
#define SCU_ISIZE_SCNN "tn"

/**
 * @brief The format specifier for scanning an `Scuusize` value in octal
 * representation.
 */
#define SCU_USIZE_SCNO "zo"

/**
 * @brief The format specifier for scanning an `Scuusize` value in decimal
 * representation.
 */
#define SCU_USIZE_SCNU "zu"

/**
 * @brief The format specifier for scanning an `Scuusize` value in hexadecimal
 * representation.
 */
#define SCU_USIZE_SCNX "zX"

/**
 * @brief The format specifier for scanning an `Scuusize` value for the number
 * bytes read or written so far.
 */
#define SCU_USIZE_SCNN "zn"

/**
 * @brief The format specifier for scanning an `Scuf32` value in decimal,
 * decimal exponent or hexadecimal exponent representation.
 */
#define SCU_F32_SCNA "A"

/**
 * @brief The format specifier for scanning an `Scuf32` value in decimal,
 * decimal exponent or hexadecimal exponent representation.
 */
#define SCU_F32_SCNE "E"

/**
 * @brief The format specifier for scanning an `Scuf32` value in decimal,
 * decimal exponent or hexadecimal exponent representation.
 */
#define SCU_F32_SCNF "F"

/**
 * @brief The format specifier for scanning an `Scuf32` value in decimal,
 * decimal exponent or hexadecimal exponent representation.
 */
#define SCU_F32_SCNG "G"

/**
 * @brief The format specifier for scanning an `Scuf64` value in decimal,
 * decimal exponent or hexadecimal exponent representation.
 */
#define SCU_F64_SCNA "lA"

/**
 * @brief The format specifier for scanning an `Scuf64` value in decimal,
 * decimal exponent or hexadecimal exponent representation.
 */
#define SCU_F64_SCNE "lE"

/**
 * @brief The format specifier for scanning an `Scuf64` value in decimal,
 * decimal exponent or hexadecimal exponent representation.
 */
#define SCU_F64_SCNF "lF"

/**
 * @brief The format specifier for scanning an `Scuf64` value in decimal,
 * decimal exponent or hexadecimal exponent representation.
 */
#define SCU_F64_SCNG "lG"

/**
 * @brief The format specifier for scanning an `Scuchar8` value in octal
 * representation.
 */
#define SCU_CHAR8_SCNO SCU_BYTE_SCNO

/**
 * @brief The format specifier for scanning an `Scuchar8` value in decimal
 * representation.
 */
#define SCU_CHAR8_SCNU SCU_BYTE_SCNU

/**
 * @brief The format specifier for scanning an `Scuchar8` value in hexadecimal
 * representation.
 */
#define SCU_CHAR8_SCNX SCU_BYTE_SCNX

/**
 * @brief The format specifier for scanning an `Scuchar16` value in octal
 * representation.
 */
#define SCU_CHAR16_SCNO SCNoLEAST16

/**
 * @brief The format specifier for scanning an `Scuchar16` value in decimal
 * representation.
 */
#define SCU_CHAR16_SCNU SCNuLEAST16

/**
 * @brief The format specifier for scanning an `Scuchar16` value in hexadecimal
 * representation.
 */
#define SCU_CHAR16_SCNX SCNxLEAST16

/**
 * @brief The format specifier for scanning an `Scuchar32` value in octal
 * representation.
 */
#define SCU_CHAR32_SCNO SCNoLEAST32

/**
 * @brief The format specifier for scanning an `Scuchar32` value in decimal
 * representation.
 */
#define SCU_CHAR32_SCNU SCNuLEAST32

/**
 * @brief The format specifier for scanning an `Scuchar32` value in hexadecimal
 * representation.
 */
#define SCU_CHAR32_SCNX SCNxLEAST32

/**
 * @brief The format specifier for printing an `Scubyte` value in octal
 * representation.
 */
#define SCU_BYTE_PRIO "hho"

/**
 * @brief The format specifier for printing an `Scubyte` value in decimal
 * representation.
 */
#define SCU_BYTE_PRIU "hhu"

/**
 * @brief The format specifier for printing an `Scubyte` value in hexadecimal
 * representation.
 */
#define SCU_BYTE_PRIX "hhX"

/**
 * @brief The format specifier for printing an `Scui8` value in decimal
 * representation.
 */
#define SCU_I8_PRID PRId8

/**
 * @brief The format specifier for printing an `Scui8` value in decimal
 * representation.
 */
#define SCU_I8_PRII PRIi8

/**
 * @brief The format specifier for printing an `Scuu8` value in octal
 * representation.
 */
#define SCU_U8_PRIO PRIo8

/**
 * @brief The format specifier for printing an `Scuu8` value in decimal
 * representation.
 */
#define SCU_U8_PRIU PRIu8

/**
 * @brief The format specifier for printing an `Scuu8` value in hexadecimal
 * representation.
 */
#define SCU_U8_PRIX PRIX8

/**
 * @brief The format specifier for printing an `Scui16` value in decimal
 * representation.
 */
#define SCU_I16_PRID PRId16

/**
 * @brief The format specifier for printing an `Scui16` value in decimal
 * representation.
 */
#define SCU_I16_PRII PRIi16

/**
 * @brief The format specifier for printing an `Scuu16` value in octal
 * representation.
 */
#define SCU_U16_PRIO PRIo16

/**
 * @brief The format specifier for printing an `Scuu16` value in decimal
 * representation.
 */
#define SCU_U16_PRIU PRIu16

/**
 * @brief The format specifier for printing an `Scuu16` value in hexadecimal
 * representation.
 */
#define SCU_U16_PRIX PRIX16

/**
 * @brief The format specifier for printing an `Scui32` value in decimal
 * representation.
 */
#define SCU_I32_PRID PRId32

/**
 * @brief The format specifier for printing an `Scui32` value in decimal
 * representation.
 */
#define SCU_I32_PRII PRIi32

/**
 * @brief The format specifier for printing an `Scuu32` value in octal
 * representation.
 */
#define SCU_U32_PRIO PRIo32

/**
 * @brief The format specifier for printing an `Scuu32` value in decimal
 * representation.
 */
#define SCU_U32_PRIU PRIu32

/**
 * @brief The format specifier for printing an `Scuu32` value in hexadecimal
 * representation.
 */
#define SCU_U32_PRIX PRIX32

/**
 * @brief The format specifier for printing an `Scui64` value in decimal
 * representation.
 */
#define SCU_I64_PRID PRId64

/**
 * @brief The format specifier for printing an `Scui64` value in decimal
 * representation.
 */
#define SCU_I64_PRII PRIi64

/**
 * @brief The format specifier for printing an `Scuu64` value in octal
 * representation.
 */
#define SCU_U64_PRIO PRIo64

/**
 * @brief The format specifier for printing an `Scuu64` value in decimal
 * representation.
 */
#define SCU_U64_PRIU PRIu64

/**
 * @brief The format specifier for printing an `Scuu64` value in hexadecimal
 * representation.
 */
#define SCU_U64_PRIX PRIX64

/**
 * @brief The format specifier for printing an `Scuiptr` value in decimal
 * representation.
 */
#define SCU_IPTR_PRID PRIdPTR

/**
 * @brief The format specifier for printing an `Scuiptr` value in decimal
 * representation.
 */
#define SCU_IPTR_PRII PRIiPTR

/**
 * @brief The format specifier for printing an `Scuuptr` value in octal
 * representation.
 */
#define SCU_UPTR_PRIO PRIoPTR

/**
 * @brief The format specifier for printing an `Scuuptr` value in decimal
 * representation.
 */
#define SCU_UPTR_PRIU PRIuPTR

/**
 * @brief The format specifier for printing an `Scuuptr` value in hexadecimal
 * representation.
 */
#define SCU_UPTR_PRIX PRIXPTR

/**
 * @brief The format specifier for printing an `Scuisize` value in decimal
 * representation.
 */
#define SCU_ISIZE_PRID "td"

/**
 * @brief The format specifier for printing an `Scuisize` value in decimal
 * representation.
 */
#define SCU_ISIZE_PRII "ti"

/**
 * @brief The format specifier for printing an `Scuusize` value in octal
 * representation.
 */
#define SCU_USIZE_PRIO "zo"

/**
 * @brief The format specifier for printing an `Scuusize` value in decimal
 * representation.
 */
#define SCU_USIZE_PRIU "zu"

/**
 * @brief The format specifier for printing an `Scuusize` value in hexadecimal
 * representation.
 */
#define SCU_USIZE_PRIX "zX"

/**
 * @brief The format specifier for printing an `Scuf32` value in hexadecimal
 * exponent representation.
 */
#define SCU_F32_PRIA "A"

/**
 * @brief The format specifier for printing an `Scuf32` value in decimal
 * exponent representation.
 */
#define SCU_F32_PRIE "E"

/**
 * @brief The format specifier for printing an `Scuf32` value in decimal
 * representation.
 */
#define SCU_F32_PRIF "F"

/**
 * @brief The format specifier for printing an `Scuf32` value in decimal
 * or decimal exponent representation.
 */
#define SCU_F32_PRIG "G"

/**
 * @brief The format specifier for printing an `Scuf64` value in hexadecimal
 * exponent representation.
 */
#define SCU_F64_PRIA "lA"

/**
 * @brief The format specifier for printing an `Scuf64` value in decimal
 * exponent representation.
 */
#define SCU_F64_PRIE "lE"

/**
 * @brief The format specifier for printing an `Scuf64` value in decimal
 * representation.
 */
#define SCU_F64_PRIF "lF"

/**
 * @brief The format specifier for printing an `Scuf64` value in decimal
 * or decimal exponent representation.
 */
#define SCU_F64_PRIG "lG"

/**
 * @brief The format specifier for printing an `Scuchar8` value in octal
 * representation.
 */
#define SCU_CHAR8_PRIO SCU_BYTE_PRIO

/**
 * @brief The format specifier for printing an `Scuchar8` value in decimal
 * representation.
 */
#define SCU_CHAR8_PRIU SCU_BYTE_PRIU

/**
 * @brief The format specifier for printing an `Scuchar8` value in hexadecimal
 * representation.
 */
#define SCU_CHAR8_PRIX SCU_BYTE_PRIX

/**
 * @brief The format specifier for printing an `Scuchar16` value in octal
 * representation.
 */
#define SCU_CHAR16_PRIO PRIoLEAST16

/**
 * @brief The format specifier for printing an `Scuchar16` value in decimal
 * representation.
 */
#define SCU_CHAR16_PRIU PRIuLEAST16

/**
 * @brief The format specifier for printing an `Scuchar16` value in hexadecimal
 * representation.
 */
#define SCU_CHAR16_PRIX PRIXLEAST16

/**
 * @brief The format specifier for printing an `Scuchar32` value in octal
 * representation.
 */
#define SCU_CHAR32_PRIO PRIoLEAST32

/**
 * @brief The format specifier for printing an `Scuchar32` value in decimal
 * representation.
 */
#define SCU_CHAR32_PRIU PRIuLEAST32

/**
 * @brief The format specifier for printing an `Scuchar32` value in hexadecimal
 * representation.
 */
#define SCU_CHAR32_PRIX PRIXLEAST32

#ifdef SCU_SHORT_ALIASES
    /** @brief An unsigned integer for raw memory access. */
    typedef Scubyte byte;

    /** @brief An 8-bit signed integer. */
    typedef Scui8 i8;

    /** @brief An 8-bit unsigned integer. */
    typedef Scuu8 u8;

    /** @brief A 16-bit signed integer. */
    typedef Scui16 i16;

    /** @brief A 16-bit unsigned integer. */
    typedef Scuu16 u16;

    /** @brief A 32-bit signed integer. */
    typedef Scui32 i32;

    /** @brief A 32-bit unsigned integer. */
    typedef Scuu32 u32;

    /** @brief A 64-bit signed integer. */
    typedef Scui64 i64;

    /** @brief A 64-bit unsigned integer. */
    typedef Scuu64 u64;

    /** @brief A signed integer capable of holding a pointer. */
    typedef Scuiptr iptr;

    /** @brief An unsigned integer capable of holding a pointer. */
    typedef Scuuptr uptr;

    /** @brief A signed integer for sizes, counts and indices. */
    typedef Scuisize isize;

    /** @brief An unsigned integer for sizes, counts and indices. */
    typedef Scuusize usize;

    /** @brief A 32-bit floating point number. */
    typedef Scuf32 f32;

    /** @brief A 64-bit floating point number. */
    typedef Scuf64 f64;

    /** @brief An unsigned integer used for UTF-8. */
    typedef Scuchar8 char8;

    /** @brief An unsigned integer used for UTF-16. */
    typedef Scuchar16 char16;

    /** @brief An unsigned integer used for UTF-32. */
    typedef Scuchar32 char32;

    /** @brief The bit width of the `byte` type. */
    #define BYTE_WIDTH SCU_BYTE_WIDTH

    /** @brief The bit width of the `i8` type. */
    #define I8_WIDTH SCU_I8_WIDTH

    /** @brief The bit width of the `u8` type. */
    #define U8_WIDTH SCU_U8_WIDTH

    /** @brief The bit width of the `i16` type. */
    #define I16_WIDTH SCU_I16_WIDTH

    /** @brief The bit width of the `u16` type. */
    #define U16_WIDTH SCU_U16_WIDTH

    /** @brief The bit width of the `i32` type. */
    #define I32_WIDTH SCU_I32_WIDTH

    /** @brief The bit width of the `u32` type. */
    #define U32_WIDTH SCU_U32_WIDTH

    /** @brief The bit width of the `i64` type. */
    #define I64_WIDTH SCU_I64_WIDTH

    /** @brief The bit width of the `u64` type. */
    #define U64_WIDTH SCU_U64_WIDTH

    /** @brief The bit width of the `iptr` type. */
    #define IPTR_WIDTH SCU_IPTR_WIDTH

    /** @brief The bit width of the `uptr` type. */
    #define UPTR_WIDTH SCU_UPTR_WIDTH

    /** @brief The bit width of the `isize` type. */
    #define ISIZE_WIDTH SCU_ISIZE_WIDTH

    /** @brief The bit width of the `usize` type. */
    #define USIZE_WIDTH SCU_USIZE_WIDTH

    /** @brief The bit width of the `f32` type. */
    #define F32_WIDTH SCU_F32_WIDTH

    /** @brief The bit width of the `f64` type. */
    #define F64_WIDTH SCU_F64_WIDTH

    /** @brief The bit width of the `char8` type. */
    #define CHAR8_WIDTH SCU_CHAR8_WIDTH

    /** @brief The bit width of the `char16` type. */
    #define CHAR16_WIDTH SCU_CHAR16_WIDTH

    /** @brief The bit width of the `char32` type. */
    #define CHAR32_WIDTH SCU_CHAR32_WIDTH

    /** @brief The minimum value of the `byte` type. */
    #define BYTE_MIN SCU_BYTE_MIN

    /** @brief The minimum value of the `i8` type. */
    #define I8_MIN SCU_I8_MIN

    /** @brief The minimum value of the `u8` type. */
    #define U8_MIN SCU_U8_MIN

    /** @brief The minimum value of the `i16` type. */
    #define I16_MIN SCU_I16_MIN

    /** @brief The minimum value of the `u16` type. */
    #define U16_MIN SCU_U16_MIN

    /** @brief The minimum value of the `i32` type. */
    #define I32_MIN SCU_I32_MIN

    /** @brief The minimum value of the `u32` type. */
    #define U32_MIN SCU_U32_MIN

    /** @brief The minimum value of the `i64` type. */
    #define I64_MIN SCU_I64_MIN

    /** @brief The minimum value of the `u64` type. */
    #define U64_MIN SCU_U64_MIN

    /** @brief The minimum value of the `iptr` type. */
    #define IPTR_MIN SCU_IPTR_MIN

    /** @brief The minimum value of the `uptr` type. */
    #define UPTR_MIN SCU_UPTR_MIN

    /** @brief The minimum value of the `isize` type. */
    #define ISIZE_MIN SCU_ISIZE_MIN

    /** @brief The minimum value of the `usize` type. */
    #define USIZE_MIN SCU_USIZE_MIN

    /** @brief The minimum normalized positive value of the `f32` type. */
    #define F32_MIN SCU_F32_MIN

    /** @brief The minimum positive value of the `f32` type. */
    #define F32_TRUE_MIN SCU_F32_TRUE_MIN

    /** @brief The lowest (most negative) value of the `f32` type. */
    #define F32_LOWEST SCU_F32_LOWEST

    /**
     * @brief The absolute difference between one and the next value
     * representable by the `f32` type.
     */
    #define F32_EPSILON SCU_F32_EPSILON

    /** @brief The minimum normalized positive value of the `f64` type. */
    #define F64_MIN SCU_F64_MIN

    /** @brief The minimum positive value of the `f64` type. */
    #define F64_TRUE_MIN SCU_F64_TRUE_MIN

    /** @brief The lowest (most negative) value of the `f64` type. */
    #define F64_LOWEST SCU_F64_LOWEST

    /**
     * @brief The absolute difference between one and the next value
     * representable by the `f64` type.
     */
    #define F64_EPSILON SCU_F64_EPSILON

    /** @brief The minimum value of the `char8` type. */
    #define CHAR8_MIN SCU_CHAR8_MIN

    /** @brief The minimum value of the `char16` type. */
    #define CHAR16_MIN SCU_CHAR16_MIN

    /** @brief The minimum value of the `char32` type. */
    #define CHAR32_MIN SCU_CHAR32_MIN

    /** @brief The maximum value of the `byte` type. */
    #define BYTE_MAX SCU_BYTE_MAX

    /** @brief The maximum value of the `i8` type. */
    #define I8_MAX SCU_I8_MAX

    /** @brief The maximum value of the `u8` type. */
    #define U8_MAX SCU_U8_MAX

    /** @brief The maximum value of the `i16` type. */
    #define I16_MAX SCU_I16_MAX

    /** @brief The maximum value of the `u16` type. */
    #define U16_MAX SCU_U16_MAX

    /** @brief The maximum value of the `i32` type. */
    #define I32_MAX SCU_I32_MAX

    /** @brief The maximum value of the `u32` type. */
    #define U32_MAX SCU_U32_MAX

    /** @brief The maximum value of the `i64` type. */
    #define I64_MAX SCU_I64_MAX

    /** @brief The maximum value of the `u64` type. */
    #define U64_MAX SCU_U64_MAX

    /** @brief The maximum value of the `iptr` type. */
    #define IPTR_MAX SCU_IPTR_MAX

    /** @brief The maximum value of the `uptr` type. */
    #define UPTR_MAX SCU_UPTR_MAX

    /** @brief The maximum value of the `isize` type. */
    #define ISIZE_MAX SCU_ISIZE_MAX

    /** @brief The maximum value of the `usize` type. */
    #define USIZE_MAX SCU_USIZE_MAX

    /** @brief The maximum value of the `f32` type. */
    #define F32_MAX SCU_F32_MAX

    /** @brief The maximum value of the `f64` type. */
    #define F64_MAX SCU_F64_MAX

    /** @brief The maximum value of the `char8` type. */
    #define CHAR8_MAX SCU_CHAR8_MAX

    /** @brief The maximum value of the `char16` type. */
    #define CHAR16_MAX SCU_CHAR16_MAX

    /** @brief The maximum value of the `char32` type. */
    #define CHAR32_MAX SCU_CHAR32_MAX

    /**
     * @brief The format specifier for scanning a `byte` value in octal
     * representation.
     */
    #define BYTE_SCNO SCU_BYTE_SCNO

    /**
     * @brief The format specifier for scanning a `byte` value in decimal
     * representation.
     */
    #define BYTE_SCNU SCU_BYTE_SCNU

    /**
     * @brief The format specifier for scanning a `byte` value in hexadecimal
     * representation.
     */
    #define BYTE_SCNX SCU_BYTE_SCNX

    /**
     * @brief The format specifier for scanning an `i8` value in decimal
     * representation.
     */
    #define I8_SCND SCU_I8_SCND

    /**
     * @brief The format specifier for scanning an `i8` value in octal, decimal
     * or hexadecimal representation.
     */
    #define I8_SCNI SCU_I8_SCNI

    /**
     * @brief The format specifier for scanning a `u8` value in octal
     * representation.
     */
    #define U8_SCNO SCU_U8_SCNO

    /**
     * @brief The format specifier for scanning a `u8` value in decimal
     * representation.
     */
    #define U8_SCNU SCU_U8_SCNU

    /**
     * @brief The format specifier for scanning a `u8` value in hexadecimal
     * representation.
     */
    #define U8_SCNX SCU_U8_SCNX

    /**
     * @brief The format specifier for scanning an `i16` value in decimal
     * representation.
     */
    #define I16_SCND SCU_I16_SCND

    /**
     * @brief The format specifier for scanning an `i16` value in octal, decimal
     * or hexadecimal representation.
     */
    #define I16_SCNI SCU_I16_SCNI

    /**
     * @brief The format specifier for scanning a `u16` value in octal
     * representation.
     */
    #define U16_SCNO SCU_U16_SCNO

    /**
     * @brief The format specifier for scanning a `u16` value in decimal
     * representation.
     */
    #define U16_SCNU SCU_U16_SCNU

    /**
     * @brief The format specifier for scanning a `u16` value in hexadecimal
     * representation.
     */
    #define U16_SCNX SCU_U16_SCNX

    /**
     * @brief The format specifier for scanning an `i32` value in decimal
     * representation.
     */
    #define I32_SCND SCU_I32_SCND

    /**
     * @brief The format specifier for scanning an `i32` value in octal, decimal
     * or hexadecimal representation.
     */
    #define I32_SCNI SCU_I32_SCNI

    /**
     * @brief The format specifier for scanning a `u32` value in octal
     * representation.
     */
    #define U32_SCNO SCU_U32_SCNO

    /**
     * @brief The format specifier for scanning a `u32` value in decimal
     * representation.
     */
    #define U32_SCNU SCU_U32_SCNU

    /**
     * @brief The format specifier for scanning a `u32` value in hexadecimal
     * representation.
     */
    #define U32_SCNX SCU_U32_SCNX

    /**
     * @brief The format specifier for scanning an `i64` value in decimal
     * representation.
     */
    #define I64_SCND SCU_I64_SCND

    /**
     * @brief The format specifier for scanning an `i64` value in octal, decimal
     * or hexadecimal representation.
     */
    #define I64_SCNI SCU_I64_SCNI

    /**
     * @brief The format specifier for scanning a `u64` value in octal
     * representation.
     */
    #define U64_SCNO SCU_U64_SCNO

    /**
     * @brief The format specifier for scanning a `u64` value in decimal
     * representation.
     */
    #define U64_SCNU SCU_U64_SCNU

    /**
     * @brief The format specifier for scanning a `u64` value in hexadecimal
     * representation.
     */
    #define U64_SCNX SCU_U64_SCNX

    /**
     * @brief The format specifier for scanning an `iptr` value in decimal
     * representation.
     */
    #define IPTR_SCND SCU_IPTR_SCND

    /**
     * @brief The format specifier for scanning an `iptr` value in octal,
     * decimal or hexadecimal representation.
     */
    #define IPTR_SCNI SCU_IPTR_SCNI

    /**
     * @brief The format specifier for scanning a `uptr` value in octal
     * representation.
     */
    #define UPTR_SCNO SCU_UPTR_SCNO

    /**
     * @brief The format specifier for scanning a `uptr` value in decimal
     * representation.
     */
    #define UPTR_SCNU SCU_UPTR_SCNU

    /**
     * @brief The format specifier for scanning a `uptr` value in hexadecimal
     * representation.
     */
    #define UPTR_SCNX SCU_UPTR_SCNX

    /**
     * @brief The format specifier for scanning an `isize` value in decimal
     * representation.
     */
    #define ISIZE_SCND SCU_ISIZE_SCND

    /**
     * @brief The format specifier for scanning an `isize` value in octal,
     * decimal or hexadecimal representation.
     */
    #define ISIZE_SCNI SCU_ISIZE_SCNI

    /**
     * @brief The format specifier for scanning an `isize` value for the number
     * of bytes read or written so far.
     */
    #define ISIZE_SCNN SCU_ISIZE_SCNN

    /**
     * @brief The format specifier for scanning a `usize` value in octal
     * representation.
     */
    #define USIZE_SCNO SCU_USIZE_SCNO

    /**
     * @brief The format specifier for scanning a `usize` value in decimal
     * representation.
     */
    #define USIZE_SCNU SCU_USIZE_SCNU

    /**
     * @brief The format specifier for scanning a `usize` value in hexadecimal
     * representation.
     */
    #define USIZE_SCNX SCU_USIZE_SCNX

    /**
     * @brief The format specifier for scanning a `usize` value for the number
     * bytes read or written so far.
     */
    #define USIZE_SCNN SCU_USIZE_SCNN

    /**
     * @brief The format specifier for scanning an `f32` value in decimal,
     * decimal exponent or hexadecimal exponent representation.
     */
    #define F32_SCNA SCU_F32_SCNA

    /**
     * @brief The format specifier for scanning an `f32` value in decimal,
     * decimal exponent or hexadecimal exponent representation.
     */
    #define F32_SCNE SCU_F32_SCNE

    /**
     * @brief The format specifier for scanning an `f32` value in decimal,
     * decimal exponent or hexadecimal exponent representation.
     */
    #define F32_SCNF SCU_F32_SCNF

    /**
     * @brief The format specifier for scanning an `f32` value in decimal,
     * decimal exponent or hexadecimal exponent representation.
     */
    #define F32_SCNG SCU_F32_SCNG

    /**
     * @brief The format specifier for scanning an `f64` value in decimal,
     * decimal exponent or hexadecimal exponent representation.
     */
    #define F64_SCNA SCU_F64_SCNA

    /**
     * @brief The format specifier for scanning an `f64` value in decimal,
     * decimal exponent or hexadecimal exponent representation.
     */
    #define F64_SCNE SCU_F64_SCNE

    /**
     * @brief The format specifier for scanning an `f64` value in decimal,
     * decimal exponent or hexadecimal exponent representation.
     */
    #define F64_SCNF SCU_F64_SCNF

    /**
     * @brief The format specifier for scanning an `f64` value in decimal,
     * decimal exponent or hexadecimal exponent representation.
     */
    #define F64_SCNG SCU_F64_SCNG

    /**
     * @brief The format specifier for scanning a `char8` value in octal
     * representation.
     */
    #define CHAR8_SCNO SCU_CHAR8_SCNO

    /**
     * @brief The format specifier for scanning a `char8` value in decimal
     * representation.
     */
    #define CHAR8_SCNU SCU_CHAR8_SCNU

    /**
     * @brief The format specifier for scanning a `char8` value in hexadecimal
     * representation.
     */
    #define CHAR8_SCNX SCU_CHAR8_SCNX

    /**
     * @brief The format specifier for scanning a `char16` value in octal
     * representation.
     */
    #define CHAR16_SCNO SCU_CHAR16_SCNO

    /**
     * @brief The format specifier for scanning a `char16` value in decimal
     * representation.
     */
    #define CHAR16_SCNU SCU_CHAR16_SCNU

    /**
     * @brief The format specifier for scanning a `char16` value in hexadecimal
     * representation.
     */
    #define CHAR16_SCNX SCU_CHAR16_SCNX

    /**
     * @brief The format specifier for scanning a `char32` value in octal
     * representation.
     */
    #define CHAR32_SCNO SCU_CHAR32_SCNO

    /**
     * @brief The format specifier for scanning a `char32` value in decimal
     * representation.
     */
    #define CHAR32_SCNU SCU_CHAR32_SCNU

    /**
     * @brief The format specifier for scanning a `char32` value in hexadecimal
     * representation.
     */
    #define CHAR32_SCNX SCU_CHAR32_SCNX

    /**
     * @brief The format specifier for printing a `byte` value in octal
     * representation.
     */
    #define BYTE_PRIO SCU_BYTE_PRIO

    /**
     * @brief The format specifier for printing a `byte` value in decimal
     * representation.
     */
    #define BYTE_PRIU SCU_BYTE_PRIU

    /**
     * @brief The format specifier for printing a `byte` value in hexadecimal
     * representation.
     */
    #define BYTE_PRIX SCU_BYTE_PRIX

    /**
     * @brief The format specifier for printing an `i8` value in decimal
     * representation.
     */
    #define I8_PRID SCU_I8_PRID

    /**
     * @brief The format specifier for printing an `i8` value in decimal
     * representation.
     */
    #define I8_PRII SCU_I8_PRII

    /**
     * @brief The format specifier for printing a `u8` value in octal
     * representation.
     */
    #define U8_PRIO SCU_U8_PRIO

    /**
     * @brief The format specifier for printing a `u8` value in decimal
     * representation.
     */
    #define U8_PRIU SCU_U8_PRIU

    /**
     * @brief The format specifier for printing a `u8` value in hexadecimal
     * representation.
     */
    #define U8_PRIX SCU_U8_PRIX

    /**
     * @brief The format specifier for printing an `i16` value in decimal
     * representation.
     */
    #define I16_PRID SCU_I16_PRID

    /**
     * @brief The format specifier for printing an `i16` value in decimal
     * representation.
     */
    #define I16_PRII SCU_I16_PRII

    /**
     * @brief The format specifier for printing a `u16` value in octal
     * representation.
     */
    #define U16_PRIO SCU_U16_PRIO

    /**
     * @brief The format specifier for printing a `u16` value in decimal
     * representation.
     */
    #define U16_PRIU SCU_U16_PRIU

    /**
     * @brief The format specifier for printing a `u16` value in hexadecimal
     * representation.
     */
    #define U16_PRIX SCU_U16_PRIX

    /**
     * @brief The format specifier for printing an `i32` value in decimal
     * representation.
     */
    #define I32_PRID SCU_I32_PRID

    /**
     * @brief The format specifier for printing an `i32` value in decimal
     * representation.
     */
    #define I32_PRII SCU_I32_PRII

    /**
     * @brief The format specifier for printing a `u32` value in octal
     * representation.
     */
    #define U32_PRIO SCU_U32_PRIO

    /**
     * @brief The format specifier for printing a `u32` value in decimal
     * representation.
     */
    #define U32_PRIU SCU_U32_PRIU

    /**
     * @brief The format specifier for printing a `u32` value in hexadecimal
     * representation.
     */
    #define U32_PRIX SCU_U32_PRIX

    /**
     * @brief The format specifier for printing an `i64` value in decimal
     * representation.
     */
    #define I64_PRID SCU_I64_PRID

    /**
     * @brief The format specifier for printing an `i64` value in decimal
     * representation.
     */
    #define I64_PRII SCU_I64_PRII

    /**
     * @brief The format specifier for printing a `u64` value in octal
     * representation.
     */
    #define U64_PRIO SCU_U64_PRIO

    /**
     * @brief The format specifier for printing a `u64` value in decimal
     * representation.
     */
    #define U64_PRIU SCU_U64_PRIU

    /**
     * @brief The format specifier for printing a `u64` value in hexadecimal
     * representation.
     */
    #define U64_PRIX SCU_U64_PRIX

    /**
     * @brief The format specifier for printing an `iptr` value in decimal
     * representation.
     */
    #define IPTR_PRID SCU_IPTR_PRID

    /**
     * @brief The format specifier for printing an `iptr` value in decimal
     * representation.
     */
    #define IPTR_PRII SCU_IPTR_PRII

    /**
     * @brief The format specifier for printing a `uptr` value in octal
     * representation.
     */
    #define UPTR_PRIO SCU_UPTR_PRIO

    /**
     * @brief The format specifier for printing a `uptr` value in decimal
     * representation.
     */
    #define UPTR_PRIU SCU_UPTR_PRIU

    /**
     * @brief The format specifier for printing a `uptr` value in hexadecimal
     * representation.
     */
    #define UPTR_PRIX SCU_UPTR_PRIX

    /**
     * @brief The format specifier for printing an `isize` value in decimal
     * representation.
     */
    #define ISIZE_PRID SCU_ISIZE_PRID

    /**
     * @brief The format specifier for printing an `isize` value in decimal
     * representation.
     */
    #define ISIZE_PRII SCU_ISIZE_PRII

    /**
     * @brief The format specifier for printing a `usize` value in octal
     * representation.
     */
    #define USIZE_PRIO SCU_USIZE_PRIO

    /**
     * @brief The format specifier for printing a `usize` value in decimal
     * representation.
     */
    #define USIZE_PRIU SCU_USIZE_PRIU

    /**
     * @brief The format specifier for printing a `usize` value in hexadecimal
     * representation.
     */
    #define USIZE_PRIX SCU_USIZE_PRIX

    /**
     * @brief The format specifier for printing an `f32` value in hexadecimal
     * exponent representation.
     */
    #define F32_PRIA SCU_F32_PRIA

    /**
     * @brief The format specifier for printing an `f32` value in decimal
     * exponent representation.
     */
    #define F32_PRIE SCU_F32_PRIE

    /**
     * @brief The format specifier for printing an `f32` value in decimal
     * representation.
     */
    #define F32_PRIF SCU_F32_PRIF

    /**
     * @brief The format specifier for printing an `f32` value in decimal or
     * decimal exponent representation.
     */
    #define F32_PRIG SCU_F32_PRIG

    /**
     * @brief The format specifier for printing an `f64` value in hexadecimal
     * exponent representation.
     */
    #define F64_PRIA SCU_F64_PRIA

    /**
     * @brief The format specifier for printing an `f64` value in decimal
     * exponent representation.
     */
    #define F64_PRIE SCU_F64_PRIE

    /**
     * @brief The format specifier for printing an `f64` value in decimal
     * representation.
     */
    #define F64_PRIF SCU_F64_PRIF

    /**
     * @brief The format specifier for printing an `f64` value in decimal or
     * decimal exponent representation.
     */
    #define F64_PRIG SCU_F64_PRIG

    /**
     * @brief The format specifier for printing a `char8` value in octal
     * representation.
     */
    #define CHAR8_PRIO SCU_CHAR8_PRIO

    /**
     * @brief The format specifier for printing a `char8` value in decimal
     * representation.
     */
    #define CHAR8_PRIU SCU_CHAR8_PRIU

    /**
     * @brief The format specifier for printing a `char8` value in hexadecimal
     * representation.
     */
    #define CHAR8_PRIX SCU_CHAR8_PRIX

    /**
     * @brief The format specifier for printing a `char16` value in octal
     * representation.
     */
    #define CHAR16_PRIO SCU_CHAR16_PRIO

    /**
     * @brief The format specifier for printing a `char16` value in decimal
     * representation.
     */
    #define CHAR16_PRIU SCU_CHAR16_PRIU

    /**
     * @brief The format specifier for printing a `char16` value in hexadecimal
     * representation.
     */
    #define CHAR16_PRIX SCU_CHAR16_PRIX

    /**
     * @brief The format specifier for printing a `char32` value in octal
     * representation.
     */
    #define CHAR32_PRIO SCU_CHAR32_PRIO

    /**
     * @brief The format specifier for printing a `char32` value in decimal
     * representation.
     */
    #define CHAR32_PRIU SCU_CHAR32_PRIU

    /**
     * @brief The format specifier for printing a `char32` value in hexadecimal
     * representation.
     */
    #define CHAR32_PRIX SCU_CHAR32_PRIX
#endif

SCU_STATIC_ASSERT(SCU_ISIZE_WIDTH == 32 || SCU_ISIZE_WIDTH == 64);
SCU_STATIC_ASSERT(SCU_ISIZE_WIDTH == SCU_USIZE_WIDTH);
SCU_STATIC_ASSERT(SCU_ISIZE_WIDTH == SCU_IPTR_WIDTH);
SCU_STATIC_ASSERT(SCU_ISIZE_WIDTH == SCU_UPTR_WIDTH);

#endif

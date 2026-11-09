#ifndef SCU_ARRAY_H
#define SCU_ARRAY_H

#include "scu/api.h"
#include "scu/compare.h"
#include "scu/memory.h"

/**
 * @brief Returns the number of elements in a statically allocated array.
 *
 * @param[in] array The statically allocated array.
 * @return The number of elements in the statically allocated array.
 *
 * @warning The behavior is undefined if `array` is not a statically allocated
 * array.
 */
#define SCU_COUNTOF(array) (SCU_SIZEOF(array) / SCU_SIZEOF((array)[0]))

/**
 * @brief Iterates over each element in a statically allocated array.
 *
 * This macro expands to a for loop that iterates over each element in the
 * statically allocated array. During each iteration, the provided variable is
 * assigned a pointer to the current element.
 *
 * The following example demonstrates the basic usage of this macro:
 *
 * ```c
 * // T is the type of the elements stored in the statically allocated array.
 * T elems[] = { ... };
 * ...
 * T* elem;
 * SCU_ARRAY_FOREACH(elem, elems) {
 *     // Do something with *elem.
 * }
 * ```
 *
 * @param[out] elem  A pointer to the current element.
 * @param[in]  array The statically allocated array.
 *
 * @warning The behavior is undefined if `array` is not a statically allocated
 * array.
 *
 * @note The variable `elem` must be declared manually before the loop. It must
 * be of a pointer type compatible with the array's element type.
 */
#define SCU_ARRAY_FOREACH(elem, array)                                        \
    for ((elem) = (array); (elem) < ((array) + SCU_COUNTOF(array)); (elem)++)

/**
 * @brief Sorts an array using a specified comparison function.
 *
 * @param[in, out] array    The array to sort.
 * @param[in]      count    The number of elements in the array.
 * @param[in]      elemSize The size of each element (in bytes).
 * @param[in]      cmpFunc  A comparison function used to determine the order of
 *                          the elements.
 *
 * @warning The behavior is undefined if `array` does not point to a block of
 * memory of at least `count * elemSize` bytes, if `count` is negative, if
 * `elemSize` is less than or equal to zero, if `cmpFunc` is `nullptr` or
 * modifies the elements passed to it, or if `cmpFunc` returns inconsistent
 * results when called repeatedly with the same arguments.
 *
 * @note If `count` is zero, `array` is ignored and it may even be `nullptr`.
 *
 * No guarantees are made regarding the complexity and stability of the
 * underlying sorting algorithm.
 */
SCU_API void scu_array_sort(
    void* array,
    Scuisize count,
    Scuisize elemSize,
    ScuCompareFunc* cmpFunc
);

#endif

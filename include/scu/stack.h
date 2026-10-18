#ifndef SCU_STACK_H
#define SCU_STACK_H

#include "scu/common.h"
#include "scu/error.h"
#include "scu/types.h"

/** @brief A last-in-first-out (LIFO) collection of elements. */
typedef struct ScuStack ScuStack;

/**
 * @brief An iterator for a stack.
 *
 * @warning The internal representation of the iterator is an implementation
 * detail and should not be relied upon. Most importantly, the behavior is
 * undefined if its fields are accessed directly.
 */
typedef struct ScuStackIterator {

    /** @brief The stack being iterated over. */
    ScuStack* stack;

    /** @brief The current index within the stack. */
    Scuisize index;

} ScuStackIterator;

/**
 * @brief Constructs a stack with a specified element size and an unspecified
 * default capacity.
 *
 * @param[in] elemSize The size of each element (in bytes).
 * @return A pointer to the stack, or `nullptr` on failure.
 *
 * @warning The caller is responsible for deallocating the stack with
 * `scu_stack_free()` when it is no longer needed.
 *
 * @note This function dynamically allocates memory using `scu_malloc()`.
 */
ScuStack* scu_stack_new(Scuisize elemSize);

/**
 * @brief Constructs a stack with a specified element size and initial capacity.
 *
 * @param[in] elemSize The size of each element (in bytes).
 * @param[in] capacity The initial capacity (in number of elements).
 * @return A pointer to the stack, or `nullptr` on failure.
 *
 * @warning The caller is responsible for deallocating the stack with
 * `scu_stack_free()` when it is no longer needed.
 *
 * @note This function dynamically allocates memory using `scu_malloc()`.
 */
ScuStack* scu_stack_new_with_capacity(Scuisize elemSize, Scuisize capacity);

/**
 * @brief Creates a shallow copy of a stack.
 *
 * @param[in] stack The stack to clone.
 * @return A pointer to the cloned stack, or `nullptr` on failure.
 *
 * @warning The caller is responsible for deallocating the cloned stack with
 * `scu_stack_free()` when it is no longer needed.
 *
 * @note This function dynamically allocates memory using `scu_malloc()`.
 */
ScuStack* scu_stack_clone(const ScuStack* stack);

/**
 * @brief Returns the capacity of a stack, i.e., the maximum number of elements
 * that can be stored before a reallocation is required.
 *
 * @param[in] stack The stack to examine.
 * @return The capacity of the stack.
 *
 * @note The capacity can be influenced to a certain extent using
 * `scu_stack_ensure_capacity()` and `scu_stack_trim_excess()`. See their
 * respective documentation for details.
 */
Scuisize scu_stack_capacity(const ScuStack* stack);

/**
 * @brief Returns the number of elements in a stack.
 *
 * @param[in] stack The stack to examine.
 * @return The number of elements in the stack.
 */
Scuisize scu_stack_count(const ScuStack* stack);

/**
 * @brief Ensures that a stack has at least a specified capacity.
 *
 * @param[in, out] stack    The stack whose capacity to ensure.
 * @param[in]      capacity The desired capacity (in number of elements).
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred, or
 * `SCU_ERROR_NONE` on success.
 *
 * @warning This function may invalidate any iterators previously created with
 * `scu_stack_begin()` and `scu_stack_end()`.
 *
 * @note This function dynamically allocates memory using `scu_realloc()`.
 */
ScuError scu_stack_ensure_capacity(ScuStack* stack, Scuisize capacity);

/**
 * @brief Pushes an element onto the top of a stack.
 *
 * @param[in, out] stack The stack to push the element onto.
 * @param[in]      elem  The element to push onto the stack.
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred, or
 * `SCU_ERROR_NONE` on success.
 *
 * @warning This function invalidates any iterators previously created with
 * `scu_stack_begin()` and `scu_stack_end()`.
 *
 * @note This function dynamically allocates memory using `scu_realloc()`.
 */
ScuError scu_stack_push(ScuStack* restrict stack, const void* restrict elem);

/**
 * @brief Removes the top element from a stack.
 *
 * @param[in, out] stack The stack to remove the top element from.
 * @param[out]     elem  The removed top element.
 *
 * @warning This function invalidates any iterators previously created with
 * `scu_stack_begin()` and `scu_stack_end()`.
 *
 * The behavior is undefined if the stack is empty.
 */
void scu_stack_pop(ScuStack* restrict stack, void* restrict elem);

/**
 * @brief Tries to remove the top element from a stack.
 *
 * @param[in, out] stack The stack to remove the top element from.
 * @param[out]     elem  The removed top element if the stack was not empty,
 *                       unchanged otherwise.
 * @return `true` if the top element was successfully removed, `false`
 * otherwise.
 *
 * @warning This function may invalidate any iterators previously created with
 * `scu_stack_begin()` and `scu_stack_end()`.
 */
bool scu_stack_try_pop(ScuStack* restrict stack, void* restrict elem);

/**
 * @brief Retrieves the top element of a stack without removing it.
 *
 * @param[in]  stack The stack to examine.
 * @param[out] elem  A pointer to the top element of the stack.
 *
 * @warning The behavior is undefined if the stack is empty.
 *
 * @note This function is an implementation detail and not intended to be called
 * directly. Use the `scu_stack_peek()` macro instead.
 */
void scu_stack_peek_impl(const ScuStack* restrict stack, void** restrict elem);

/**
 * @brief Retrieves the top element of a stack without removing it.
 *
 * @param[in]  stack The stack to examine.
 * @param[out] elem  A pointer to the top element of the stack.
 *
 * @warning The behavior is undefined if the stack is empty.
 */
#define scu_stack_peek(stack, elem) scu_stack_peek_impl(stack, (void**) (elem))

/**
 * @brief Tries to retrieve the top element of a stack without removing it.
 *
 * @param[in]  stack The stack to examine.
 * @param[out] elem  A pointer to the top element of the stack on success,
 *                   or `nullptr` on failure.
 * @return `true` if the top element was successfully retrieved, `false`
 * otherwise.
 *
 * @note This function is an implementation detail and not intended to be called
 * directly. Use the `scu_stack_try_peek()` macro instead.
 */
bool scu_stack_try_peek_impl(
    const ScuStack* restrict stack,
    void** restrict elem
);

/**
 * @brief Tries to retrieve the top element of a stack without removing it.
 *
 * @param[in]  stack The stack to examine.
 * @param[out] elem  A pointer to the top element of the stack on success,
 *                   or `nullptr` on failure.
 * @return `true` if the top element was successfully retrieved, `false`
 * otherwise.
 */
#define scu_stack_try_peek(stack, elem)             \
    scu_stack_try_peek_impl(stack, (void**) (elem))

/**
 * @brief Removes all elements from a stack.
 *
 * @param[in, out] stack The stack to clear.
 *
 * @warning This function invalidates any iterators previously created with
 * `scu_stack_begin()` and `scu_stack_end()`.
 *
 * This function does not deallocate the stack itself nor the elements contained
 * within, it only resets the number of elements to zero. The caller is
 * responsible for deallocating the individual elements if they are pointers to
 * dynamically allocated objects and no other references to them exist.
 *
 * @note Consider using `scu_stack_trim_excess()` if you wish to reduce the
 * memory usage of the stack after clearing it.
 */
void scu_stack_clear(ScuStack* stack);

/**
 * @brief Trims the excess capacity of a stack to match its current number of
 * elements.
 *
 * @param[in, out] stack The stack to trim.
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred, or
 * `SCU_ERROR_NONE` on success.
 *
 * @warning This function may invalidate any iterators previously created with
 * `scu_stack_begin()` and `scu_stack_end()`.
 *
 * @note This function dynamically allocates memory using `scu_realloc()`.
 */
ScuError scu_stack_trim_excess(ScuStack* stack);

/**
 * @brief Deallocates a stack.
 *
 * @param[in, out] stack The stack to deallocate.
 *
 * @warning This function only deallocates the memory occupied by the stack
 * itself, but not the elements contained within. The caller is responsible for
 * deallocating the individual elements if they are pointers to dynamically
 * allocated objects and no other references to them exist.
 *
 * The behavior is undefined if `stack` is used after it has been deallocated.
 *
 * @note If `stack` is `nullptr`, this function does nothing.
 */
void scu_stack_free(ScuStack* stack);

/**
 * @brief Returns an iterator to the top element of a stack.
 *
 * @param[in] stack The stack to iterate over.
 * @return An iterator to the top element of the stack.
 *
 * @note If `stack` is empty, this function behaves identically to
 * `scu_stack_end(stack)`, i.e., it returns the end iterator.
 */
ScuStackIterator scu_stack_begin(const ScuStack* stack);

/**
 * @brief Returns the end iterator for a stack.
 *
 * @param[in] stack The stack to iterate over.
 * @return The end iterator for the stack.
 */
ScuStackIterator scu_stack_end(const ScuStack* stack);

/**
 * @brief Determines whether two iterators are equal.
 *
 * @param[in] lhs The first iterator.
 * @param[in] rhs The second iterator.
 * @return `true` if the two iterators are equal, `false` otherwise.
 *
 * @warning The behavior is undefined if `lhs` and `rhs` do not belong to the
 * same stack.
 */
bool scu_stack_iterator_equal(
    const ScuStackIterator* lhs,
    const ScuStackIterator* rhs
);

/**
 * @brief Moves an iterator to the next element.
 *
 * @param[in, out] iterator The iterator to move.
 *
 * @warning The behavior is undefined if moving `iterator` would cause it to
 * point after the end iterator returned by `scu_stack_end()`.
 */
void scu_stack_iterator_next(ScuStackIterator* iterator);

/**
 * @brief Moves an iterator to the previous element.
 *
 * @param[in, out] iterator The iterator to move.
 *
 * @warning The behavior is undefined if moving `iterator` would cause it to
 * point before the begin iterator returned by `scu_stack_begin()`.
 */
void scu_stack_iterator_prev(ScuStackIterator* iterator);

/**
 * @brief Returns a pointer to the current element an iterator points to.
 *
 * @param[in] iterator The iterator to dereference.
 * @return A pointer to the current element the iterator points to.
 *
 * @warning The behavior is undefined if `iterator` points before the begin
 * iterator returned by `scu_stack_begin()`, if it is the end iterator returned
 * by `scu_stack_end()`, or if it points after the end iterator.
 */
void* scu_stack_iterator_deref(const ScuStackIterator* iterator);

/**
 * @brief Iterates over each element in a stack.
 *
 * This macro expands to a for loop that iterates over each element in the
 * stack (starting at the top). During each iteration, the provided variable is
 * assigned a pointer to the current element.
 *
 * The following example demonstrates the basic usage of this macro:
 *
 * ```c
 * // T is the type of the elements stored in the stack.
 * ScuStack* stack = scu_stack_new(SCU_SIZEOF(T));
 * ...
 * T* elem;
 * SCU_STACK_FOREACH(elem, stack) {
 *     // Do something with *elem.
 * }
 * ```
 *
 * @param[out] elem  A pointer to the current element during each iteration.
 * @param[in]  stack The stack to iterate over.
 *
 * @warning The behavior is undefined if the stack is modified (e.g., elements
 * are added or removed) while being iterated over.
 *
 * @note The variable `elem` must be declared manually before the loop. It must
 * be of a pointer type compatible with the stack's element type.
 */
#define SCU_STACK_FOREACH(elem, stack)                                         \
    for (                                                                      \
        ScuStackIterator SCU_XCONCAT(it, __LINE__) = scu_stack_begin(stack),   \
        SCU_XCONCAT(end, __LINE__) = scu_stack_end(stack);                     \
        !scu_stack_iterator_equal(                                             \
            &SCU_XCONCAT(it, __LINE__),                                        \
            &SCU_XCONCAT(end, __LINE__)                                        \
        )                                                                      \
            && (                                                               \
                (elem) = scu_stack_iterator_deref(&SCU_XCONCAT(it, __LINE__)), \
                true                                                           \
            );                                                                 \
        scu_stack_iterator_next(&SCU_XCONCAT(it, __LINE__))                    \
    )

#endif
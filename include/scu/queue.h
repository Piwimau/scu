#ifndef SCU_QUEUE_H
#define SCU_QUEUE_H

#include "scu/api.h"
#include "scu/error.h"
#include "scu/memory.h"
#include "scu/types.h"
#include "scu/util.h"

/** @brief A first-in-first-out (FIFO) collection of elements. */
typedef struct ScuQueue ScuQueue;

/**
 * @brief An iterator for a queue.
 *
 * @warning The internal representation of the iterator is an implementation
 * detail and should not be relied upon. Most importantly, the behavior is
 * undefined if its fields are accessed directly.
 */
typedef struct ScuQueueIterator {

    /** @brief The queue being iterated over. */
    ScuQueue* queue;

    /** @brief The current index within the queue. */
    Scuisize index;

} ScuQueueIterator;

/**
 * @brief Constructs a queue with a specified element size and an unspecified
 * default capacity.
 *
 * @param[in] elemSize  The size of each element (in bytes).
 * @param[in] allocator The allocator to use for memory management.
 * @return A pointer to the queue, or `nullptr` on failure.
 *
 * @warning The caller is responsible for deallocating the queue with
 * `scu_queue_delete()` when it is no longer needed.
 *
 * @note This function is an implementation detail and not intended to be used
 * directly. Use the `scu_queue_new()` macro instead.
 */
SCU_API ScuQueue* scu_queue_new_impl(
    Scuisize elemSize,
    ScuAllocator* allocator
);

/**
 * @brief Constructs a queue with a specified element type and an unspecified
 * default capacity.
 *
 * @param[in] elemType The type of each element.
 * @param[in] ...      The allocator to use for memory management.
 * @return A pointer to the queue, or `nullptr` on failure.
 *
 * @warning The caller is responsible for deallocating the queue with
 * `scu_queue_delete()` when it is no longer needed.
 *
 * @note The last argument is optional and specifies the allocator to use. If
 * omitted, the default allocator will be used.
 */
#define scu_queue_new(elemType, ...)          \
    scu_queue_new_impl(                       \
        SCU_SIZEOF(elemType),                 \
        SCU_ALLOCATOR_OR_DEFAULT(__VA_ARGS__) \
    )

/**
 * @brief Constructs a queue with a specified element size and initial capacity.
 *
 * @param[in] elemSize  The size of each element (in bytes).
 * @param[in] capacity  The initial capacity (in number of elements).
 * @param[in] allocator The allocator to use for memory management.
 * @return A pointer to the queue, or `nullptr` on failure.
 *
 * @warning The caller is responsible for deallocating the queue with
 * `scu_queue_delete()` when it is no longer needed.
 *
 * @note This function is an implementation detail and not intended to be used
 * directly. Use the `scu_queue_new_with_capacity()` macro instead.
 */
SCU_API ScuQueue* scu_queue_new_with_capacity_impl(
    Scuisize elemSize,
    Scuisize capacity,
    ScuAllocator* allocator
);

/**
 * @brief Constructs a queue with a specified element type and initial capacity.
 *
 * @param[in] elemType The type of each element.
 * @param[in] capacity The initial capacity (in number of elements).
 * @param[in] ...      The allocator to use for memory management.
 * @return A pointer to the queue, or `nullptr` on failure.
 *
 * @warning The caller is responsible for deallocating the queue with
 * `scu_queue_delete()` when it is no longer needed.
 *
 * @note The last argument is optional and specifies the allocator to use. If
 * omitted, the default allocator will be used.
 */
#define scu_queue_new_with_capacity(elemType, capacity, ...) \
    scu_queue_new_with_capacity_impl(                        \
        SCU_SIZEOF(elemType),                                \
        (capacity),                                          \
        SCU_ALLOCATOR_OR_DEFAULT(__VA_ARGS__)                \
    )

/**
 * @brief Creates a shallow copy of a queue.
 *
 * @param[in] queue The queue to clone.
 * @return A pointer to the cloned queue, or `nullptr` on failure.
 *
 * @warning The caller is responsible for deallocating the cloned queue with
 * `scu_queue_delete()` when it is no longer needed.
 */
SCU_API ScuQueue* scu_queue_clone(const ScuQueue* queue);

/**
 * @brief Returns the capacity of a queue, i.e., the maximum number of elements
 * that can be stored before a reallocation is required.
 *
 * @param[in] queue The queue to examine.
 * @return The capacity of the queue.
 *
 * @note The capacity can be influenced to a certain extent using
 * `scu_queue_ensure_capacity()` and `scu_queue_trim_excess()`. See their
 * respective documentation for details.
 */
SCU_API Scuisize scu_queue_capacity(const ScuQueue* queue);

/**
 * @brief Returns the number of elements stored in a queue.
 *
 * @param[in] queue The queue to examine.
 * @return The number of elements stored in the queue.
 */
SCU_API Scuisize scu_queue_count(const ScuQueue* queue);

/**
 * @brief Ensures that a queue has at least a specified capacity.
 *
 * @param[in, out] queue    The queue whose capacity to ensure.
 * @param[in]      capacity The desired capacity (in number of elements).
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred, or
 * `SCU_ERROR_NONE` on success.
 *
 * @warning This function may invalidate any iterators previously created with
 * `scu_queue_begin()` and `scu_queue_end()`.
 */
SCU_API ScuError scu_queue_ensure_capacity(ScuQueue* queue, Scuisize capacity);

/**
 * @brief Enqueues an element into a queue.
 *
 * @param[in, out] queue The queue to enqueue the element into.
 * @param[in]      elem  The element to enqueue.
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred, or
 * `SCU_ERROR_NONE` on success.
 *
 * @warning This function invalidates any iterators previously created with
 * `scu_queue_begin()` and `scu_queue_end()`.
 */
SCU_API ScuError scu_queue_enqueue(
    ScuQueue* restrict queue,
    const void* restrict elem
);

/**
 * @brief Dequeues the element at the front of a queue.
 *
 * @param[in, out] queue The queue to dequeue the element from.
 * @param[out]     elem  The dequeued element.
 *
 * @warning This function invalidates any iterators previously created with
 * `scu_queue_begin()` and `scu_queue_end()`.
 *
 * The behavior is undefined if the queue is empty. Use
 * `scu_queue_try_dequeue()` to handle this case gracefully.
 */
SCU_API void scu_queue_dequeue(ScuQueue* restrict queue, void* restrict elem);

/**
 * @brief Tries to dequeue the element at the front of a queue.
 *
 * @param[in, out] queue The queue to dequeue the element from.
 * @param[out]     elem  The dequeued element on success, unchanged otherwise.
 * @return `true` if an element was successfully dequeued, `false` otherwise.
 *
 * @warning This function may invalidate any iterators previously created with
 * `scu_queue_begin()` and `scu_queue_end()`.
 */
SCU_API bool scu_queue_try_dequeue(
    ScuQueue* restrict queue,
    void* restrict elem
);

/**
 * @brief Returns the element at the front of a queue without removing it.
 *
 * @param[in]  queue The queue to examine.
 * @param[out] elem  A pointer to the element at the front of the queue.
 *
 * @warning The behavior is undefined if the queue is empty. Use
 * `scu_queue_try_peek()` to handle this case gracefully.
 *
 * @note This function is an implementation detail and not intended to be called
 * directly. Use the `scu_queue_peek()` macro instead.
 */
SCU_API void scu_queue_peek_impl(
    const ScuQueue* restrict queue,
    void** restrict elem
);

/**
 * @brief Returns the element at the front of a queue without removing it.
 *
 * @param[in]  queue The queue to examine.
 * @param[out] elem  A pointer to the element at the front of the queue.
 *
 * @warning The behavior is undefined if the queue is empty. Use
 * `scu_queue_try_peek()` to handle this case gracefully.
 */
#define scu_queue_peek(queue, elem) scu_queue_peek_impl(queue, (void**) (elem))

/**
 * @brief Tries to return the element at the front of a queue without removing
 * it.
 *
 * @param[in]  queue The queue to examine.
 * @param[out] elem  A pointer to the element at the front of the queue on
 *                   success, or `nullptr` on failure.
 * @return `true` if an element was successfully retrieved, `false` otherwise.
 *
 * @note This function is an implementation detail and not intended to be called
 * directly. Use the `scu_queue_try_peek()` macro instead.
 */
SCU_API bool scu_queue_try_peek_impl(
    const ScuQueue* restrict queue,
    void** restrict elem
);

/**
 * @brief Tries to return the element at the front of a queue without removing
 * it.
 *
 * @param[in]  queue The queue to examine.
 * @param[out] elem  A pointer to the element at the front of the queue on
 *                   success, or `nullptr` on failure.
 * @return `true` if an element was successfully retrieved, `false` otherwise.
 */
#define scu_queue_try_peek(queue, elem)             \
    scu_queue_try_peek_impl(queue, (void**) (elem))

/**
 * @brief Removes all elements from a queue.
 *
 * @param[in, out] queue The queue to clear.
 *
 * @warning This function invalidates any iterators previously created with
 * `scu_queue_begin()` and `scu_queue_end()`.
 *
 * This function does not deallocate the queue itself nor the elements contained
 * within, it only resets the number of elements to zero. The caller is
 * responsible for deallocating the individual elements if they are pointers to
 * dynamically allocated objects and no other references to them exist.
 *
 * @note Consider using `scu_queue_trim_excess()` if you wish to reduce the
 * memory usage of the queue after clearing it.
 */
SCU_API void scu_queue_clear(ScuQueue* queue);

/**
 * @brief Trims the excess capacity of a queue to match its current number of
 * elements.
 *
 * @param[in, out] queue The queue to trim.
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred, or
 * `SCU_ERROR_NONE` on success.
 *
 * @warning This function may invalidate any iterators previously created with
 * `scu_queue_begin()` and `scu_queue_end()`.
 */
SCU_API ScuError scu_queue_trim_excess(ScuQueue* queue);

/**
 * @brief Deallocates a queue.
 *
 * @param[in] queue The queue to deallocate.
 *
 * @warning This function only deallocates the memory occupied by the queue
 * itself, but not the elements contained within. The caller is responsible for
 * deallocating the individual elements if they are pointers to dynamically
 * allocated objects and no other references to them exist.
 *
 * The behavior is undefined if the queue is used after it has been deallocated.
 *
 * @note If `queue` is `nullptr`, this function does nothing.
 */
SCU_API void scu_queue_delete(ScuQueue* queue);

/**
 * @brief Returns an iterator to the front element of a queue.
 *
 * @param[in] queue The queue to iterate over.
 * @return An iterator to the front element of the queue.
 *
 * @note If `queue` is empty, this function behaves identically to
 * `scu_queue_end(queue)`, i.e., it returns the end iterator.
 */
SCU_API ScuQueueIterator scu_queue_begin(const ScuQueue* queue);

/**
 * @brief Returns the end iterator for a queue.
 *
 * @param[in] queue The queue to iterate over.
 * @return The end iterator for the queue.
 */
SCU_API ScuQueueIterator scu_queue_end(const ScuQueue* queue);

/**
 * @brief Determines whether two iterators are equal.
 *
 * @param[in] lhs The first iterator.
 * @param[in] rhs The second iterator.
 * @return `true` if the two iterators are equal, `false` otherwise.
 *
 * @warning The behavior is undefined if `lhs` and `rhs` do not belong to the
 * same queue.
 */
SCU_API bool scu_queue_iterator_equal(
    const ScuQueueIterator* lhs,
    const ScuQueueIterator* rhs
);

/**
 * @brief Moves an iterator to the next element.
 *
 * @param[in, out] iterator The iterator to move.
 *
 * @warning The behavior is undefined if moving `iterator` would cause it to
 * point after the end iterator returned by `scu_queue_end()`.
 */
SCU_API void scu_queue_iterator_next(ScuQueueIterator* iterator);

/**
 * @brief Moves an iterator to the previous element.
 *
 * @param[in, out] iterator The iterator to move.
 *
 * @warning The behavior is undefined if moving `iterator` would cause it to
 * point before the begin iterator returned by `scu_queue_begin()`.
 */
SCU_API void scu_queue_iterator_prev(ScuQueueIterator* iterator);

/**
 * @brief Returns a pointer to the current element an iterator points to.
 *
 * @param[in] iterator The iterator to dereference.
 * @return A pointer to the current element the iterator points to.
 *
 * @warning The behavior is undefined if `iterator` points before the begin
 * iterator returned by `scu_queue_begin()`, if it is the end iterator returned
 * by `scu_queue_end()`, or if it points after the end iterator.
 */
SCU_API void* scu_queue_iterator_deref(const ScuQueueIterator* iterator);

/**
 * @brief Iterates over each element in a queue.
 *
 * This macro expands to a for loop that iterates over each element in the
 * queue (starting at the front element). During each iteration, the provided
 * variable is assigned a pointer to the current element.
 *
 * The following example demonstrates the basic usage of this macro:
 *
 * ```c
 * // T is the type of the elements stored in the queue.
 * ScuQueue* queue = scu_queue_new(sizeof(T));
 * ...
 * T* elem;
 * SCU_QUEUE_FOREACH(elem, queue) {
 *     // Do something with *elem.
 * }
 * ```
 *
 * @param[out] elem  A pointer to the current element during each iteration.
 * @param[in]  queue The queue to iterate over.
 *
 * @warning The behavior is undefined if the queue is modified (e.g., elements
 * are enqueued or dequeued) while being iterated over.
 *
 * @note The variable `elem` must be declared manually before the loop. It must
 * be of a pointer type compatible with the queue's element type.
 *
 * The elements are returned in the order they would be dequeued, i.e., starting
 * from the front of the queue all the way to the back.
 */
#define SCU_QUEUE_FOREACH(elem, queue)                                         \
    for (                                                                      \
        ScuQueueIterator SCU_XCONCAT(it, __LINE__) = scu_queue_begin(queue),   \
        SCU_XCONCAT(end, __LINE__) = scu_queue_end(queue);                     \
        !scu_queue_iterator_equal(                                             \
            &SCU_XCONCAT(it, __LINE__),                                        \
            &SCU_XCONCAT(end, __LINE__)                                        \
        )                                                                      \
            && (                                                               \
                (elem) = scu_queue_iterator_deref(&SCU_XCONCAT(it, __LINE__)), \
                true                                                           \
            );                                                                 \
        scu_queue_iterator_next(&SCU_XCONCAT(it, __LINE__))                    \
    )

#endif
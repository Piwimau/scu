#ifndef SCU_PRIO_QUEUE_H
#define SCU_PRIO_QUEUE_H

#include "scu/api.h"
#include "scu/compare.h"
#include "scu/error.h"
#include "scu/types.h"
#include "scu/util.h"

/** @brief A collection of elements and associated priorities. */
typedef struct ScuPrioQueue ScuPrioQueue;

/**
 * @brief An iterator for a priority queue.
 *
 * @warning The internal representation of the iterator is an implementation
 * detail and should not be relied upon. Most importantly, the behavior is
 * undefined if its fields are accessed directly.
 */
typedef struct ScuPrioQueueIterator {

    /** @brief The priority queue being iterated over. */
    ScuPrioQueue* prioQueue;

    /** @brief The current index within the priority queue. */
    Scuisize index;

} ScuPrioQueueIterator;

/** @brief An entry in a priority queue. */
typedef struct ScuPrioQueueEntry {

    /** @brief The element of the entry. */
    void* elem;

    /** @brief The priority of the entry. */
    void* prio;

} ScuPrioQueueEntry;

/**
 * @brief Constructs a priority queue with specified element size, priority
 * size, priority comparison function, and an unspecified default capacity.
 *
 * @param[in] elemSize    The size of each element (in bytes).
 * @param[in] prioSize    The size of each priority (in bytes).
 * @param[in] prioCmpFunc A function used for comparing priorities.
 * @return A pointer to the priority queue, or `nullptr` on failure.
 *
 * @warning The caller is responsible for deallocating the priority queue with
 * `scu_prio_queue_free()` when it is no longer needed.
 *
 * @note This function dynamically allocates memory using `scu_malloc()`.
 */
SCU_API ScuPrioQueue* scu_prio_queue_new(
    Scuisize elemSize,
    Scuisize prioSize,
    ScuCompareFunc* prioCmpFunc
);

/**
 * @brief Constructs a priority queue with specified element size, priority
 * size, priority comparison function, and initial capacity.
 *
 * @param[in] elemSize    The size of each element (in bytes).
 * @param[in] prioSize    The size of each priority (in bytes).
 * @param[in] capacity    The initial capacity (in number of elements).
 * @param[in] prioCmpFunc A function used for comparing priorities.
 * @return A pointer to the priority queue, or `nullptr` on failure.
 *
 * @warning The caller is responsible for deallocating the priority queue with
 * `scu_prio_queue_free()` when it is no longer needed.
 *
 * @note This function dynamically allocates memory using `scu_malloc()`.
 */
SCU_API ScuPrioQueue* scu_prio_queue_new_with_capacity(
    Scuisize elemSize,
    Scuisize prioSize,
    Scuisize capacity,
    ScuCompareFunc* prioCmpFunc
);

/**
 * @brief Creates a shallow copy of a priority queue.
 *
 * @param[in] prioQueue The priority queue to clone.
 * @return A pointer to the cloned priority queue, or `nullptr` on failure.
 *
 * @warning The caller is responsible for deallocating the cloned priority queue
 * with `scu_prio_queue_free()` when it is no longer needed.
 *
 * @note This function dynamically allocates memory using `scu_malloc()`.
 */
SCU_API ScuPrioQueue* scu_prio_queue_clone(const ScuPrioQueue* prioQueue);

/**
 * @brief Returns the capacity of a priority queue, i.e., the maximum number of
 * elements that can be stored before a reallocation is required.
 *
 * @param[in] prioQueue The priority queue to examine.
 * @return The capacity of the priority queue.
 *
 * @note The capacity can be influenced to a certain extent using
 * `scu_prio_queue_ensure_capacity()` and `scu_prio_queue_trim_excess()`. See
 * their respective documentation for details.
 */
SCU_API Scuisize scu_prio_queue_capacity(const ScuPrioQueue* prioQueue);

/**
 * @brief Returns the number of elements stored in a priority queue.
 *
 * @param[in] prioQueue The priority queue to examine.
 * @return The number of elements stored in the priority queue.
 */
SCU_API Scuisize scu_prio_queue_count(const ScuPrioQueue* prioQueue);

/**
 * @brief Ensures that a priority queue has at least a specified capacity.
 *
 * @param[in, out] prioQueue The priority queue whose capacity to ensure.
 * @param[in]      capacity  The desired capacity (in number of elements).
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred, or
 * `SCU_ERROR_NONE` on success.
 *
 * @warning This function may invalidate any iterators previously created with
 * `scu_prio_queue_begin()` and `scu_prio_queue_end()`.
 *
 * @note This function dynamically allocates memory using `scu_realloc()`.
 */
SCU_API ScuError scu_prio_queue_ensure_capacity(
    ScuPrioQueue* prioQueue,
    Scuisize capacity
);

/**
 * @brief Enqueues an element with a specified priority into a priority queue.
 *
 * @param[in, out] prioQueue The priority queue to enqueue the element into.
 * @param[in]      elem      The element to enqueue.
 * @param[in]      prio      The priority of the element.
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred, or
 * `SCU_ERROR_NONE` on success.
 *
 * @warning This function invalidates any iterators previously created with
 * `scu_prio_queue_begin()` and `scu_prio_queue_end()`.
 *
 * @note This function dynamically allocates memory using `scu_realloc()`.
 */
SCU_API ScuError scu_prio_queue_enqueue(
    ScuPrioQueue* restrict prioQueue,
    const void* restrict elem,
    const void* restrict prio
);

/**
 * @brief Dequeues the element with the lowest priority from a priority queue.
 *
 * @param[in, out] prioQueue The priority queue to dequeue the element from.
 * @param[out]     elem      The dequeued element.
 * @param[out]     prio      The priority of the dequeued element.
 *
 * @warning This function invalidates any iterators previously created with
 * `scu_prio_queue_begin()` and `scu_prio_queue_end()`.
 *
 * The behavior is undefined if the priority queue is empty. Use
 * `scu_prio_queue_try_dequeue()` to handle this case gracefully.
 */
SCU_API void scu_prio_queue_dequeue(
    ScuPrioQueue* restrict prioQueue,
    void* restrict elem,
    void* restrict prio
);

/**
 * @brief Tries to dequeue the element with the lowest priority from a priority
 * queue.
 *
 * @param[in, out] prioQueue The priority queue to dequeue the element from.
 * @param[out]     elem      The dequeued element on success, unchanged
 *                           otherwise.
 * @param[out]     prio      The priority of the dequeued element on success,
 *                           unchanged otherwise.
 * @return `true` if an element was successfully dequeued, `false` otherwise.
 *
 * @warning This function may invalidate any iterators previously created with
 * `scu_prio_queue_begin()` and `scu_prio_queue_end()`.
 */
SCU_API bool scu_prio_queue_try_dequeue(
    ScuPrioQueue* restrict prioQueue,
    void* restrict elem,
    void* restrict prio
);

/**
 * @brief Returns the element with the lowest priority from a priority queue
 * without removing it.
 *
 * @param[in]  prioQueue The priority queue to examine.
 * @param[out] elem      A pointer to the element with the lowest priority.
 * @param[out] prio      A pointer to the priority of the element with the
 *                       lowest priority.
 *
 * @warning The behavior is undefined if the priority queue is empty. Use
 * `scu_prio_queue_try_peek()` to handle this case gracefully.
 *
 * @note This function is an implementation detail and not intended to be called
 * directly. Use the `scu_prio_queue_peek()` macro instead.
 */
SCU_API void scu_prio_queue_peek_impl(
    const ScuPrioQueue* restrict prioQueue,
    void** restrict elem,
    void** restrict prio
);

/**
 * @brief Returns the element with the lowest priority from a priority queue
 * without removing it.
 *
 * @param[in]  prioQueue The priority queue to examine.
 * @param[out] elem      A pointer to the element with the lowest priority.
 * @param[out] prio      A pointer to the priority of the element with the
 *                       lowest priority.
 *
 * @warning The behavior is undefined if the priority queue is empty. Use
 * `scu_prio_queue_try_peek()` to handle this case gracefully.
 */
#define scu_prio_queue_peek(queue, elem, prio)                        \
    scu_prio_queue_peek_impl(queue, (void**) (elem), (void**) (prio))

/**
 * @brief Tries to return the element with the lowest priority from a priority
 * queue without removing it.
 *
 * @param[in]  prioQueue The priority queue to examine.
 * @param[out] elem      A pointer to the element with the lowest priority on
 *                       success, or `nullptr` on failure.
 * @param[out] prio      A pointer to the priority of the element with the
 *                       lowest priority on success, or `nullptr` on failure.
 * @return `true` if an element was successfully retrieved, `false` otherwise.
 *
 * @note This function is an implementation detail and not intended to be called
 * directly. Use the `scu_prio_queue_try_peek()` macro instead.
 */
SCU_API bool scu_prio_queue_try_peek_impl(
    const ScuPrioQueue* restrict prioQueue,
    void** restrict elem,
    void** restrict prio
);

/**
 * @brief Tries to return the element with the lowest priority from a priority
 * queue without removing it.
 *
 * @param[in]  prioQueue The priority queue to examine.
 * @param[out] elem      A pointer to the element with the lowest priority on
 *                       success, or `nullptr` on failure.
 * @param[out] prio      A pointer to the priority of the element with the
 *                       lowest priority on success, or `nullptr` on failure.
 * @return `true` if an element was successfully retrieved, `false` otherwise.
 */
#define scu_prio_queue_try_peek(queue, elem, prio)                        \
    scu_prio_queue_try_peek_impl(queue, (void**) (elem), (void**) (prio))

/**
 * @brief Removes all elements from a priority queue.
 *
 * @param[in, out] prioQueue The priority queue to clear.
 *
 * @warning This function invalidates any iterators previously created with
 * `scu_prio_queue_begin()` and `scu_prio_queue_end()`.
 *
 * This function does not deallocate the priority queue itself nor the elements
 * contained within, it only resets the number of elements to zero. The caller
 * is responsible for deallocating the individual elements if they are pointers
 * to dynamically allocated objects and no other references to them exist.
 *
 * @note Consider using `scu_prio_queue_trim_excess()` if you wish to reduce the
 * memory usage of the priority queue after clearing it.
 */
SCU_API void scu_prio_queue_clear(ScuPrioQueue* prioQueue);

/**
 * @brief Trims the excess capacity of a priority queue to match its current
 * number of elements.
 *
 * @param[in, out] prioQueue The priority queue to trim.
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred, or
 * `SCU_ERROR_NONE` on success.
 *
 * @warning This function may invalidate any iterators previously created with
 * `scu_prio_queue_begin()` and `scu_prio_queue_end()`.
 *
 * @note This function dynamically allocates memory using `scu_realloc()`.
 */
SCU_API ScuError scu_prio_queue_trim_excess(ScuPrioQueue* prioQueue);

/**
 * @brief Deallocates a priority queue.
 *
 * @param[in] prioQueue The priority queue to deallocate.
 *
 * @warning This function only deallocates the memory occupied by the priority
 * queue itself, but not the elements and priorities contained within. The
 * caller is responsible for deallocating the individual elements and priorities
 * if they are pointers to dynamically allocated objects and no other references
 * to them exist.
 *
 * The behavior is undefined if the priority queue is used after it has been
 * deallocated.
 *
 * @note If `prioQueue` is `nullptr`, this function does nothing.
 */
SCU_API void scu_prio_queue_free(ScuPrioQueue* prioQueue);

/**
 * @brief Returns an iterator to an element of a priority queue.
 *
 * @param[in] prioQueue The priority queue to iterate over.
 * @return An iterator to an element of the priority queue.
 *
 * @note If `prioQueue` is empty, this function behaves identically to
 * `scu_prio_queue_end(prioQueue)`, i.e., it returns the end iterator.
 */
SCU_API ScuPrioQueueIterator scu_prio_queue_begin(
    const ScuPrioQueue* prioQueue
);

/**
 * @brief Returns the end iterator for a priority queue.
 *
 * @param[in] prioQueue The priority queue to iterate over.
 * @return The end iterator for the priority queue.
 */
SCU_API ScuPrioQueueIterator scu_prio_queue_end(const ScuPrioQueue* prioQueue);

/**
 * @brief Determines whether two iterators are equal.
 *
 * @param[in] lhs The first iterator.
 * @param[in] rhs The second iterator.
 * @return `true` if the two iterators are equal, `false` otherwise.
 *
 * @warning The behavior is undefined if `lhs` and `rhs` do not belong to the
 * same priority queue.
 */
SCU_API bool scu_prio_queue_iterator_equal(
    const ScuPrioQueueIterator* lhs,
    const ScuPrioQueueIterator* rhs
);

/**
 * @brief Moves an iterator to the next element.
 *
 * @param[in, out] iterator The iterator to move.
 *
 * @warning The behavior is undefined if moving `iterator` would cause it to
 * point after the end iterator returned by `scu_prio_queue_end()`.
 */
SCU_API void scu_prio_queue_iterator_next(ScuPrioQueueIterator* iterator);

/**
 * @brief Moves an iterator to the previous element.
 *
 * @param[in, out] iterator The iterator to move.
 *
 * @warning The behavior is undefined if moving `iterator` would cause it to
 * point before the begin iterator returned by `scu_prio_queue_begin()`.
 */
SCU_API void scu_prio_queue_iterator_prev(ScuPrioQueueIterator* iterator);

/**
 * @brief Returns an entry for the current element an iterator points to.
 *
 * @param[in] iterator The iterator to dereference.
 * @return An entry for the current element the iterator points to.
 *
 * @warning The behavior is undefined if `iterator` points before the begin
 * iterator returned by `scu_prio_queue_begin()`, if it is the end iterator
 * returned by `scu_prio_queue_end()`, or if it points after the end iterator.
 */
SCU_API ScuPrioQueueEntry scu_prio_queue_iterator_deref(
    const ScuPrioQueueIterator* iterator
);

/**
 * @brief Iterates over each element and its priority in a priority queue.
 *
 * This macro expands to a for loop that iterates over each element and its
 * priority in the priority queue. During each iteration, the provided variable
 * is assigned an entry containing the current element and its priority.
 *
 * The following example demonstrates the basic usage of this macro:
 *
 * ```c
 * // E and P are the types of the elements and priorities, respectively.
 * ScuPrioQueue* prioQueue = scu_prio_queue_new(
 *     SCU_SIZEOF(E),
 *     SCU_SIZEOF(P),
 *     ...
 * );
 * ...
 * ScuPrioQueueEntry entry;
 * SCU_PRIO_QUEUE_FOREACH(entry, prioQueue) {
 *     // Do something with the element and its priority.
 *     E* elem = entry.elem;
 *     P* prio = entry.prio;
 * }
 * ```
 *
 * @param[out] entry     An entry containing the current element and its
 *                       priority.
 * @param[in]  prioQueue The priority queue to iterate over.
 *
 * @warning The behavior is undefined if the priority queue is modified (e.g.,
 * elements are enqueued or dequeued) while being iterated over.
 *
 * @note The variable `entry` must be declared manually before the loop. It must
 * be of type `ScuPrioQueueEntry`.
 *
 * Note that the order of iteration is explicitly unspecified and may not
 * necessarily correspond to the order of priorities. In particular, it is not
 * guaranteed that elements with lower priorities are returned before elements
 * with higher priorities.
 */
#define SCU_PRIO_QUEUE_FOREACH(entry, prioQueue)                                          \
    for (                                                                                 \
        ScuPrioQueueIterator SCU_XCONCAT(it, __LINE__) = scu_prio_queue_begin(prioQueue), \
        SCU_XCONCAT(end, __LINE__) = scu_prio_queue_end(prioQueue);                       \
        !scu_prio_queue_iterator_equal(                                                   \
            &SCU_XCONCAT(it, __LINE__),                                                   \
            &SCU_XCONCAT(end, __LINE__)                                                   \
        )                                                                                 \
            && (                                                                          \
                (entry) = scu_prio_queue_iterator_deref(&SCU_XCONCAT(it, __LINE__)),      \
                true                                                                      \
            );                                                                            \
        scu_prio_queue_iterator_next(&SCU_XCONCAT(it, __LINE__))                          \
    )

#endif
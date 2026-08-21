#ifndef SCU_PRIO_QUEUE_H
#define SCU_PRIO_QUEUE_H

#include "scu/common.h"
#include "scu/compare.h"
#include "scu/error.h"
#include "scu/types.h"

/** @brief Represents a collection of elements and associated priorities. */
typedef struct ScuPrioQueue ScuPrioQueue;

/**
 * @brief Represents an iterator for a priority queue.
 *
 * @warning The internal representation of the iterator is an implementation
 * detail and should not be relied upon. Most importantly, the behavior is
 * undefined if its fields are accessed directly.
 */
typedef struct ScuPrioQueueIter {

    /** @brief The priority queue being iterated over. */
    ScuPrioQueue* prioQueue;

    /** @brief The current index within the priority queue. */
    Scuisize index;

} ScuPrioQueueIter;

/** @brief Represents an entry in a priority queue. */
typedef struct ScuPrioQueueEntry {

    /** @brief The element of the entry. */
    void* elem;

    /** @brief The priority of the entry. */
    void* prio;

} ScuPrioQueueEntry;

/**
 * @brief Allocates and initializes a new priority queue with specified element
 * size, priority size, priority comparison function, and an unspecified default
 * capacity.
 *
 * @note This function dynamically allocates memory using `scu_malloc()`.
 *
 * @warning The caller is responsible for deallocating the priority queue with
 * `scu_prio_queue_free()` when it is no longer needed.
 *
 * @param[in] elemSize    The size of each element (in bytes).
 * @param[in] prioSize    The size of each priority (in bytes).
 * @param[in] prioCmpFunc A function used for comparing priorities.
 * @return A pointer to the new priority queue, or `nullptr` on failure.
 */
[[nodiscard]]
ScuPrioQueue* scu_prio_queue_new(
    Scuisize elemSize,
    Scuisize prioSize,
    ScuCompareFunc* prioCmpFunc
);

/**
 * @brief Allocates and initializes a new priority queue with specified element
 * size, priority size, priority comparison function, and initial capacity.
 *
 * @note This function dynamically allocates memory using `scu_malloc()`.
 *
 * @warning The caller is responsible for deallocating the priority queue with
 * `scu_prio_queue_free()` when it is no longer needed.
 *
 * @param[in] elemSize    The size of each element (in bytes).
 * @param[in] prioSize    The size of each priority (in bytes).
 * @param[in] capacity    The initial capacity (in number of elements).
 * @param[in] prioCmpFunc A function used for comparing priorities.
 * @return A pointer to the new priority queue, or `nullptr` on failure.
 */
[[nodiscard]]
ScuPrioQueue* scu_prio_queue_new_with_capacity(
    Scuisize elemSize,
    Scuisize prioSize,
    Scuisize capacity,
    ScuCompareFunc* prioCmpFunc
);

/**
 * @brief Creates a shallow copy of a specified priority queue.
 *
 * @note This function dynamically allocates memory using `scu_malloc()`.
 *
 * @warning The caller is responsible for deallocating the cloned priority queue
 * with `scu_prio_queue_free()` when it is no longer needed.
 *
 * @param[in] prioQueue The priority queue to clone.
 * @return A pointer to the cloned priority queue, or `nullptr` on failure.
 */
[[nodiscard]]
ScuPrioQueue* scu_prio_queue_clone(const ScuPrioQueue* prioQueue);

/**
 * @brief Returns the capacity of a specified priority queue, i.e., the maximum
 * number of elements that can be stored before a reallocation is required.
 *
 * @note The capacity can be influenced to a certain extent using
 * `scu_prio_queue_ensure_capacity()` and `scu_prio_queue_trim_excess()`. See
 * their respective documentation for details.
 *
 * @param[in] prioQueue The priority queue to examine.
 * @return The capacity of the specified priority queue.
 */
Scuisize scu_prio_queue_capacity(const ScuPrioQueue* prioQueue);

/**
 * @brief Returns the number of elements stored in a specified priority queue.
 *
 * @param[in] prioQueue The priority queue to examine.
 * @return The number of elements stored in the specified priority queue.
 */
Scuisize scu_prio_queue_count(const ScuPrioQueue* prioQueue);

/**
 * @brief Ensures that a specified priority queue has at least a specified
 * capacity.
 *
 * @note This function dynamically allocates memory using `scu_realloc()`.
 *
 * @param[in, out] prioQueue The priority queue to ensure the capacity of.
 * @param[in]      capacity  The desired capacity (in number of elements).
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred, or
 * `SCU_ERROR_NONE` on success.
 */
ScuError scu_prio_queue_ensure_capacity(
    ScuPrioQueue* prioQueue,
    Scuisize capacity
);

/**
 * @brief Enqueues a new element with a specified priority into a specified
 * priority queue.
 *
 * @note This function dynamically allocates memory using `scu_realloc()`.
 *
 * @param[in, out] prioQueue The priority queue to enqueue the element into.
 * @param[in]      elem      The element to enqueue.
 * @param[in]      prio      The priority of the element.
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred, or
 * `SCU_ERROR_NONE` on success.
 */
ScuError scu_prio_queue_enqueue(
    ScuPrioQueue* restrict prioQueue,
    const void* restrict elem,
    const void* restrict prio
);

/**
 * @brief Dequeues the element with the lowest priority from a specified
 * priority queue.
 *
 * @warning The behavior is undefined if the priority queue is empty. Use
 * `scu_prio_queue_try_dequeue()` to handle this case gracefully.
 *
 * @param[in, out] prioQueue The priority queue to dequeue the element from.
 * @param[out]     elem      The dequeued element.
 * @param[out]     prio      The priority of the dequeued element.
 */
void scu_prio_queue_dequeue(
    ScuPrioQueue* restrict prioQueue,
    void* restrict elem,
    void* restrict prio
);

/**
 * @brief Tries to dequeue the element with the lowest priority from a specified
 * priority queue.
 *
 * @param[in, out] prioQueue The priority queue to dequeue the element from.
 * @param[out]     elem      The dequeued element on success, otherwise
 *                           unchanged.
 * @param[out]     prio      The priority of the dequeued element on success,
 *                           otherwise unchanged.
 * @return `true` if an element was successfully dequeued, otherwise `false`.
 */
bool scu_prio_queue_try_dequeue(
    ScuPrioQueue* restrict prioQueue,
    void* restrict elem,
    void* restrict prio
);

/**
 * @brief Returns the element with the lowest priority from a specified priority
 * queue without removing it.
 *
 * @note This function is an implementation detail and not intended to be called
 * directly. Use the `scu_prio_queue_peek()` macro instead.
 *
 * @warning The behavior is undefined if the priority queue is empty. Use
 * `scu_prio_queue_try_peek()` to handle this case gracefully.
 *
 * @param[in]  prioQueue The priority queue to examine.
 * @param[out] elem      A pointer to the element with the lowest priority.
 * @param[out] prio      A pointer to the priority of the element with the
 *                       lowest priority.
 */
void scu_prio_queue_peek_impl(
    const ScuPrioQueue* restrict prioQueue,
    void** restrict elem,
    void** restrict prio
);

/**
 * @brief Returns the element with the lowest priority from a specified priority
 * queue without removing it.
 *
 * @warning The behavior is undefined if the priority queue is empty. Use
 * `scu_prio_queue_try_peek()` to handle this case gracefully.
 *
 * @param[in]  prioQueue The priority queue to examine.
 * @param[out] elem      A pointer to the element with the lowest priority.
 * @param[out] prio      A pointer to the priority of the element with the
 *                       lowest priority.
 */
#define scu_prio_queue_peek(queue, elem, prio)                        \
    scu_prio_queue_peek_impl(queue, (void**) (elem), (void**) (prio))

/**
 * @brief Tries to return the element with the lowest priority from a specified
 * priority queue without removing it.
 *
 * @note This function is an implementation detail and not intended to be called
 * directly. Use the `scu_prio_queue_try_peek()` macro instead.
 *
 * @param[in]  prioQueue The priority queue to examine.
 * @param[out] elem      A pointer to the element with the lowest priority on
 *                       success, otherwise a `nullptr`.
 * @param[out] prio      A pointer to the priority of the element with the
 *                       lowest priority on success, otherwise a `nullptr`.
 * @return `true` if an element was successfully retrieved, otherwise `false`.
 */
bool scu_prio_queue_try_peek_impl(
    const ScuPrioQueue* restrict prioQueue,
    void** restrict elem,
    void** restrict prio
);

/**
 * @brief Tries to return the element with the lowest priority from a specified
 * priority queue without removing it.
 *
 * @param[in]  prioQueue The priority queue to examine.
 * @param[out] elem      A pointer to the element with the lowest priority on
 *                       success, otherwise a `nullptr`.
 * @param[out] prio      A pointer to the priority of the element with the
 *                       lowest priority on success, otherwise a `nullptr`.
 * @return `true` if an element was successfully retrieved, otherwise `false`.
 */
#define scu_prio_queue_try_peek(queue, elem, prio)                        \
    scu_prio_queue_try_peek_impl(queue, (void**) (elem), (void**) (prio))

/**
 * @brief Removes all elements from a specified priority queue.
 *
 * @note Consider using `scu_prio_queue_trim_excess()` if you wish to reduce the
 * memory usage of the priority queue after clearing it.
 *
 * @warning This function does not deallocate the priority queue itself nor the
 * elements contained within, it only resets the number of elements to zero. The
 * caller is responsible for deallocating the individual elements if they are
 * pointers to dynamically allocated objects and no other references to them
 * exist.
 *
 * @param[in, out] prioQueue The priority queue to clear.
 */
void scu_prio_queue_clear(ScuPrioQueue* prioQueue);

/**
 * @brief Trims the excess capacity of a specified priority queue to match its
 * current number of elements.
 *
 * @note This function dynamically allocates memory using `scu_realloc()`.
 *
 * @param[in, out] prioQueue The priority queue to trim.
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred, or
 * `SCU_ERROR_NONE` on success.
 */
ScuError scu_prio_queue_trim_excess(ScuPrioQueue* prioQueue);

/**
 * @brief Returns an iterator for a specified priority queue.
 *
 * @note The iterator is initially positioned before the first element of the
 * priority queue (if any). This means that `scu_prio_queue_iter_move_next()`
 * must be called before accessing the first and subsequent elements with
 * `scu_prio_queue_iter_current()`.
 *
 * Note that the order of iteration is explicitly unspecified and may not
 * necessarily correspond to the order of priorities. In particular, it is not
 * guaranteed that elements with lower priorities are returned before elements
 * with higher priorities.
 *
 * @warning The behavior is undefined if the priority queue being iterated over
 * is modified (e.g., elements are enqueued or dequeued) while the iterator is
 * in use.
 *
 * @param[in] prioQueue The priority queue to iterate over.
 * @return An iterator for the specified priority queue.
 */
ScuPrioQueueIter scu_prio_queue_iter(const ScuPrioQueue* prioQueue);

/**
 * @brief Advances a specified priority queue iterator to the next element.
 *
 * @param[in, out] iter The iterator to advance.
 * @return `true` if the iterator was successfully advanced to the next element,
 * otherwise `false` (i.e., the priority queue does not contain any more
 * elements).
 */
bool scu_prio_queue_iter_move_next(ScuPrioQueueIter* iter);

/**
 * @brief Returns the current element and its priority of a specified priority
 * queue iterator.
 *
 * @param[in] iter The iterator to examine.
 * @return An entry containing the current element and its priority.
 */
ScuPrioQueueEntry scu_prio_queue_iter_current(const ScuPrioQueueIter* iter);

/**
 * @brief Resets a specified priority queue iterator to its initial position.
 *
 * @note The iterator is initially positioned before the first element of the
 * priority queue (if any). This means that `scu_prio_queue_iter_move_next()`
 * must be called before accessing the first and subsequent elements with
 * `scu_prio_queue_iter_current()`.
 *
 * @param[in, out] iter The iterator to reset.
 */
void scu_prio_queue_iter_reset(ScuPrioQueueIter* iter);

/**
 * @brief Deallocates a specified priority queue.
 *
 * @note If `prioQueue` is a `nullptr`, this function does nothing.
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
 * @param[in] prioQueue The priority queue to deallocate.
 */
void scu_prio_queue_free(ScuPrioQueue* prioQueue);

/**
 * @brief Iterates over each element and its priority in a specified priority
 * queue.
 *
 * This macro expands to a for loop that iterates over each element and its
 * priority in the specified priority queue. During each iteration, the provided
 * variable is assigned an entry containing the current element and its
 * priority.
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
 * @note The variable `entry` must be declared manually before the loop. It must
 * be of type `ScuPrioQueueEntry`.
 *
 * Note that the order of iteration is explicitly unspecified and may not
 * necessarily correspond to the order of priorities. In particular, it is not
 * guaranteed that elements with lower priorities are returned before elements
 * with higher priorities.
 *
 * @warning The behavior is undefined if the priority queue is modified (e.g.,
 * elements are enqueued or dequeued) while being iterated over.
 *
 * @param[out] entry     An entry containing the current element and its
 *                       priority.
 * @param[in]  prioQueue The priority queue to iterate over.
 */
#define SCU_PRIO_QUEUE_FOREACH(entry, prioQueue)                                     \
    for (                                                                            \
        ScuPrioQueueIter SCU_XCONCAT(it, __LINE__) = scu_prio_queue_iter(prioQueue); \
        scu_prio_queue_iter_move_next(&SCU_XCONCAT(it, __LINE__))                    \
            && (                                                                     \
                (entry) = scu_prio_queue_iter_current(&SCU_XCONCAT(it, __LINE__)),   \
                true                                                                 \
            );                                                                       \
    )

#endif
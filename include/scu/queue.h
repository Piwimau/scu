#ifndef SCU_QUEUE_H
#define SCU_QUEUE_H

#include "scu/common.h"
#include "scu/error.h"
#include "scu/types.h"

/** @brief A first-in-first-out (FIFO) collection of elements. */
typedef struct ScuQueue ScuQueue;

/**
 * @brief An iterator for a queue.
 *
 * @warning The internal representation of the iterator is an implementation
 * detail and should not be relied upon. Most importantly, the behavior is
 * undefined if its fields are accessed directly.
 */
typedef struct ScuQueueIter {

    /** @brief The queue being iterated over. */
    ScuQueue* queue;

    /** @brief The current index within the queue. */
    Scuisize index;

} ScuQueueIter;

/**
 * @brief Constructs a queue with a specified element size and an unspecified
 * default capacity.
 *
 * @param[in] elemSize The size of each element (in bytes).
 * @return A pointer to the queue, or `nullptr` on failure.
 *
 * @warning The caller is responsible for deallocating the queue with
 * `scu_queue_free()` when it is no longer needed.
 *
 * @note This function dynamically allocates memory using `scu_malloc()`.
 */
ScuQueue* scu_queue_new(Scuisize elemSize);

/**
 * @brief Constructs a queue with a specified element size and initial capacity.
 *
 * @param[in] elemSize The size of each element (in bytes).
 * @param[in] capacity The initial capacity (in number of elements).
 * @return A pointer to the queue, or `nullptr` on failure.
 *
 * @warning The caller is responsible for deallocating the queue with
 * `scu_queue_free()` when it is no longer needed.
 *
 * @note This function dynamically allocates memory using `scu_malloc()`.
 */
ScuQueue* scu_queue_new_with_capacity(Scuisize elemSize, Scuisize capacity);

/**
 * @brief Creates a shallow copy of a queue.
 *
 * @param[in] queue The queue to clone.
 * @return A pointer to the cloned queue, or `nullptr` on failure.
 *
 * @warning The caller is responsible for deallocating the cloned queue with
 * `scu_queue_free()` when it is no longer needed.
 *
 * @note This function dynamically allocates memory using `scu_malloc()`.
 */
ScuQueue* scu_queue_clone(const ScuQueue* queue);

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
Scuisize scu_queue_capacity(const ScuQueue* queue);

/**
 * @brief Returns the number of elements stored in a queue.
 *
 * @param[in] queue The queue to examine.
 * @return The number of elements stored in the queue.
 */
Scuisize scu_queue_count(const ScuQueue* queue);

/**
 * @brief Ensures that a queue has at least a specified capacity.
 *
 * @param[in, out] queue    The queue whose capacity to ensure.
 * @param[in]      capacity The desired capacity (in number of elements).
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred, or
 * `SCU_ERROR_NONE` on success.
 *
 * @note This function dynamically allocates memory using `scu_malloc()`.
 */
ScuError scu_queue_ensure_capacity(ScuQueue* queue, Scuisize capacity);

/**
 * @brief Enqueues an element into a queue.
 *
 * @param[in, out] queue The queue to enqueue the element into.
 * @param[in]      elem  The element to enqueue.
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred, or
 * `SCU_ERROR_NONE` on success.
 *
 * @note This function dynamically allocates memory using `scu_malloc()`.
 */
ScuError scu_queue_enqueue(ScuQueue* restrict queue, const void* restrict elem);

/**
 * @brief Dequeues the element at the front of a queue.
 *
 * @param[in, out] queue The queue to dequeue the element from.
 * @param[out]     elem  The dequeued element.
 *
 * @warning The behavior is undefined if the queue is empty. Use
 * `scu_queue_try_dequeue()` to handle this case gracefully.
 */
void scu_queue_dequeue(ScuQueue* restrict queue, void* restrict elem);

/**
 * @brief Tries to dequeue the element at the front of a queue.
 *
 * @param[in, out] queue The queue to dequeue the element from.
 * @param[out]     elem  The dequeued element on success, unchanged otherwise.
 * @return `true` if an element was successfully dequeued, `false` otherwise.
 */
bool scu_queue_try_dequeue(ScuQueue* restrict queue, void* restrict elem);

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
void scu_queue_peek_impl(const ScuQueue* restrict queue, void** restrict elem);

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
bool scu_queue_try_peek_impl(
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
 * @warning This function does not deallocate the queue itself nor the elements
 * contained within, it only resets the number of elements to zero. The caller
 * is responsible for deallocating the individual elements if they are pointers
 * to dynamically allocated objects and no other references to them exist.
 *
 * @note Consider using `scu_queue_trim_excess()` if you wish to reduce the
 * memory usage of the queue after clearing it.
 */
void scu_queue_clear(ScuQueue* queue);

/**
 * @brief Trims the excess capacity of a queue to match its current number of
 * elements.
 *
 * @param[in, out] queue The queue to trim.
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred, or
 * `SCU_ERROR_NONE` on success.
 *
 * @note This function dynamically allocates memory using `scu_malloc()`.
 */
ScuError scu_queue_trim_excess(ScuQueue* queue);

/**
 * @brief Returns an iterator for a queue.
 *
 * @param[in] queue The queue to iterate over.
 * @return An iterator for the queue.
 *
 * @warning The behavior is undefined if the queue is modified (e.g., elements
 * are enqueued or dequeued) while the iterator is in use.
 *
 * @note The iterator is initially positioned before the first element of the
 * queue (if any). This means that `scu_queue_iter_move_next()` must be called
 * before accessing the first and subsequent elements with
 * `scu_queue_iter_current()`.
 *
 * The elements are returned in the order they would be dequeued, i.e., starting
 * from the front of the queue all the way to the back.
 */
ScuQueueIter scu_queue_iter(const ScuQueue* queue);

/**
 * @brief Advances a queue iterator to the next element.
 *
 * @param[in, out] iter The iterator to advance.
 * @return `true` if the iterator was successfully advanced to the next element,
 * `false` otherwise (i.e., the queue does not contain any more elements).
 */
bool scu_queue_iter_move_next(ScuQueueIter* iter);

/**
 * @brief Returns the current element of a queue iterator.
 *
 * @param[in] iter The iterator to examine.
 * @return The current element of the iterator.
 */
void* scu_queue_iter_current(const ScuQueueIter* iter);

/**
 * @brief Resets a queue iterator to its initial position.
 *
 * @param[in, out] iter The iterator to reset.
 *
 * @note The iterator is initially positioned before the first element of the
 * queue (if any). This means that `scu_queue_iter_move_next()` must be called
 * before accessing the first and subsequent elements with
 * `scu_queue_iter_current()`.
 */
void scu_queue_iter_reset(ScuQueueIter* iter);

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
void scu_queue_free(ScuQueue* queue);

/**
 * @brief Iterates over each element in a queue.
 *
 * This macro expands to a for loop that iterates over each element in the
 * queue. During each iteration, the provided variable is assigned a pointer to
 * the current element.
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
#define SCU_QUEUE_FOREACH(elem, queue)                                       \
    for (                                                                    \
        ScuQueueIter SCU_XCONCAT(it, __LINE__) = scu_queue_iter(queue);      \
        scu_queue_iter_move_next(&SCU_XCONCAT(it, __LINE__))                 \
            && (                                                             \
                (elem) = scu_queue_iter_current(&SCU_XCONCAT(it, __LINE__)), \
                true                                                         \
            );                                                               \
    )

#endif
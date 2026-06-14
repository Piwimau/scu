#define SCU_SHORT_ALIASES

#include "scu/alloc.h"
#include "scu/assert.h"
#include "scu/math.h"
#include "scu/memory.h"
#include "scu/queue.h"

struct ScuQueue {

    /** @brief The size of each element (in bytes). */
    isize elemSize;

    /** @brief The maximum number of elements that can be stored. */
    isize capacity;

    /** @brief The current number of elements. */
    isize count;

    /** @brief The index of the next element to dequeue. */
    isize head;

    /** @brief The index at which the next element will be enqueued. */
    isize tail;

    /**
     * @brief The elements stored in the queue.
     *
     * @note This is a dynamically allocated array of `capacity` elements, or
     * `nullptr` if `capacity` is zero.
     */
    byte* elems;

};

/** @brief The default capacity of a queue. */
static constexpr isize SCU_DEFAULT_CAPACITY = 8;

/** @brief The growth factor for increasing the capacity of a queue. */
static constexpr isize SCU_GROWTH_FACTOR = 2;

[[nodiscard]]
ScuQueue* scu_queue_new(isize elemSize) {
    return scu_queue_new_with_capacity(elemSize, SCU_DEFAULT_CAPACITY);
}

[[nodiscard]]
ScuQueue* scu_queue_new_with_capacity(isize elemSize, isize capacity) {
    SCU_ASSERT(elemSize > 0);
    SCU_ASSERT(capacity >= 0);
    ScuQueue* queue = scu_malloc(SCU_SIZEOF(ScuQueue));
    if (queue == nullptr) {
        return nullptr;
    }
    queue->elemSize = elemSize;
    queue->capacity = capacity;
    queue->count = 0;
    queue->head = 0;
    queue->tail = 0;
    if (capacity > 0) {
        queue->elems = scu_malloc(elemSize * capacity);
        if (queue->elems == nullptr) {
            scu_free(queue);
            return nullptr;
        }
    }
    else {
        queue->elems = nullptr;
    }
    return queue;
}

[[nodiscard]]
ScuQueue* scu_queue_clone(const ScuQueue* queue) {
    SCU_ASSERT(queue != nullptr);
    ScuQueue* clone = scu_malloc(SCU_SIZEOF(ScuQueue));
    if (clone == nullptr) {
        return nullptr;
    }
    clone->elemSize = queue->elemSize;
    clone->capacity = queue->capacity;
    clone->count = queue->count;
    if (clone->capacity > 0) {
        clone->elems = scu_malloc(clone->elemSize * clone->capacity);
        if (clone->elems == nullptr) {
            scu_free(clone);
            return nullptr;
        }
        if (queue->count > 0) {
            isize firstChunk = SCU_MIN(
                queue->capacity - queue->head,
                queue->count
            );
            scu_memcpy(
                clone->elems,
                queue->elems + queue->head * queue->elemSize,
                firstChunk * queue->elemSize
            );
            isize secondChunk = queue->count - firstChunk;
            scu_memcpy(
                clone->elems + firstChunk * queue->elemSize,
                queue->elems,
                secondChunk * queue->elemSize
            );
        }
        clone->head = 0;
        clone->tail = queue->count;
    }
    else {
        clone->elems = nullptr;
        clone->head = 0;
        clone->tail = 0;
    }
    return clone;
}

isize scu_queue_capacity(const ScuQueue* queue) {
    SCU_ASSERT(queue != nullptr);
    return queue->capacity;
}

isize scu_queue_count(const ScuQueue* queue) {
    SCU_ASSERT(queue != nullptr);
    return queue->count;
}

ScuError scu_queue_ensure_capacity(ScuQueue* queue, isize capacity) {
    SCU_ASSERT(queue != nullptr);
    SCU_ASSERT(capacity >= 0);
    if (queue->capacity < capacity) {
        isize newCapacity = (queue->capacity > 0) ? queue->capacity : 1;
        while (newCapacity < capacity) {
            newCapacity *= SCU_GROWTH_FACTOR;
        }
        byte* newElems = scu_malloc(queue->elemSize * newCapacity);
        if (newElems == nullptr) {
            return SCU_ERROR_OUT_OF_MEMORY;
        }
        if (queue->count > 0) {
            isize firstChunk = SCU_MIN(
                queue->capacity - queue->head,
                queue->count
            );
            scu_memcpy(
                newElems,
                queue->elems + queue->head * queue->elemSize,
                firstChunk * queue->elemSize
            );
            isize secondChunk = queue->count - firstChunk;
            scu_memcpy(
                newElems + firstChunk * queue->elemSize,
                queue->elems,
                secondChunk * queue->elemSize
            );
        }
        scu_free(queue->elems);
        queue->elems = newElems;
        queue->capacity = newCapacity;
        queue->head = 0;
        queue->tail = queue->count;
    }
    return SCU_ERROR_NONE;
}

ScuError scu_queue_enqueue(
    ScuQueue* restrict queue,
    const void* restrict elem
) {
    SCU_ASSERT(queue != nullptr);
    SCU_ASSERT(elem != nullptr);
    ScuError error = scu_queue_ensure_capacity(queue, queue->count + 1);
    if (error != SCU_ERROR_NONE) {
        return error;
    }
    scu_memcpy(
        queue->elems + queue->tail * queue->elemSize,
        elem,
        queue->elemSize
    );
    queue->tail = (queue->tail + 1) % queue->capacity;
    queue->count++;
    return SCU_ERROR_NONE;
}

void scu_queue_dequeue(ScuQueue* restrict queue, void* restrict elem) {
    if (!scu_queue_try_dequeue(queue, elem)) {
        SCU_FATAL("The specified queue is empty.\n");
    }
}

bool scu_queue_try_dequeue(ScuQueue* restrict queue, void* restrict elem) {
    SCU_ASSERT(queue != nullptr);
    SCU_ASSERT(elem != nullptr);
    if (queue->count == 0) {
        return false;
    }
    scu_memcpy(
        elem,
        queue->elems + queue->head * queue->elemSize,
        queue->elemSize
    );
    queue->head = (queue->head + 1) % queue->capacity;
    queue->count--;
    return true;
}

void scu_queue_peek_impl(const ScuQueue* restrict queue, void** restrict elem) {
    if (!scu_queue_try_peek_impl(queue, elem)) {
        SCU_FATAL("The specified queue is empty.\n");
    }
}

bool scu_queue_try_peek_impl(
    const ScuQueue* restrict queue,
    void** restrict elem
) {
    SCU_ASSERT(queue != nullptr);
    SCU_ASSERT(elem != nullptr);
    if (queue->count == 0) {
        *elem = nullptr;
        return false;
    }
    *elem = queue->elems + queue->head * queue->elemSize;
    return true;
}

void scu_queue_clear(ScuQueue* queue) {
    SCU_ASSERT(queue != nullptr);
    queue->count = 0;
    queue->head = 0;
    queue->tail = 0;
}

ScuError scu_queue_trim_excess(ScuQueue* queue) {
    SCU_ASSERT(queue != nullptr);
    if (queue->capacity > queue->count) {
        if (queue->count == 0) {
            scu_free(queue->elems);
            queue->elems = nullptr;
            queue->capacity = 0;
            queue->head = 0;
            queue->tail = 0;
        }
        else {
            isize newCapacity = queue->count;
            byte* newElems = scu_malloc(queue->elemSize * newCapacity);
            if (newElems == nullptr) {
                return SCU_ERROR_OUT_OF_MEMORY;
            }
            isize firstChunk = SCU_MIN(
                queue->capacity - queue->head,
                queue->count
            );
            scu_memcpy(
                newElems,
                queue->elems + queue->head * queue->elemSize,
                firstChunk * queue->elemSize
            );
            isize secondChunk = queue->count - firstChunk;
            scu_memcpy(
                newElems + firstChunk * queue->elemSize,
                queue->elems,
                secondChunk * queue->elemSize
            );
            scu_free(queue->elems);
            queue->elems = newElems;
            queue->capacity = newCapacity;
            queue->head = 0;
            queue->tail = queue->count;
        }
    }
    return SCU_ERROR_NONE;
}

ScuQueueIter scu_queue_iter(const ScuQueue* queue) {
    SCU_ASSERT(queue != nullptr);
    return (ScuQueueIter) {
        .queue = SCU_CONST_CAST(ScuQueue*, queue),
        .index = -1
    };
}

bool scu_queue_iter_move_next(ScuQueueIter* iter) {
    SCU_ASSERT(iter != nullptr);
    ScuQueue* queue = iter->queue;
    isize index = iter->index;
    SCU_ASSERT(queue != nullptr);
    SCU_ASSERT((index >= -1) && (index < queue->count));
    if ((index + 1) < queue->count) {
        iter->index++;
        return true;
    }
    return false;
}

void* scu_queue_iter_current(const ScuQueueIter* iter) {
    SCU_ASSERT(iter != nullptr);
    ScuQueue* queue = iter->queue;
    isize index = iter->index;
    SCU_ASSERT(queue != nullptr);
    SCU_ASSERT((index >= 0) && (index < queue->count));
    isize actualIndex = (queue->head + index) % queue->capacity;
    return queue->elems + actualIndex * queue->elemSize;
}

void scu_queue_iter_reset(ScuQueueIter* iter) {
    SCU_ASSERT(iter != nullptr);
    SCU_ASSERT(iter->queue != nullptr);
    SCU_ASSERT((iter->index >= -1) && (iter->index < iter->queue->count));
    iter->index = -1;
}

void scu_queue_free(ScuQueue* queue) {
    if (queue != nullptr) {
        scu_free(queue->elems);
        queue->elems = nullptr;
        queue->capacity = 0;
        queue->count = 0;
        queue->head = 0;
        queue->tail = 0;
        scu_free(queue);
    }
}
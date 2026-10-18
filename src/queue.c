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

ScuQueue* scu_queue_new(isize elemSize) {
    return scu_queue_new_with_capacity(elemSize, SCU_DEFAULT_CAPACITY);
}

ScuQueue* scu_queue_new_with_capacity(isize elemSize, isize capacity) {
    SCU_ASSERT(elemSize > 0);
    SCU_ASSERT(capacity >= 0);
    ScuQueue* queue = scu_malloc(SCU_SIZEOF(ScuQueue));
    if (!queue) {
        return nullptr;
    }
    queue->elemSize = elemSize;
    queue->capacity = capacity;
    queue->count = 0;
    queue->head = 0;
    queue->tail = 0;
    if (capacity > 0) {
        queue->elems = scu_malloc(elemSize * capacity);
        if (!queue->elems) {
            scu_free(queue);
            return nullptr;
        }
    }
    else {
        queue->elems = nullptr;
    }
    return queue;
}

ScuQueue* scu_queue_clone(const ScuQueue* queue) {
    SCU_ASSERT(queue);
    ScuQueue* clone = scu_malloc(SCU_SIZEOF(ScuQueue));
    if (!clone) {
        return nullptr;
    }
    clone->elemSize = queue->elemSize;
    clone->capacity = queue->capacity;
    clone->count = queue->count;
    if (clone->capacity > 0) {
        clone->elems = scu_malloc(clone->elemSize * clone->capacity);
        if (!clone->elems) {
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
        clone->tail = queue->count % clone->capacity;
    }
    else {
        clone->elems = nullptr;
        clone->head = 0;
        clone->tail = 0;
    }
    return clone;
}

isize scu_queue_capacity(const ScuQueue* queue) {
    SCU_ASSERT(queue);
    return queue->capacity;
}

isize scu_queue_count(const ScuQueue* queue) {
    SCU_ASSERT(queue);
    return queue->count;
}

ScuError scu_queue_ensure_capacity(ScuQueue* queue, isize capacity) {
    SCU_ASSERT(queue);
    SCU_ASSERT(capacity >= 0);
    if (queue->capacity < capacity) {
        isize newCapacity = (queue->capacity > 0) ? queue->capacity : 1;
        while (newCapacity < capacity) {
            newCapacity *= SCU_GROWTH_FACTOR;
        }
        byte* newElems = scu_malloc(queue->elemSize * newCapacity);
        if (!newElems) {
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
    SCU_ASSERT(queue);
    SCU_ASSERT(elem);
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
    SCU_ASSERT(queue);
    SCU_ASSERT(elem);
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
    SCU_ASSERT(queue);
    SCU_ASSERT(elem);
    if (queue->count == 0) {
        *elem = nullptr;
        return false;
    }
    *elem = queue->elems + queue->head * queue->elemSize;
    return true;
}

void scu_queue_clear(ScuQueue* queue) {
    SCU_ASSERT(queue);
    queue->count = 0;
    queue->head = 0;
    queue->tail = 0;
}

ScuError scu_queue_trim_excess(ScuQueue* queue) {
    SCU_ASSERT(queue);
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
            if (!newElems) {
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
            queue->tail = queue->count % queue->capacity;
        }
    }
    return SCU_ERROR_NONE;
}

void scu_queue_free(ScuQueue* queue) {
    if (!queue) {
        return;
    }
    scu_free(queue->elems);
    queue->elems = nullptr;
    queue->capacity = 0;
    queue->count = 0;
    queue->head = 0;
    queue->tail = 0;
    scu_free(queue);
}

ScuQueueIterator scu_queue_begin(const ScuQueue* queue) {
    SCU_ASSERT(queue);
    return (ScuQueueIterator) {
        .queue = SCU_CONST_CAST(ScuQueue*, queue),
        .index = 0
    };
}

ScuQueueIterator scu_queue_end(const ScuQueue* queue) {
    SCU_ASSERT(queue);
    return (ScuQueueIterator) {
        .queue = SCU_CONST_CAST(ScuQueue*, queue),
        .index = queue->count
    };
}

bool scu_queue_iterator_equal(
    const ScuQueueIterator* lhs,
    const ScuQueueIterator* rhs
) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    SCU_ASSERT(lhs->queue == rhs->queue);
    return lhs->index == rhs->index;
}

void scu_queue_iterator_next(ScuQueueIterator* iterator) {
    SCU_ASSERT(iterator);
    SCU_ASSERT(iterator->index < iterator->queue->count);
    iterator->index++;
}

void scu_queue_iterator_prev(ScuQueueIterator* iterator) {
    SCU_ASSERT(iterator);
    SCU_ASSERT(iterator->index > 0);
    iterator->index--;
}

void* scu_queue_iterator_deref(const ScuQueueIterator* iterator) {
    SCU_ASSERT(iterator);
    const ScuQueue* queue = iterator->queue;
    isize index = iterator->index;
    SCU_ASSERT((index >= 0) && (index < queue->count));
    index = (queue->head + index) % queue->capacity;
    return queue->elems + queue->elemSize * index;
}
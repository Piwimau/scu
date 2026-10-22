#define SCU_SHORT_ALIASES

#include <stddef.h>
#include "scu/assert.h"
#include "scu/math.h"
#include "scu/prio-queue.h"

struct ScuPrioQueue {

    /** @brief The allocator used for memory management. */
    ScuAllocator* allocator;

    /** @brief The size of each element (in bytes). */
    isize elemSize;

    /** @brief The size of each priority (in bytes). */
    isize prioSize;

    /** @brief The offset of the element within a node (in bytes). */
    isize elemOffset;

    /** @brief The effective size of a node (in bytes). */
    isize nodeSize;

    /** @brief The alignment of a node (in bytes). */
    isize nodeAlign;

    /** @brief The maximum number of elements that can be stored. */
    isize capacity;

    /** @brief The current number of elements. */
    isize count;

    /** @brief A function used for comparing priorities. */
    ScuCompareFunc* prioCmpFunc;

    /**
     * @brief The nodes storing the elements and their priorities.
     *
     * @note This is a dynamically allocated array of `capacity` nodes, or
     * `nullptr` if `capacity` is zero.
     */
    byte* nodes;

};

/** @brief The default capacity of a priority queue. */
static constexpr isize SCU_DEFAULT_CAPACITY = 8;

/** @brief The growth factor for increasing the capacity of a priority queue. */
static constexpr isize SCU_GROWTH_FACTOR = 2;

/** @brief The arity of the heap used to implement the priority queue. */
static constexpr isize SCU_HEAP_ARITY = 4;

SCU_API ScuPrioQueue* scu_prio_queue_new_impl(
    isize elemSize,
    isize prioSize,
    ScuCompareFunc* prioCmpFunc,
    ScuAllocator* allocator
) {
    return scu_prio_queue_new_with_capacity_impl(
        elemSize,
        prioSize,
        SCU_DEFAULT_CAPACITY,
        prioCmpFunc,
        allocator
    );
}

SCU_API ScuPrioQueue* scu_prio_queue_new_with_capacity_impl(
    isize elemSize,
    isize prioSize,
    isize capacity,
    ScuCompareFunc* prioCmpFunc,
    ScuAllocator* allocator
) {
    SCU_ASSERT(elemSize > 0);
    SCU_ASSERT(prioSize > 0);
    SCU_ASSERT(capacity >= 0);
    SCU_ASSERT(prioCmpFunc);
    SCU_ASSERT(allocator);
    ScuPrioQueue* prioQueue = SCU_NEW(ScuPrioQueue, allocator);
    if (!prioQueue) {
        return nullptr;
    }
    prioQueue->allocator = allocator;
    prioQueue->elemSize = elemSize;
    prioQueue->prioSize = prioSize;
    isize prioAlign = SCU_MIN(prioSize & -prioSize, SCU_ALIGNOF(max_align_t));
    isize elemAlign = SCU_MIN(elemSize & -elemSize, SCU_ALIGNOF(max_align_t));
    prioQueue->elemOffset = SCU_ALIGN_UP(prioSize, elemAlign);
    prioQueue->nodeAlign = SCU_MAX(prioAlign, elemAlign);
    prioQueue->nodeSize = SCU_ALIGN_UP(
        prioQueue->elemOffset + elemSize,
        prioQueue->nodeAlign
    );
    prioQueue->capacity = capacity;
    prioQueue->count = 0;
    prioQueue->prioCmpFunc = prioCmpFunc;
    if (capacity > 0) {
        prioQueue->nodes = scu_allocate(
            prioQueue->nodeSize * capacity,
            prioQueue->nodeAlign,
            prioQueue->allocator
        );
        if (!prioQueue->nodes) {
            SCU_DELETE(prioQueue, prioQueue->allocator);
            return nullptr;
        }
    }
    else {
        prioQueue->nodes = nullptr;
    }
    return prioQueue;
}

SCU_API ScuPrioQueue* scu_prio_queue_clone(const ScuPrioQueue* prioQueue) {
    SCU_ASSERT(prioQueue);
    ScuPrioQueue* clone = SCU_NEW(ScuPrioQueue, prioQueue->allocator);
    if (!clone) {
        return nullptr;
    }
    clone->allocator = prioQueue->allocator;
    clone->elemSize = prioQueue->elemSize;
    clone->prioSize = prioQueue->prioSize;
    clone->elemOffset = prioQueue->elemOffset;
    clone->nodeSize = prioQueue->nodeSize;
    clone->nodeAlign = prioQueue->nodeAlign;
    clone->capacity = prioQueue->capacity;
    clone->count = prioQueue->count;
    clone->prioCmpFunc = prioQueue->prioCmpFunc;
    if (clone->capacity > 0) {
        clone->nodes = scu_allocate(
            clone->nodeSize * clone->capacity,
            clone->nodeAlign,
            clone->allocator
        );
        if (!clone->nodes) {
            SCU_DELETE(clone, clone->allocator);
            return nullptr;
        }
        scu_memcpy(
            clone->nodes,
            prioQueue->nodes,
            clone->nodeSize * clone->count
        );
    }
    else {
        clone->nodes = nullptr;
    }
    return clone;
}

SCU_API isize scu_prio_queue_capacity(const ScuPrioQueue* prioQueue) {
    SCU_ASSERT(prioQueue);
    return prioQueue->capacity;
}

SCU_API isize scu_prio_queue_count(const ScuPrioQueue* prioQueue) {
    SCU_ASSERT(prioQueue);
    return prioQueue->count;
}

SCU_API ScuError scu_prio_queue_ensure_capacity(
    ScuPrioQueue* prioQueue,
    isize capacity
) {
    SCU_ASSERT(prioQueue);
    SCU_ASSERT(capacity >= 0);
    if (prioQueue->capacity < capacity) {
        isize newCapacity = (prioQueue->capacity > 0) ? prioQueue->capacity : 1;
        while (newCapacity < capacity) {
            newCapacity *= SCU_GROWTH_FACTOR;
        }
        byte* newNodes = scu_resize_or_move(
            prioQueue->nodes,
            prioQueue->nodeSize * prioQueue->capacity,
            prioQueue->nodeSize * newCapacity,
            prioQueue->nodeAlign,
            prioQueue->allocator
        );
        if (!newNodes) {
            return SCU_ERROR_OUT_OF_MEMORY;
        }
        prioQueue->nodes = newNodes;
        prioQueue->capacity = newCapacity;
    }
    return SCU_ERROR_NONE;
}

/**
 * @brief Returns a pointer to the priority of a node at an index.
 *
 * @param[in] prioQueue The priority queue to examine.
 * @param[in] index     The index of the node to retrieve the priority of.
 * @return A pointer to the priority of the node at the index.
 */
static inline byte* scu_node_prio(const ScuPrioQueue* prioQueue, isize index) {
    SCU_ASSERT(prioQueue);
    SCU_ASSERT(index >= 0 && index < prioQueue->capacity);
    return prioQueue->nodes + index * prioQueue->nodeSize;
}

/**
 * @brief Returns a pointer to the element of a node at an index.
 *
 * @param[in] prioQueue The priority queue to examine.
 * @param[in] index     The index of the node to retrieve the element of.
 * @return A pointer to the element of the node at the index.
 */
static inline byte* scu_node_elem(const ScuPrioQueue* prioQueue, isize index) {
    SCU_ASSERT(prioQueue);
    SCU_ASSERT(index >= 0 && index < prioQueue->capacity);
    return prioQueue->nodes + index * prioQueue->nodeSize
        + prioQueue->elemOffset;
}

SCU_API ScuError scu_prio_queue_enqueue(
    ScuPrioQueue* restrict prioQueue,
    const void* restrict elem,
    const void* restrict prio
) {
    SCU_ASSERT(prioQueue);
    SCU_ASSERT(elem);
    SCU_ASSERT(prio);
    ScuError error = scu_prio_queue_ensure_capacity(
        prioQueue,
        prioQueue->count + 1
    );
    if (error != SCU_ERROR_NONE) {
        return error;
    }
    isize index = prioQueue->count;
    scu_memcpy(scu_node_prio(prioQueue, index), prio, prioQueue->prioSize);
    scu_memcpy(scu_node_elem(prioQueue, index), elem, prioQueue->elemSize);
    prioQueue->count++;
    while (index > 0) {
        byte* nodePrio = scu_node_prio(prioQueue, index);
        isize parentIndex = (index - 1) / SCU_HEAP_ARITY;
        byte* parentPrio = scu_node_prio(prioQueue, parentIndex);
        if (prioQueue->prioCmpFunc(nodePrio, parentPrio) >= 0) {
            break;
        }
        scu_memswap(nodePrio, parentPrio, prioQueue->nodeSize);
        index = parentIndex;
    }
    return SCU_ERROR_NONE;
}

SCU_API void scu_prio_queue_dequeue(
    ScuPrioQueue* restrict prioQueue,
    void* restrict elem,
    void* restrict prio
) {
    if (!scu_prio_queue_try_dequeue(prioQueue, elem, prio)) {
        SCU_FATAL("The specified priority queue is empty.\n");
    }
}

SCU_API bool scu_prio_queue_try_dequeue(
    ScuPrioQueue* restrict prioQueue,
    void* restrict elem,
    void* restrict prio
) {
    SCU_ASSERT(prioQueue);
    SCU_ASSERT(elem);
    SCU_ASSERT(prio);
    if (prioQueue->count == 0) {
        return false;
    }
    scu_memcpy(elem, scu_node_elem(prioQueue, 0), prioQueue->elemSize);
    scu_memcpy(prio, scu_node_prio(prioQueue, 0), prioQueue->prioSize);
    prioQueue->count--;
    if (prioQueue->count > 0) {
        scu_memcpy(
            scu_node_prio(prioQueue, 0),
            scu_node_prio(prioQueue, prioQueue->count),
            prioQueue->nodeSize
        );
        isize index = 0;
        while (true) {
            isize firstChildIndex = index * SCU_HEAP_ARITY + 1;
            if (firstChildIndex >= prioQueue->count) {
                break;
            }
            isize lastChildIndex = SCU_MIN(
                firstChildIndex + SCU_HEAP_ARITY,
                prioQueue->count
            );
            isize minChildIndex = firstChildIndex;
            for (isize i = firstChildIndex + 1; i < lastChildIndex; i++) {
                const byte* childPrio = scu_node_prio(prioQueue, i);
                const byte* minChildPrio = scu_node_prio(
                    prioQueue,
                    minChildIndex
                );
                if (prioQueue->prioCmpFunc(childPrio, minChildPrio) < 0) {
                    minChildIndex = i;
                }
            }
            byte* nodePrio = scu_node_prio(prioQueue, index);
            byte* minChildPrio = scu_node_prio(prioQueue, minChildIndex);
            if (prioQueue->prioCmpFunc(nodePrio, minChildPrio) <= 0) {
                break;
            }
            scu_memswap(nodePrio, minChildPrio, prioQueue->nodeSize);
            index = minChildIndex;
        }
    }
    return true;
}

SCU_API void scu_prio_queue_peek_impl(
    const ScuPrioQueue* restrict prioQueue,
    void** restrict elem,
    void** restrict prio
) {
    if (!scu_prio_queue_try_peek_impl(prioQueue, elem, prio)) {
        SCU_FATAL("The specified priority queue is empty.\n");
    }
}

SCU_API bool scu_prio_queue_try_peek_impl(
    const ScuPrioQueue* restrict prioQueue,
    void** restrict elem,
    void** restrict prio
) {
    SCU_ASSERT(prioQueue);
    SCU_ASSERT(elem);
    SCU_ASSERT(prio);
    if (prioQueue->count == 0) {
        *elem = nullptr;
        *prio = nullptr;
        return false;
    }
    *elem = scu_node_elem(prioQueue, 0);
    *prio = scu_node_prio(prioQueue, 0);
    return true;
}

SCU_API void scu_prio_queue_clear(ScuPrioQueue* prioQueue) {
    SCU_ASSERT(prioQueue);
    prioQueue->count = 0;
}

SCU_API ScuError scu_prio_queue_trim_excess(ScuPrioQueue* prioQueue) {
    SCU_ASSERT(prioQueue);
    if (prioQueue->capacity > prioQueue->count) {
        if (prioQueue->count == 0) {
            scu_deallocate(
                prioQueue->nodes,
                prioQueue->nodeSize * prioQueue->capacity,
                prioQueue->nodeAlign,
                prioQueue->allocator
            );
            prioQueue->nodes = nullptr;
            prioQueue->capacity = 0;
        }
        else {
            isize newCapacity = prioQueue->count;
            byte* newNodes = scu_resize_or_move(
                prioQueue->nodes,
                prioQueue->nodeSize * prioQueue->capacity,
                prioQueue->nodeSize * newCapacity,
                prioQueue->nodeAlign,
                prioQueue->allocator
            );
            if (!newNodes) {
                return SCU_ERROR_OUT_OF_MEMORY;
            }
            prioQueue->nodes = newNodes;
            prioQueue->capacity = newCapacity;
        }
    }
    return SCU_ERROR_NONE;
}

SCU_API void scu_prio_queue_delete(ScuPrioQueue* prioQueue) {
    if (!prioQueue) {
        return;
    }
    scu_deallocate(
        prioQueue->nodes,
        prioQueue->nodeSize * prioQueue->capacity,
        prioQueue->nodeAlign,
        prioQueue->allocator
    );
    prioQueue->nodes = nullptr;
    prioQueue->capacity = 0;
    prioQueue->count = 0;
    SCU_DELETE(prioQueue, prioQueue->allocator);
}

SCU_API ScuPrioQueueIterator scu_prio_queue_begin(
    const ScuPrioQueue* prioQueue
) {
    SCU_ASSERT(prioQueue);
    return (ScuPrioQueueIterator) {
        .prioQueue = SCU_CONST_CAST(ScuPrioQueue*, prioQueue),
        .index = 0
    };
}

SCU_API ScuPrioQueueIterator scu_prio_queue_end(const ScuPrioQueue* prioQueue) {
    SCU_ASSERT(prioQueue);
    return (ScuPrioQueueIterator) {
        .prioQueue = SCU_CONST_CAST(ScuPrioQueue*, prioQueue),
        .index = prioQueue->count
    };
}

SCU_API bool scu_prio_queue_iterator_equal(
    const ScuPrioQueueIterator* lhs,
    const ScuPrioQueueIterator* rhs
) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    SCU_ASSERT(lhs->prioQueue == rhs->prioQueue);
    return lhs->index == rhs->index;
}

SCU_API void scu_prio_queue_iterator_next(ScuPrioQueueIterator* iterator) {
    SCU_ASSERT(iterator);
    SCU_ASSERT(iterator->index < iterator->prioQueue->count);
    iterator->index++;
}

SCU_API void scu_prio_queue_iterator_prev(ScuPrioQueueIterator* iterator) {
    SCU_ASSERT(iterator);
    SCU_ASSERT(iterator->index > 0);
    iterator->index--;
}

SCU_API ScuPrioQueueEntry scu_prio_queue_iterator_deref(
    const ScuPrioQueueIterator* iterator
) {
    SCU_ASSERT(iterator);
    const ScuPrioQueue* prioQueue = iterator->prioQueue;
    isize index = iterator->index;
    SCU_ASSERT(index >= 0 && index < prioQueue->count);
    return (ScuPrioQueueEntry) {
        .elem = scu_node_elem(prioQueue, index),
        .prio = scu_node_prio(prioQueue, index)
    };
}
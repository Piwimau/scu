#define SCU_SHORT_ALIASES

#include <stddef.h>
#include "scu/alloc.h"
#include "scu/assert.h"
#include "scu/math.h"
#include "scu/memory.h"
#include "scu/prio-queue.h"

struct ScuPrioQueue {

    /** @brief The size of each element (in bytes). */
    isize elemSize;

    /** @brief The size of each priority (in bytes). */
    isize prioSize;

    /** @brief The offset of the element within a node (in bytes). */
    isize elemOffset;

    /** @brief The effective size of a node (in bytes). */
    isize nodeSize;

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

[[nodiscard]]
ScuPrioQueue* scu_prio_queue_new(
    isize elemSize,
    isize prioSize,
    ScuCompareFunc prioCmpFunc
) {
    return scu_prio_queue_new_with_capacity(
        elemSize,
        prioSize,
        SCU_DEFAULT_CAPACITY,
        prioCmpFunc
    );
}

/**
 * @brief Rounds up a value to the next multiple of a specified alignment.
 *
 * @warning The behavior is undefined if `alignment` is not a power of two.
 *
 * @param[in] value     The value to round up.
 * @param[in] alignment The required alignment.
 * @return The smallest multiple of `alignment` greater than or equal to
 * `value`.
 */
static inline isize scu_align_up(isize value, isize alignment) {
    SCU_ASSERT(value >= 0);
    SCU_ASSERT(alignment > 0);
    SCU_ASSERT((alignment & (alignment - 1)) == 0);
    return (value + alignment - 1) & ~(alignment - 1);
}

[[nodiscard]]
ScuPrioQueue* scu_prio_queue_new_with_capacity(
    isize elemSize,
    isize prioSize,
    isize capacity,
    ScuCompareFunc prioCmpFunc
) {
    SCU_ASSERT(elemSize > 0);
    SCU_ASSERT(prioSize > 0);
    SCU_ASSERT(capacity >= 0);
    SCU_ASSERT(prioCmpFunc != nullptr);
    ScuPrioQueue* prioQueue = scu_malloc(SCU_SIZEOF(ScuPrioQueue));
    if (prioQueue == nullptr) {
        return nullptr;
    }
    prioQueue->elemSize = elemSize;
    prioQueue->prioSize = prioSize;
    prioQueue->elemOffset = scu_align_up(prioSize, SCU_ALIGNOF(max_align_t));
    prioQueue->nodeSize = scu_align_up(
        prioQueue->elemOffset + elemSize,
        SCU_ALIGNOF(max_align_t)
    );
    prioQueue->capacity = capacity;
    prioQueue->count = 0;
    prioQueue->prioCmpFunc = prioCmpFunc;
    if (capacity > 0) {
        prioQueue->nodes = scu_malloc(prioQueue->nodeSize * capacity);
        if (prioQueue->nodes == nullptr) {
            scu_free(prioQueue);
            return nullptr;
        }
    }
    else {
        prioQueue->nodes = nullptr;
    }
    return prioQueue;
}

[[nodiscard]]
ScuPrioQueue* scu_prio_queue_clone(const ScuPrioQueue* prioQueue) {
    SCU_ASSERT(prioQueue != nullptr);
    ScuPrioQueue* clone = scu_malloc(SCU_SIZEOF(ScuPrioQueue));
    if (clone == nullptr) {
        return nullptr;
    }
    clone->elemSize = prioQueue->elemSize;
    clone->prioSize = prioQueue->prioSize;
    clone->elemOffset = prioQueue->elemOffset;
    clone->nodeSize = prioQueue->nodeSize;
    clone->capacity = prioQueue->capacity;
    clone->count = prioQueue->count;
    clone->prioCmpFunc = prioQueue->prioCmpFunc;
    if (clone->capacity > 0) {
        clone->nodes = scu_malloc(clone->nodeSize * clone->capacity);
        if (clone->nodes == nullptr) {
            scu_free(clone);
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

isize scu_prio_queue_capacity(const ScuPrioQueue* prioQueue) {
    SCU_ASSERT(prioQueue != nullptr);
    return prioQueue->capacity;
}

isize scu_prio_queue_count(const ScuPrioQueue* prioQueue) {
    SCU_ASSERT(prioQueue != nullptr);
    return prioQueue->count;
}

ScuError scu_prio_queue_ensure_capacity(
    ScuPrioQueue* prioQueue,
    isize capacity
) {
    SCU_ASSERT(prioQueue != nullptr);
    SCU_ASSERT(capacity >= 0);
    if (prioQueue->capacity < capacity) {
        isize newCapacity = (prioQueue->capacity > 0) ? prioQueue->capacity : 1;
        while (newCapacity < capacity) {
            newCapacity *= SCU_GROWTH_FACTOR;
        }
        byte* newNodes = scu_realloc(
            prioQueue->nodes,
            prioQueue->nodeSize * newCapacity
        );
        if (newNodes == nullptr) {
            return SCU_ERROR_OUT_OF_MEMORY;
        }
        prioQueue->nodes = newNodes;
        prioQueue->capacity = newCapacity;
    }
    return SCU_ERROR_NONE;
}

/**
 * @brief Returns a pointer to the priority of a node at a specified index.
 *
 * @param[in] prioQueue The priority queue to examine.
 * @param[in] index     The index of the node to retrieve the priority of.
 * @return A pointer to the priority of the node at the specified index.
 */
static inline byte* scu_node_prio(const ScuPrioQueue* prioQueue, isize index) {
    SCU_ASSERT(prioQueue != nullptr);
    SCU_ASSERT((index >= 0) && (index < prioQueue->capacity));
    return prioQueue->nodes + index * prioQueue->nodeSize;
}

/**
 * @brief Returns a pointer to the element of a node at a specified index.
 *
 * @param[in] prioQueue The priority queue to examine.
 * @param[in] index     The index of the node to retrieve the element of.
 * @return A pointer to the element of the node at the specified index.
 */
static inline byte* scu_node_elem(const ScuPrioQueue* prioQueue, isize index) {
    SCU_ASSERT(prioQueue != nullptr);
    SCU_ASSERT((index >= 0) && (index < prioQueue->capacity));
    return prioQueue->nodes + index * prioQueue->nodeSize
        + prioQueue->elemOffset;
}

ScuError scu_prio_queue_enqueue(
    ScuPrioQueue* restrict prioQueue,
    const void* restrict elem,
    const void* restrict prio
) {
    SCU_ASSERT(prioQueue != nullptr);
    SCU_ASSERT(elem != nullptr);
    SCU_ASSERT(prio != nullptr);
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

void scu_prio_queue_dequeue(
    ScuPrioQueue* restrict prioQueue,
    void* restrict elem,
    void* restrict prio
) {
    if (!scu_prio_queue_try_dequeue(prioQueue, elem, prio)) {
        SCU_FATAL("The specified priority queue is empty.\n");
    }
}

bool scu_prio_queue_try_dequeue(
    ScuPrioQueue* restrict prioQueue,
    void* restrict elem,
    void* restrict prio
) {
    SCU_ASSERT(prioQueue != nullptr);
    SCU_ASSERT(elem != nullptr);
    SCU_ASSERT(prio != nullptr);
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
                byte* childPrio = scu_node_prio(prioQueue, i);
                byte* minChildPrio = scu_node_prio(prioQueue, minChildIndex);
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

void scu_prio_queue_peek_impl(
    const ScuPrioQueue* restrict prioQueue,
    void** restrict elem,
    void** restrict prio
) {
    if (!scu_prio_queue_try_peek_impl(prioQueue, elem, prio)) {
        SCU_FATAL("The specified priority queue is empty.\n");
    }
}

bool scu_prio_queue_try_peek_impl(
    const ScuPrioQueue* restrict prioQueue,
    void** restrict elem,
    void** restrict prio
) {
    SCU_ASSERT(prioQueue != nullptr);
    SCU_ASSERT(elem != nullptr);
    SCU_ASSERT(prio != nullptr);
    if (prioQueue->count == 0) {
        *elem = nullptr;
        *prio = nullptr;
        return false;
    }
    *elem = scu_node_elem(prioQueue, 0);
    *prio = scu_node_prio(prioQueue, 0);
    return true;
}

void scu_prio_queue_clear(ScuPrioQueue* prioQueue) {
    SCU_ASSERT(prioQueue != nullptr);
    prioQueue->count = 0;
}

ScuError scu_prio_queue_trim_excess(ScuPrioQueue* prioQueue) {
    SCU_ASSERT(prioQueue != nullptr);
    if (prioQueue->capacity > prioQueue->count) {
        if (prioQueue->count == 0) {
            scu_free(prioQueue->nodes);
            prioQueue->nodes = nullptr;
            prioQueue->capacity = 0;
        }
        else {
            isize newCapacity = prioQueue->count;
            byte* newNodes = scu_realloc(
                prioQueue->nodes,
                prioQueue->nodeSize * newCapacity
            );
            if (newNodes == nullptr) {
                return SCU_ERROR_OUT_OF_MEMORY;
            }
            prioQueue->nodes = newNodes;
            prioQueue->capacity = newCapacity;
        }
    }
    return SCU_ERROR_NONE;
}

ScuPrioQueueIter scu_prio_queue_iter(const ScuPrioQueue* prioQueue) {
    SCU_ASSERT(prioQueue != nullptr);
    return (ScuPrioQueueIter) {
        .prioQueue = SCU_CONST_CAST(ScuPrioQueue*, prioQueue),
        .index = -1
    };
}

bool scu_prio_queue_iter_move_next(ScuPrioQueueIter* iter) {
    SCU_ASSERT(iter != nullptr);
    ScuPrioQueue* prioQueue = iter->prioQueue;
    isize index = iter->index;
    SCU_ASSERT(prioQueue != nullptr);
    SCU_ASSERT((index >= -1) && (index < prioQueue->count));
    if ((index + 1) < prioQueue->count) {
        iter->index++;
        return true;
    }
    return false;
}

ScuPrioQueueEntry scu_prio_queue_iter_current(const ScuPrioQueueIter* iter) {
    SCU_ASSERT(iter != nullptr);
    ScuPrioQueue* prioQueue = iter->prioQueue;
    isize index = iter->index;
    SCU_ASSERT(prioQueue != nullptr);
    SCU_ASSERT((index >= 0) && (index < prioQueue->count));
    return (ScuPrioQueueEntry) {
        .elem = scu_node_elem(prioQueue, index),
        .prio = scu_node_prio(prioQueue, index)
    };
}

void scu_prio_queue_iter_reset(ScuPrioQueueIter* iter) {
    SCU_ASSERT(iter != nullptr);
    SCU_ASSERT(iter->prioQueue != nullptr);
    SCU_ASSERT((iter->index >= -1) && (iter->index < iter->prioQueue->count));
    iter->index = -1;
}

void scu_prio_queue_free(ScuPrioQueue* prioQueue) {
    if (prioQueue != nullptr) {
        scu_free(prioQueue->nodes);
        prioQueue->nodes = nullptr;
        prioQueue->capacity = 0;
        prioQueue->count = 0;
        scu_free(prioQueue);
    }
}
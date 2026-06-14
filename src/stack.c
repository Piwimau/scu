#define SCU_SHORT_ALIASES

#include "scu/alloc.h"
#include "scu/assert.h"
#include "scu/memory.h"
#include "scu/stack.h"

struct ScuStack {

    /** @brief The size of each element (in bytes). */
    isize elemSize;

    /**
     * @brief The maximum number of elements that can be stored before a
     * reallocation is required.
     */
    isize capacity;

    /** @brief The current number of elements. */
    isize count;

    /**
     * @brief The actual data (i.e., the elements) stored in the stack.
     *
     * @note This is a dynamically allocated array of `count` elements of size
     * `elemSize`, or a `nullptr` if `capacity` is zero.
     */
    byte* data;

};

/** @brief The default capacity of a stack. */
static constexpr isize SCU_DEFAULT_CAPACITY = 8;

/** @brief The growth factor for increasing the capacity of a stack. */
static constexpr isize SCU_GROWTH_FACTOR = 2;

[[nodiscard]]
ScuStack* scu_stack_new(isize elemSize) {
    return scu_stack_new_with_capacity(elemSize, SCU_DEFAULT_CAPACITY);
}

[[nodiscard]]
ScuStack* scu_stack_new_with_capacity(isize elemSize, isize capacity) {
    SCU_ASSERT(elemSize > 0);
    SCU_ASSERT(capacity >= 0);
    ScuStack* stack = scu_malloc(SCU_SIZEOF(ScuStack));
    if (stack == nullptr) {
        return nullptr;
    }
    stack->elemSize = elemSize;
    stack->capacity = capacity;
    stack->count = 0;
    if (capacity > 0) {
        stack->data = scu_malloc(elemSize * capacity);
        if (stack->data == nullptr) {
            scu_free(stack);
            return nullptr;
        }
    }
    else {
        stack->data = nullptr;
    }
    return stack;
}

[[nodiscard]]
ScuStack* scu_stack_clone(const ScuStack* stack) {
    SCU_ASSERT(stack != nullptr);
    ScuStack* clone = scu_malloc(SCU_SIZEOF(ScuStack));
    if (clone == nullptr) {
        return nullptr;
    }
    clone->elemSize = stack->elemSize;
    clone->capacity = stack->capacity;
    clone->count = stack->count;
    if (clone->capacity > 0) {
        clone->data = scu_malloc(clone->elemSize * clone->capacity);
        if (clone->data == nullptr) {
            scu_free(clone);
            return nullptr;
        }
        scu_memcpy(clone->data, stack->data, clone->elemSize * clone->count);
    }
    else {
        clone->data = nullptr;
    }
    return clone;
}

isize scu_stack_capacity(const ScuStack* stack) {
    SCU_ASSERT(stack != nullptr);
    return stack->capacity;
}

isize scu_stack_count(const ScuStack* stack) {
    SCU_ASSERT(stack != nullptr);
    return stack->count;
}

ScuError scu_stack_ensure_capacity(ScuStack* stack, isize capacity) {
    SCU_ASSERT(stack != nullptr);
    SCU_ASSERT(capacity >= 0);
    if (stack->capacity < capacity) {
        isize newCapacity = (stack->capacity > 0) ? stack->capacity : 1;
        while (newCapacity < capacity) {
            newCapacity *= SCU_GROWTH_FACTOR;
        }
        byte* newData = scu_realloc(stack->data, stack->elemSize * newCapacity);
        if (newData == nullptr) {
            return SCU_ERROR_OUT_OF_MEMORY;
        }
        stack->capacity = newCapacity;
        stack->data = newData;
    }
    return SCU_ERROR_NONE;
}

ScuError scu_stack_push(ScuStack* restrict stack, const void* restrict elem) {
    SCU_ASSERT(stack != nullptr);
    SCU_ASSERT(elem != nullptr);
    ScuError error = scu_stack_ensure_capacity(stack, stack->count + 1);
    if (error != SCU_ERROR_NONE) {
        return error;
    }
    scu_memcpy(
        stack->data + stack->elemSize * stack->count,
        elem,
        stack->elemSize
    );
    stack->count++;
    return SCU_ERROR_NONE;
}

void scu_stack_pop(ScuStack* restrict stack, void* restrict elem) {
    SCU_ASSERT(stack != nullptr);
    SCU_ASSERT(stack->count > 0);
    SCU_ASSERT(elem != nullptr);
    stack->count--;
    scu_memcpy(
        elem,
        stack->data + stack->elemSize * stack->count,
        stack->elemSize
    );
}

bool scu_stack_try_pop(ScuStack* restrict stack, void* restrict elem) {
    SCU_ASSERT(stack != nullptr);
    SCU_ASSERT(elem != nullptr);
    if (stack->count == 0) {
        return false;
    }
    stack->count--;
    scu_memcpy(
        elem,
        stack->data + stack->elemSize * stack->count,
        stack->elemSize
    );
    return true;
}

void scu_stack_peek_impl(const ScuStack* restrict stack, void** restrict elem) {
    SCU_ASSERT(stack != nullptr);
    SCU_ASSERT(stack->count > 0);
    SCU_ASSERT(elem != nullptr);
    *elem = stack->data + stack->elemSize * (stack->count - 1);
}

bool scu_stack_try_peek_impl(
    const ScuStack* restrict stack,
    void** restrict elem
) {
    SCU_ASSERT(stack != nullptr);
    SCU_ASSERT(elem != nullptr);
    if (stack->count == 0) {
        *elem = nullptr;
        return false;
    }
    *elem = stack->data + stack->elemSize * (stack->count - 1);
    return true;
}

void scu_stack_clear(ScuStack* stack) {
    SCU_ASSERT(stack != nullptr);
    stack->count = 0;
}

ScuError scu_stack_trim_excess(ScuStack* stack) {
    SCU_ASSERT(stack != nullptr);
    if (stack->capacity > stack->count) {
        if (stack->count == 0) {
            scu_free(stack->data);
            stack->data = nullptr;
            stack->capacity = 0;
        }
        else {
            byte* newData = scu_realloc(
                stack->data,
                stack->elemSize * stack->count
            );
            if (newData == nullptr) {
                return SCU_ERROR_OUT_OF_MEMORY;
            }
            stack->data = newData;
            stack->capacity = stack->count;
        }
    }
    return SCU_ERROR_NONE;
}

ScuStackIter scu_stack_iter(const ScuStack* stack) {
    SCU_ASSERT(stack != nullptr);
    return (ScuStackIter) {
        .stack = SCU_CONST_CAST(ScuStack*, stack),
        .index = stack->count
    };
}

bool scu_stack_iter_move_next(ScuStackIter* iter) {
    SCU_ASSERT(iter != nullptr);
    SCU_ASSERT(iter->stack != nullptr);
    ScuStack* stack = iter->stack;
    isize index = iter->index;
    SCU_ASSERT((index >= 0) && (index <= stack->count));
    if (stack->count == 0) {
        iter->index = 0;
        return false;
    }
    index--;
    if (index >= 0) {
        iter->index = index;
        return true;
    }
    iter->index = 0;
    return false;
}

void* scu_stack_iter_current(const ScuStackIter* iter) {
    SCU_ASSERT(iter != nullptr);
    SCU_ASSERT(iter->stack != nullptr);
    ScuStack* stack = iter->stack;
    isize index = iter->index;
    SCU_ASSERT((index >= 0) && (index < stack->count));
    return stack->data + stack->elemSize * index;
}

void scu_stack_iter_reset(ScuStackIter* iter) {
    SCU_ASSERT(iter != nullptr);
    SCU_ASSERT(iter->stack != nullptr);
    SCU_ASSERT((iter->index >= 0) && (iter->index <= iter->stack->count));
    iter->index = iter->stack->count;
}

void scu_stack_free(ScuStack* stack) {
    if (stack != nullptr) {
        scu_free(stack->data);
        stack->data = nullptr;
        stack->capacity = 0;
        stack->count = 0;
        scu_free(stack);
    }
}
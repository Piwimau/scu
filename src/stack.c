#define SCU_SHORT_ALIASES

#include <stddef.h>
#include "scu/assert.h"
#include "scu/math.h"
#include "scu/stack.h"

struct ScuStack {

    /** @brief The allocator used for memory management. */
    ScuAllocator* allocator;

    /** @brief The size of each element (in bytes). */
    isize elemSize;

    /** @brief The alignment of each element (in bytes). */
    isize elemAlign;

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
     * @note This is a dynamically allocated array of `capacity` elements of
     * size `elemSize`, or `nullptr` if `capacity` is zero.
     */
    byte* data;

};

/** @brief The default capacity of a stack. */
static constexpr isize SCU_DEFAULT_CAPACITY = 8;

/** @brief The growth factor for increasing the capacity of a stack. */
static constexpr isize SCU_GROWTH_FACTOR = 2;

SCU_API ScuStack* scu_stack_new_impl(isize elemSize, ScuAllocator* allocator) {
    return scu_stack_new_with_capacity_impl(
        elemSize,
        SCU_DEFAULT_CAPACITY,
        allocator
    );
}

SCU_API ScuStack* scu_stack_new_with_capacity_impl(
    isize elemSize,
    isize capacity,
    ScuAllocator* allocator
) {
    SCU_ASSERT(elemSize > 0);
    SCU_ASSERT(capacity >= 0);
    SCU_ASSERT(allocator);
    ScuStack* stack = SCU_NEW(ScuStack, allocator);
    if (!stack) {
        return nullptr;
    }
    stack->allocator = allocator;
    stack->elemAlign = SCU_MIN(elemSize & -elemSize, SCU_ALIGNOF(max_align_t));
    stack->elemSize = SCU_ALIGN_UP(elemSize, stack->elemAlign);
    stack->capacity = capacity;
    stack->count = 0;
    if (capacity > 0) {
        stack->data = scu_allocate(
            stack->elemSize * stack->capacity,
            stack->elemAlign,
            stack->allocator
        );
        if (!stack->data) {
            SCU_DELETE(stack, stack->allocator);
            return nullptr;
        }
    }
    else {
        stack->data = nullptr;
    }
    return stack;
}

SCU_API ScuStack* scu_stack_clone(const ScuStack* stack) {
    SCU_ASSERT(stack);
    ScuStack* clone = SCU_NEW(ScuStack, stack->allocator);
    if (!clone) {
        return nullptr;
    }
    clone->allocator = stack->allocator;
    clone->elemSize = stack->elemSize;
    clone->elemAlign = stack->elemAlign;
    clone->capacity = stack->capacity;
    clone->count = stack->count;
    if (clone->capacity > 0) {
        clone->data = scu_allocate(
            clone->elemSize * clone->capacity,
            clone->elemAlign,
            clone->allocator
        );
        if (!clone->data) {
            SCU_DELETE(clone, clone->allocator);
            return nullptr;
        }
        scu_memcpy(clone->data, stack->data, clone->elemSize * clone->count);
    }
    else {
        clone->data = nullptr;
    }
    return clone;
}

SCU_API isize scu_stack_capacity(const ScuStack* stack) {
    SCU_ASSERT(stack);
    return stack->capacity;
}

SCU_API isize scu_stack_count(const ScuStack* stack) {
    SCU_ASSERT(stack);
    return stack->count;
}

SCU_API ScuError scu_stack_ensure_capacity(ScuStack* stack, isize capacity) {
    SCU_ASSERT(stack);
    SCU_ASSERT(capacity >= 0);
    if (stack->capacity < capacity) {
        isize newCapacity = (stack->capacity > 0) ? stack->capacity : 1;
        while (newCapacity < capacity) {
            newCapacity *= SCU_GROWTH_FACTOR;
        }
        byte* newData = scu_resize_or_move(
            stack->data,
            stack->elemSize * stack->capacity,
            stack->elemSize * newCapacity,
            stack->elemAlign,
            stack->allocator
        );
        if (!newData) {
            return SCU_ERROR_OUT_OF_MEMORY;
        }
        stack->capacity = newCapacity;
        stack->data = newData;
    }
    return SCU_ERROR_NONE;
}

SCU_API ScuError scu_stack_push(
    ScuStack* restrict stack,
    const void* restrict elem
) {
    SCU_ASSERT(stack);
    SCU_ASSERT(elem);
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

SCU_API void scu_stack_pop(ScuStack* restrict stack, void* restrict elem) {
    SCU_ASSERT(stack);
    SCU_ASSERT(stack->count > 0);
    SCU_ASSERT(elem);
    stack->count--;
    scu_memcpy(
        elem,
        stack->data + stack->elemSize * stack->count,
        stack->elemSize
    );
}

SCU_API bool scu_stack_try_pop(ScuStack* restrict stack, void* restrict elem) {
    SCU_ASSERT(stack);
    SCU_ASSERT(elem);
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

SCU_API void scu_stack_peek_impl(
    const ScuStack* restrict stack,
    void** restrict elem
) {
    SCU_ASSERT(stack);
    SCU_ASSERT(stack->count > 0);
    SCU_ASSERT(elem);
    *elem = stack->data + stack->elemSize * (stack->count - 1);
}

SCU_API bool scu_stack_try_peek_impl(
    const ScuStack* restrict stack,
    void** restrict elem
) {
    SCU_ASSERT(stack);
    SCU_ASSERT(elem);
    if (stack->count == 0) {
        *elem = nullptr;
        return false;
    }
    *elem = stack->data + stack->elemSize * (stack->count - 1);
    return true;
}

SCU_API void scu_stack_clear(ScuStack* stack) {
    SCU_ASSERT(stack);
    stack->count = 0;
}

SCU_API ScuError scu_stack_trim_excess(ScuStack* stack) {
    SCU_ASSERT(stack);
    if (stack->capacity > stack->count) {
        if (stack->count == 0) {
            scu_deallocate(
                stack->data,
                stack->elemSize * stack->capacity,
                stack->elemAlign,
                stack->allocator
            );
            stack->data = nullptr;
            stack->capacity = 0;
        }
        else {
            byte* newData = scu_resize_or_move(
                stack->data,
                stack->elemSize * stack->capacity,
                stack->elemSize * stack->count,
                stack->elemAlign,
                stack->allocator
            );
            if (!newData) {
                return SCU_ERROR_OUT_OF_MEMORY;
            }
            stack->data = newData;
            stack->capacity = stack->count;
        }
    }
    return SCU_ERROR_NONE;
}

SCU_API void scu_stack_delete(ScuStack* stack) {
    if (!stack) {
        return;
    }
    scu_deallocate(
        stack->data,
        stack->elemSize * stack->capacity,
        stack->elemAlign,
        stack->allocator
    );
    stack->data = nullptr;
    stack->capacity = 0;
    stack->count = 0;
    SCU_DELETE(stack, stack->allocator);
}

SCU_API ScuStackIterator scu_stack_begin(const ScuStack* stack) {
    SCU_ASSERT(stack);
    return (ScuStackIterator) {
        .stack = SCU_CONST_CAST(ScuStack*, stack),
        .index = stack->count - 1
    };
}

SCU_API ScuStackIterator scu_stack_end(const ScuStack* stack) {
    SCU_ASSERT(stack);
    return (ScuStackIterator) {
        .stack = SCU_CONST_CAST(ScuStack*, stack),
        .index = -1
    };
}

SCU_API bool scu_stack_iterator_equal(
    const ScuStackIterator* lhs,
    const ScuStackIterator* rhs
) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    SCU_ASSERT(lhs->stack == rhs->stack);
    return lhs->index == rhs->index;
}

SCU_API void scu_stack_iterator_next(ScuStackIterator* iterator) {
    SCU_ASSERT(iterator);
    SCU_ASSERT(iterator->index >= 0);
    iterator->index--;
}

SCU_API void scu_stack_iterator_prev(ScuStackIterator* iterator) {
    SCU_ASSERT(iterator);
    SCU_ASSERT(iterator->index < iterator->stack->count - 1);
    iterator->index++;
}

SCU_API void* scu_stack_iterator_deref(const ScuStackIterator* iterator) {
    SCU_ASSERT(iterator);
    const ScuStack* stack = iterator->stack;
    isize index = iterator->index;
    SCU_ASSERT(index >= 0 && index < stack->count);
    return stack->data + stack->elemSize * index;
}

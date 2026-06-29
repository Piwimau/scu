#ifndef SCU_H
#define SCU_H

#include "scu/alloc.h"
#include "scu/array.h"
#include "scu/assert.h"
#include "scu/bench.h"
#include "scu/common.h"
#include "scu/compare.h"
#include "scu/equal.h"
#include "scu/error.h"
#include "scu/hash-map.h"
#include "scu/hash-set.h"
#include "scu/hash.h"
#include "scu/io.h"
#include "scu/list.h"
#include "scu/math.h"
#include "scu/memory.h"
#include "scu/prio-queue.h"
#include "scu/queue.h"
#include "scu/stack.h"
#include "scu/string.h"
#include "scu/time.h"
#include "scu/types.h"

/** @brief The major version number of SCU. */
#define SCU_VERSION_MAJOR 0

/** @brief The minor version number of SCU. */
#define SCU_VERSION_MINOR 3

/** @brief The patch version number of SCU. */
#define SCU_VERSION_PATCH 0

/** @brief The version string of SCU. */
#define SCU_VERSION_STRING "0.3.0"

#endif
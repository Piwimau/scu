#define SCU_SHORT_ALIASES

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else
    #ifndef _POSIX_C_SOURCE
        #define _POSIX_C_SOURCE 199309L
    #endif
    #include <time.h>
#endif
#include <tgmath.h>
#include "scu/array.h"
#include "scu/assert.h"
#include "scu/bench.h"
#include "scu/compare.h"
#include "scu/memory.h"

/** @brief The number of nanoseconds per second. */
static constexpr i64 SCU_NANOS_PER_SEC = 1'000'000'000;

#ifdef _WIN32
    /** @brief The number of nanoseconds per `FILETIME` tick. */
    static constexpr i64 SCU_NANOS_PER_TICK = 100;

    /**
     * @brief Converts a `FILETIME` to nanoseconds.
     *
     * @param[in] filetime The `FILETIME` to convert.
     * @return The converted `FILETIME` in nanoseconds.
     */
    static inline i64 scu_filetime_to_ns(const FILETIME* filetime) {
        SCU_ASSERT(filetime);
        u64 low = filetime->dwLowDateTime;
        u64 high = filetime->dwHighDateTime;
        u64 ticks = (high << 32) | low;
        return (i64) (ticks * SCU_NANOS_PER_TICK);
    }
#else
    /** @brief A time interval in seconds and nanoseconds. */
    typedef struct timespec ScuTimespec;

    /**
     * @brief Converts a `ScuTimespec` to nanoseconds.
     *
     * @param[in] timespec The `ScuTimespec` to convert.
     * @return The converted `ScuTimespec` in nanoseconds.
     */
    static inline i64 scu_timespec_to_ns(const ScuTimespec* timespec) {
        SCU_ASSERT(timespec);
        return timespec->tv_sec * SCU_NANOS_PER_SEC + timespec->tv_nsec;
    }
#endif

/**
 * @brief Returns the current wall time (in nanoseconds).
 *
 * @return The current wall time (in nanoseconds), or `-1` on failure.
 */
static inline i64 scu_wall_ns() {
#ifdef _WIN32
    LARGE_INTEGER counter;
    if (!QueryPerformanceCounter(&counter)) {
        return -1;
    }
    LARGE_INTEGER frequency;
    if (!QueryPerformanceFrequency(&frequency)) {
        return -1;
    }
    return counter.QuadPart * SCU_NANOS_PER_SEC / frequency.QuadPart;
#else
    ScuTimespec timespec;
    if (clock_gettime(CLOCK_MONOTONIC, &timespec) != 0) {
        return -1;
    }
    return scu_timespec_to_ns(&timespec);
#endif
}

/**
 * @brief Returns the current process CPU time (in nanoseconds).
 *
 * @return The current process CPU time (in nanoseconds), or `-1` on failure.
 */
static inline i64 scu_process_cpu_ns() {
#ifdef _WIN32
    FILETIME creationTime, exitTime, kernelTime, userTime;
    BOOL success = GetProcessTimes(
        GetCurrentProcess(),
        &creationTime,
        &exitTime,
        &kernelTime,
        &userTime
    );
    if (!success) {
        return -1;
    }
    return scu_filetime_to_ns(&kernelTime) + scu_filetime_to_ns(&userTime);
#else
    ScuTimespec timespec;
    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timespec) != 0) {
        return -1;
    }
    return scu_timespec_to_ns(&timespec);
#endif
}

/**
 * @brief Returns the current thread CPU time (in nanoseconds).
 *
 * @return The current thread CPU time (in nanoseconds), or `-1` on failure.
 */
static inline i64 scu_thread_cpu_ns() {
#ifdef _WIN32
    FILETIME creationTime, exitTime, kernelTime, userTime;
    BOOL success = GetThreadTimes(
        GetCurrentThread(),
        &creationTime,
        &exitTime,
        &kernelTime,
        &userTime
    );
    if (!success) {
        return -1;
    }
    return scu_filetime_to_ns(&kernelTime) + scu_filetime_to_ns(&userTime);
#else
    ScuTimespec timespec;
    if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &timespec) != 0) {
        return -1;
    }
    return scu_timespec_to_ns(&timespec);
#endif
}

/**
 * @brief Returns the current CPU time (in nanoseconds).
 *
 * @param[in] timingMode The timing mode for measuring CPU time.
 * @return The current CPU time (in nanoseconds), or `-1` on failure.
 */
static inline i64 scu_cpu_ns(ScuTimingMode timingMode) {
    return (timingMode == SCU_TIMING_MODE_PROCESS)
        ? scu_process_cpu_ns()
        : scu_thread_cpu_ns();
}

SCU_API void scu_stopwatch_init(
    ScuStopwatch* stopwatch,
    ScuTimingMode timingMode
) {
    SCU_ASSERT(stopwatch);
    *stopwatch = (ScuStopwatch) { .timingMode = timingMode };
}

SCU_API bool scu_stopwatch_start(ScuStopwatch* stopwatch) {
    SCU_ASSERT(stopwatch);
    if (!stopwatch->isRunning) {
        i64 wallNs = scu_wall_ns();
        if (wallNs == -1) {
            return false;
        }
        i64 cpuNs = scu_cpu_ns(stopwatch->timingMode);
        if (cpuNs == -1) {
            return false;
        }
        stopwatch->startWallNs = wallNs;
        stopwatch->startCpuNs = cpuNs;
        stopwatch->isRunning = true;
    }
    return true;
}

SCU_API bool scu_stopwatch_restart(ScuStopwatch* stopwatch) {
    SCU_ASSERT(stopwatch);
    i64 wallNs = scu_wall_ns();
    if (wallNs == -1) {
        return false;
    }
    i64 cpuNs = scu_cpu_ns(stopwatch->timingMode);
    if (cpuNs == -1) {
        return false;
    }
    stopwatch->startWallNs = wallNs;
    stopwatch->startCpuNs = cpuNs;
    stopwatch->accWallNs = 0;
    stopwatch->accCpuNs = 0;
    stopwatch->isRunning = true;
    return true;
}

SCU_API bool scu_stopwatch_stop(ScuStopwatch* stopwatch) {
    SCU_ASSERT(stopwatch);
    if (stopwatch->isRunning) {
        i64 wallNs = scu_wall_ns();
        if (wallNs == -1) {
            return false;
        }
        i64 cpuNs = scu_cpu_ns(stopwatch->timingMode);
        if (cpuNs == -1) {
            return false;
        }
        stopwatch->accWallNs += wallNs - stopwatch->startWallNs;
        stopwatch->accCpuNs += cpuNs - stopwatch->startCpuNs;
        stopwatch->isRunning = false;
    }
    return true;
}

SCU_API void scu_stopwatch_reset(ScuStopwatch* stopwatch) {
    SCU_ASSERT(stopwatch);
    *stopwatch = (ScuStopwatch) { .timingMode = stopwatch->timingMode };
}

SCU_API bool scu_stopwatch_is_running(const ScuStopwatch* stopwatch) {
    SCU_ASSERT(stopwatch);
    return stopwatch->isRunning;
}

SCU_API ScuTimingResult scu_stopwatch_elapsed(const ScuStopwatch* stopwatch) {
    SCU_ASSERT(stopwatch);
    if (!stopwatch->isRunning) {
        return (ScuTimingResult) {
            .wallNs = stopwatch->accWallNs,
            .cpuNs = stopwatch->accCpuNs,
            .error = SCU_ERROR_NONE
        };
    }
    i64 wallNs = scu_wall_ns();
    i64 cpuNs = scu_cpu_ns(stopwatch->timingMode);
    if (wallNs == -1 || cpuNs == -1) {
        return (ScuTimingResult) {
            .wallNs = -1,
            .cpuNs = -1,
            .error = SCU_ERROR_TIMING_FAILED
        };
    }
    return (ScuTimingResult) {
        .wallNs = stopwatch->accWallNs + wallNs - stopwatch->startWallNs,
        .cpuNs = stopwatch->accCpuNs + cpuNs - stopwatch->startCpuNs,
        .error = SCU_ERROR_NONE
    };
}

SCU_API ScuBenchContext scu_bench_context_new(
    ScuTimingMode timingMode,
    isize warmups,
    isize iterations,
    ScuBenchResult* benchResult
) {
    SCU_ASSERT(warmups >= 0);
    SCU_ASSERT(iterations >= 0);
    SCU_ASSERT(benchResult);
    ScuBenchContext context = {
        .timingMode = timingMode,
        .warmups = warmups,
        .iterations = iterations,
        .benchResult = benchResult
    };
    if (context.iterations > 0) {
        context.wallSamples = SCU_ARRAY_NEW(i64, context.iterations);
        context.cpuSamples = SCU_ARRAY_NEW(i64, context.iterations);
        if (!context.wallSamples || !context.cpuSamples) {
            SCU_ARRAY_DELETE(context.wallSamples, context.iterations);
            SCU_ARRAY_DELETE(context.cpuSamples, context.iterations);
            context.wallSamples = nullptr;
            context.cpuSamples = nullptr;
            context.error = SCU_ERROR_OUT_OF_MEMORY;
        }
    }
    return context;
}

/**
 * @brief Returns benchmark statistics for a sequence of samples.
 *
 * @param[in, out] samples A sequence of samples (in nanoseconds).
 * @param[in]      count   The number of samples.
 * @return The benchmark statistics for the sequence of samples.
 *
 * @note The sequence of samples is sorted in-place.
 *
 * @warning The behavior is undefined if `samples` is `nullptr`, or if `count`
 * is less than or equal to zero.
 */
static ScuBenchStats scu_bench_stats_from_samples(i64* samples, isize count) {
    SCU_ASSERT(samples);
    SCU_ASSERT(count > 0);
    scu_array_sort(samples, count, SCU_SIZEOF(i64), scu_compare_i64);
    f64 totalNs = 0.0;
    for (isize i = 0; i < count; i++) {
        totalNs += (f64) samples[i];
    }
    f64 meanNs = totalNs / (f64) count;
    f64 sumOfSquaredDiffsNs = 0.0;
    for (isize i = 0; i < count; i++) {
        f64 diff = (f64) samples[i] - meanNs;
        sumOfSquaredDiffsNs += diff * diff;
    }
    f64 medianNs = (count % 2 == 0)
        ? ((f64) samples[count / 2 - 1] + (f64) samples[count / 2]) / 2.0
        : (f64) samples[count / 2];
    f64 stdDevNs = (count > 1)
        ? sqrt(sumOfSquaredDiffsNs / (f64) (count - 1))
        : 0.0;
    return (ScuBenchStats) {
        .minNs = samples[0],
        .maxNs = samples[count - 1],
        .meanNs = meanNs,
        .medianNs = medianNs,
        .stdDevNs = stdDevNs
    };
}

SCU_API bool scu_bench_context_is_running(ScuBenchContext* context) {
    SCU_ASSERT(context);
    if (context->error != SCU_ERROR_NONE) {
        *context->benchResult = (ScuBenchResult) {
            .iterations = (context->iteration > context->warmups)
                ? context->iteration - context->warmups
                : 0,
            .error = context->error
        };
        return false;
    }
    if (context->iteration < context->warmups + context->iterations) {
        return true;
    }
    context->benchResult->iterations = context->iterations;
    context->benchResult->error = SCU_ERROR_NONE;
    if (context->iterations > 0) {
        context->benchResult->wall = scu_bench_stats_from_samples(
            context->wallSamples,
            context->iterations
        );
        context->benchResult->cpu = scu_bench_stats_from_samples(
            context->cpuSamples,
            context->iterations
        );
        SCU_ARRAY_DELETE(context->wallSamples, context->iterations);
        SCU_ARRAY_DELETE(context->cpuSamples, context->iterations);
        context->wallSamples = nullptr;
        context->cpuSamples = nullptr;
    }
    else {
        context->benchResult->wall = (ScuBenchStats) { };
        context->benchResult->cpu = (ScuBenchStats) { };
    }
    return false;
}

SCU_API void scu_bench_context_advance(ScuBenchContext* context) {
    SCU_ASSERT(context);
    if (context->timingResult.error != SCU_ERROR_NONE) {
        SCU_ARRAY_DELETE(context->wallSamples, context->iterations);
        SCU_ARRAY_DELETE(context->cpuSamples, context->iterations);
        context->wallSamples = nullptr;
        context->cpuSamples = nullptr;
        context->error = SCU_ERROR_TIMING_FAILED;
        return;
    }
    if (context->iteration >= context->warmups) {
        isize sampleIdx = context->iteration - context->warmups;
        context->wallSamples[sampleIdx] = context->timingResult.wallNs;
        context->cpuSamples[sampleIdx] = context->timingResult.cpuNs;
    }
    context->iteration++;
}
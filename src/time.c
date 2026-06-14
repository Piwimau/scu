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
#include "scu/assert.h"
#include "scu/time.h"

/** @brief The number of nanoseconds per second. */
static constexpr i64 SCU_NANOS_PER_SEC = 1'000'000'000;

#ifdef _WIN32
    /** @brief The number of nanoseconds per `FILETIME` tick. */
    static constexpr i64 SCU_NANOS_PER_TICK = 100;

    /**
     * @brief Converts a specified `FILETIME` value to nanoseconds.
     *
     * @param[in] filetime The `FILETIME` value to convert.
     * @return The converted `FILETIME` value in nanoseconds.
     */
    static inline i64 scu_filetime_to_ns(const FILETIME* filetime) {
        SCU_ASSERT(filetime != nullptr);
        u64 low = filetime->dwLowDateTime;
        u64 high = filetime->dwHighDateTime;
        u64 ticks = (high << 32) | low;
        return (i64) (ticks * SCU_NANOS_PER_TICK);
    }
#else
    /** @brief Represents a time interval in seconds and nanoseconds. */
    typedef struct timespec ScuTimespec;

    /**
     * @brief Converts a specified `ScuTimespec` value to nanoseconds.
     *
     * @param[in] timespec The `ScuTimespec` value to convert.
     * @return The converted `ScuTimespec` value in nanoseconds.
     */
    static inline i64 scu_timespec_to_ns(const ScuTimespec* timespec) {
        SCU_ASSERT(timespec != nullptr);
        return timespec->tv_sec * SCU_NANOS_PER_SEC + timespec->tv_nsec;
    }
#endif

/**
 * @brief Returns the current wall time in nanoseconds.
 *
 * @return The current wall time in nanoseconds, or `-1` on failure.
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
 * @brief Returns the current process CPU time in nanoseconds.
 *
 * @return The current process CPU time in nanoseconds, or `-1` on failure.
 */
static inline i64 scu_process_cpu_ns() {
#ifdef _WIN32
    FILETIME creationTime, exitTime, kernelTime, userTime;
    WINBOOL success = GetProcessTimes(
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
 * @brief Returns the current thread CPU time in nanoseconds.
 *
 * @return The current thread CPU time in nanoseconds, or `-1` on failure.
 */
static inline i64 scu_thread_cpu_ns() {
#ifdef _WIN32
    FILETIME creationTime, exitTime, kernelTime, userTime;
    WINBOOL success = GetThreadTimes(
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
 * @brief Returns the current CPU time in nanoseconds.
 *
 * @param[in] timingMode The timing mode for measuring CPU time.
 * @return The current CPU time in nanoseconds, or `-1` on failure.
 */
static inline i64 scu_cpu_ns(ScuTimingMode timingMode) {
    return (timingMode == SCU_TIMING_MODE_PROCESS)
        ? scu_process_cpu_ns()
        : scu_thread_cpu_ns();
}

void scu_stopwatch_init(ScuStopwatch* stopwatch, ScuTimingMode timingMode) {
    SCU_ASSERT(stopwatch != nullptr);
    *stopwatch = (ScuStopwatch) { .timingMode = timingMode };
}

bool scu_stopwatch_start(ScuStopwatch* stopwatch) {
    SCU_ASSERT(stopwatch != nullptr);
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

bool scu_stopwatch_restart(ScuStopwatch* stopwatch) {
    SCU_ASSERT(stopwatch != nullptr);
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

bool scu_stopwatch_stop(ScuStopwatch* stopwatch) {
    SCU_ASSERT(stopwatch != nullptr);
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

void scu_stopwatch_reset(ScuStopwatch* stopwatch) {
    SCU_ASSERT(stopwatch != nullptr);
    *stopwatch = (ScuStopwatch) { .timingMode = stopwatch->timingMode };
}

bool scu_stopwatch_is_running(const ScuStopwatch* stopwatch) {
    SCU_ASSERT(stopwatch != nullptr);
    return stopwatch->isRunning;
}

ScuTimingResult scu_stopwatch_elapsed(const ScuStopwatch* stopwatch) {
    SCU_ASSERT(stopwatch != nullptr);
    if (!stopwatch->isRunning) {
        return (ScuTimingResult) {
            .wallNs = stopwatch->accWallNs,
            .cpuNs = stopwatch->accCpuNs,
            .error = SCU_ERROR_NONE
        };
    }
    i64 wallNs = scu_wall_ns();
    i64 cpuNs = scu_cpu_ns(stopwatch->timingMode);
    if ((wallNs == -1) || (cpuNs == -1)) {
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
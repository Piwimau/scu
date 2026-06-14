#define SCU_SHORT_ALIASES

#include <tgmath.h>
#include "scu/alloc.h"
#include "scu/array.h"
#include "scu/assert.h"
#include "scu/bench.h"
#include "scu/compare.h"
#include "scu/memory.h"

[[nodiscard]]
ScuBenchCtx scu_bench_ctx_new(
    ScuTimingMode timingMode,
    isize warmup,
    isize iterations,
    ScuBenchResult* benchResult
) {
    SCU_ASSERT(warmup >= 0);
    SCU_ASSERT(iterations >= 0);
    SCU_ASSERT(benchResult != nullptr);
    ScuBenchCtx ctx = {
        .timingMode = timingMode,
        .warmup = warmup,
        .iterations = iterations,
        .benchResult = benchResult
    };
    if (ctx.iterations > 0) {
        ctx.wallSamples = scu_malloc(ctx.iterations * SCU_SIZEOF(i64));
        ctx.cpuSamples = scu_malloc(ctx.iterations * SCU_SIZEOF(i64));
        if ((ctx.wallSamples == nullptr) || (ctx.cpuSamples == nullptr)) {
            scu_free(ctx.wallSamples);
            ctx.wallSamples = nullptr;
            scu_free(ctx.cpuSamples);
            ctx.cpuSamples = nullptr;
            ctx.error = SCU_ERROR_OUT_OF_MEMORY;
        }
    }
    return ctx;
}

/**
 * @brief Computes benchmark statistics from an array of samples.
 *
 * @note This function sorts the specified array of samples in-place.
 *
 * @param[in, out] samples An array of samples (in nanoseconds).
 * @param[in]      count   The number of samples in the array.
 * @return The benchmark statistics for the specified array of samples.
 */
static inline ScuBenchStats scu_bench_stats_from_samples(
    i64* samples,
    isize count
) {
    SCU_ASSERT(samples != nullptr);
    SCU_ASSERT(count > 0);
    scu_array_sort(samples, count, SCU_SIZEOF(i64), scu_compare_i64);
    f64 sumNs = 0.0;
    for (isize i = 0; i < count; i++) {
        sumNs += (f64) samples[i];
    }
    f64 meanNs = sumNs / (f64) count;
    f64 sumOfSquaredDiffsNs = 0.0;
    for (isize i = 0; i < count; i++) {
        f64 diff = (f64) samples[i] - meanNs;
        sumOfSquaredDiffsNs += diff * diff;
    }
    f64 medianNs = ((count % 2) == 0)
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

bool scu_bench_ctx_is_running(ScuBenchCtx* ctx) {
    SCU_ASSERT(ctx != nullptr);
    if (ctx->error != SCU_ERROR_NONE) {
        *ctx->benchResult = (ScuBenchResult) {
            .iterations = ctx->iteration,
            .error = ctx->error
        };
        return false;
    }
    if (ctx->iteration < (ctx->warmup + ctx->iterations)) {
        return true;
    }
    ctx->benchResult->iterations = ctx->iterations;
    ctx->benchResult->error = SCU_ERROR_NONE;
    if (ctx->iterations > 0) {
        ctx->benchResult->wall = scu_bench_stats_from_samples(
            ctx->wallSamples,
            ctx->iterations
        );
        scu_free(ctx->wallSamples);
        ctx->wallSamples = nullptr;
        ctx->benchResult->cpu = scu_bench_stats_from_samples(
            ctx->cpuSamples,
            ctx->iterations
        );
        scu_free(ctx->cpuSamples);
        ctx->cpuSamples = nullptr;
    }
    else {
        ctx->benchResult->wall = (ScuBenchStats) { };
        ctx->benchResult->cpu = (ScuBenchStats) { };
    }
    return false;
}

void scu_bench_ctx_advance(ScuBenchCtx* ctx) {
    SCU_ASSERT(ctx != nullptr);
    if (ctx->timingResult.error != SCU_ERROR_NONE) {
        scu_free(ctx->wallSamples);
        ctx->wallSamples = nullptr;
        scu_free(ctx->cpuSamples);
        ctx->cpuSamples = nullptr;
        ctx->error = SCU_ERROR_TIMING_FAILED;
        return;
    }
    if (ctx->iteration >= ctx->warmup) {
        isize sampleIndex = ctx->iteration - ctx->warmup;
        ctx->wallSamples[sampleIndex] = ctx->timingResult.wallNs;
        ctx->cpuSamples[sampleIndex] = ctx->timingResult.cpuNs;
    }
    ctx->iteration++;
}
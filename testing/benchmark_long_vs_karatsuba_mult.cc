#include "../WMPInt.hh"
#include "benchmark_timer.hh"
#include <random>
#include <cassert>

namespace
{
    struct SizeData { std::size_t lhsSize, rhsSize, iterations; };

    constexpr SizeData kSizes[] =
    {
        { 12, 12, 4000000 },
        { 16, 16, 2500000 },
        { 14, 16, 2500000 },
        { 10, 16, 2500000 },
        { 25, 25, 900000 },
        { 20, 25, 900000 },
        { 15, 25, 900000 },
        { 10, 25, 900000 },
        { 50, 50, 220000 },
        { 40, 50, 220000 },
        { 30, 50, 220000 },
        { 20, 50, 220000 },
        { 10, 50, 220000 },
        { 100, 100, 52000 },
        { 50, 100, 52000 },
        { 25, 100, 52000 },
        { 10, 100, 52000 },
        { 200, 200, 14000 },
        { 150, 200, 14000 },
        { 100, 200, 14000 },
        { 50, 200, 14000 },
        { 20, 200, 14000 },
        { 300, 300, 6000 },
        { 400, 400, 3500 },
        { 1000, 1000, 550 },
    };

    constexpr std::size_t kSizesAmount = sizeof(kSizes)/sizeof(*kSizes);
    constexpr std::size_t kMaxLHSSize = kSizes[kSizesAmount-1].lhsSize;
    constexpr std::size_t kMaxRHSSize = kSizes[kSizesAmount-1].rhsSize;
    constexpr std::size_t kResultMaxSize = kMaxLHSSize + kMaxRHSSize;
    constexpr std::size_t kTempBufferMaxSize =
        WMPIntImplementations::fullKaratsubaMultiplicationBufferSize(kMaxLHSSize, kMaxRHSSize);

    std::mt19937_64 rngEngine(0);
    volatile std::uint64_t gValueSink1;
    std::uint64_t gLHS[kMaxLHSSize], gRHS[kMaxRHSSize];
    std::uint64_t gResult[kResultMaxSize];
    std::uint64_t gTempBuffer[kTempBufferMaxSize];
}

void runMultiplicationBenchmark
(std::size_t lhsSize, std::size_t rhsSize, std::size_t totalIterations)
{
    assert(lhsSize <= rhsSize);
    std::uint64_t resultSumMSW = 0;

    for(std::size_t i = 0; i < lhsSize; ++i)
        gLHS[i] = rngEngine();

    for(std::size_t i = 0; i < rhsSize; ++i)
        gRHS[i] = rngEngine();

    Timer timer1;

    for(std::size_t i = 0; i < totalIterations; ++i)
    {
        WMPIntImplementations::doFullLongMultiplication
            (gLHS, lhsSize, gRHS, rhsSize, gResult, gTempBuffer);
        resultSumMSW += gResult[0];
    }

    gValueSink1 = resultSumMSW;
    const double seconds1 = timer1.getElapsedSeconds();

    Timer timer2;

    for(std::size_t i = 0; i < totalIterations; ++i)
    {
        WMPIntImplementations::doFullKaratsubaMultiplication
            (gLHS, lhsSize, gRHS, rhsSize, gResult, gTempBuffer);
        resultSumMSW += gResult[0];
    }

    gValueSink1 = resultSumMSW;
    const double seconds2 = timer2.getElapsedSeconds();

    std::printf("lhs:%zu, rhs:%zu -> long: %.3f s (", lhsSize, rhsSize, seconds1);
    Timer::printTime(seconds1 / totalIterations);
    std::printf("), karatsuba: %.3f s (", seconds2);
    Timer::printTime(seconds2 / totalIterations);
    std::printf(") %c %zu/%zu\n", seconds1<seconds2?'<':'>',
                WMPIntImplementations::fullLongMultiplicationBufferSize(lhsSize, rhsSize),
                WMPIntImplementations::fullKaratsubaMultiplicationBufferSize(lhsSize, rhsSize));
}

int main()
{
    std::printf("Long multiplication vs. Karatsuba algorithm benchmarks:\n");
    for(const SizeData& data: kSizes)
        runMultiplicationBenchmark(data.lhsSize, data.rhsSize, data.iterations);
}

#include "../WMPInt.hh"
#include "benchmark_timer.hh"
#include <random>
#include <cassert>

namespace
{
    struct SizeData { std::size_t lhsSize, rhsSize, iterations; };

    constexpr SizeData kSizes[] =
    {
        { 12, 12, 2000000 },
        { 16, 16, 1200000 },
        { 14, 16, 1100000 },
        { 10, 16, 1700000 },
        { 8, 16, 2000000 },
        { 5, 16, 3000000 },
        { 25, 25, 450000 },
        { 20, 25, 600000 },
        { 15, 25, 700000 },
        { 10, 25, 1000000 },
        { 5, 25, 2000000 },
        { 30, 30, 300000 },
        { 25, 30, 400000 },
        { 20, 30, 450000 },
        { 15, 30, 700000 },
        { 10, 30, 1000000 },
        { 5, 30, 2000000 },
        { 50, 50, 110000 },
        { 40, 50, 140000 },
        { 30, 50, 180000 },
        { 20, 50, 250000 },
        { 15, 50, 320000 },
        { 10, 50, 450000 },
        { 100, 100, 25000 },
        { 50, 100, 52000 },
        { 25, 100, 100000 },
        { 18, 100, 160000 },
        { 10, 100, 230000 },
        { 200, 200, 7000 },
        { 150, 200, 10000 },
        { 100, 200, 14000 },
        { 50, 200, 25000 },
        { 20, 200, 65000 },
        { 10, 200, 130000 },
        { 300, 300, 3000 },
        { 200, 300, 4000 },
        { 150, 300, 6000 },
        { 100, 300, 8000 },
        { 50, 300, 17000 },
        { 25, 300, 35000 },
    };

    constexpr std::size_t getMaxLHSSize()
    {
        std::size_t maxSize = 0;
        for(const SizeData& data: kSizes)
            if(data.lhsSize > maxSize) maxSize = data.lhsSize;
        return maxSize;
    }

    constexpr std::size_t getMaxRHSSize()
    {
        std::size_t maxSize = 0;
        for(const SizeData& data: kSizes)
            if(data.rhsSize > maxSize) maxSize = data.rhsSize;
        return maxSize;
    }

    constexpr std::size_t kMaxLHSSize = getMaxLHSSize();
    constexpr std::size_t kMaxRHSSize = getMaxRHSSize();
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

    std::printf("lhs:%2zu, rhs:%2zu %c long: %.3f s (", lhsSize, rhsSize,
                seconds1<seconds2?'-':'+', seconds1);
    Timer::printTime(seconds1 / totalIterations);
    std::printf("), karatsuba: %.3f s (", seconds2);
    Timer::printTime(seconds2 / totalIterations);
    std::printf(") | %zu/%zu\n",
                WMPIntImplementations::fullLongMultiplicationBufferSize(lhsSize, rhsSize),
                WMPIntImplementations::fullKaratsubaMultiplicationBufferSize(lhsSize, rhsSize));
}

int main()
{
    std::printf("Long multiplication vs. Karatsuba algorithm benchmarks:\n");
    for(const SizeData& data: kSizes)
        runMultiplicationBenchmark(data.lhsSize, data.rhsSize, data.iterations);
}

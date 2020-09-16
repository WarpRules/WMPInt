#include "../WMPInt.hh"
#include "benchmark_timer.hh"
#include "WFLCG.hh"
#include <memory>
#include <cassert>

namespace
{
    WFLCG gRNG;
}

static void reseedAndInitialize(std::uint64_t* lhs, std::size_t lhsSize,
                                std::uint64_t* rhs, std::size_t rhsSize)
{
    gRNG = WFLCG(static_cast<std::uint32_t>(lhsSize), static_cast<std::uint32_t>(rhsSize));

    for(std::size_t i = 0; i < lhsSize; ++i)
        lhs[i] = static_cast<std::uint64_t>(gRNG()) | ( static_cast<std::uint64_t>(gRNG()) << 32);

    for(std::size_t i = 0; i < rhsSize; ++i)
        rhs[i] = static_cast<std::uint64_t>(gRNG()) | ( static_cast<std::uint64_t>(gRNG()) << 32);
}

void runMultiplicationBenchmark
(std::size_t lhsSize, std::size_t rhsSize, std::size_t totalIterations)
{
    assert(lhsSize <= rhsSize);

    const std::size_t resultSize = lhsSize + rhsSize;
    const std::size_t longMultBuffersize =
        WMPIntImplementations::fullLongMultiplicationBufferSize(rhsSize, lhsSize);
    const std::size_t karatsubaMultBufferSize =
        WMPIntImplementations::fullKaratsubaMultiplicationBufferSize(lhsSize, rhsSize);

    std::unique_ptr<std::uint64_t[]> lhs(new std::uint64_t[lhsSize]);
    std::unique_ptr<std::uint64_t[]> rhs(new std::uint64_t[rhsSize]);
    std::unique_ptr<std::uint64_t[]> result(new std::uint64_t[resultSize]);

    std::unique_ptr<std::uint64_t[]> longMultTempBuffer
        (new std::uint64_t[longMultBuffersize]);
    std::unique_ptr<std::uint64_t[]> karatsubaMultTempBuffer
        (new std::uint64_t[karatsubaMultBufferSize]);

    reseedAndInitialize(lhs.get(), lhsSize, rhs.get(), rhsSize);
    Timer timer1;

    for(std::size_t iteration = 0; iteration < totalIterations; ++iteration)
    {
        for(unsigned subIteration = 0; subIteration < WFLCG::kBufferSize; ++subIteration)
        {
            WMPIntImplementations::doFullLongMultiplication
                (rhs.get(), rhsSize, lhs.get(), lhsSize, result.get(), longMultTempBuffer.get());

            lhs.get()[0] ^= gRNG.buffer()[subIteration];
        }
        gRNG.refillBuffer();
    }

    const double seconds1 = timer1.getElapsedSeconds();

    reseedAndInitialize(lhs.get(), lhsSize, rhs.get(), rhsSize);
    Timer timer2;

    for(std::size_t i = 0; i < totalIterations; ++i)
    {
        for(unsigned subIteration = 0; subIteration < WFLCG::kBufferSize; ++subIteration)
        {
            WMPIntImplementations::doFullKaratsubaMultiplication
                (lhs.get(), lhsSize, rhs.get(), rhsSize, result.get(),
                 karatsubaMultTempBuffer.get());

            lhs.get()[0] ^= gRNG.buffer()[subIteration];
        }
        gRNG.refillBuffer();
    }

    const double seconds2 = timer2.getElapsedSeconds();

    totalIterations *= WFLCG::kBufferSize;

    std::printf("%zux%zu (lbs:%zu, kbs:%zu) %c long: %.3fs (", lhsSize, rhsSize,
                longMultBuffersize, karatsubaMultBufferSize,
                seconds1<seconds2?'-':'+', seconds1);
    Timer::printTime(seconds1 / totalIterations);
    std::printf("), karatsuba: %.3fs (", seconds2);
    Timer::printTime(seconds2 / totalIterations);
    std::printf(")\n");
}

int main()
{
    std::printf("Long multiplication vs. Karatsuba algorithm benchmarks:\n");

    for(std::size_t rhsSize: { 16, 24, 32, 48, 64, 96, 128, 192, 256, 384, 512, 768, 1024, 2048 })
    {
        std::printf("---\n");
        for(std::size_t lhsSize: { 8, 9, 16, 17, 24, 25, 32, 33, 64, 65, 128, 129, 256, 267,
                                      512, 513, 768, 769, 1024, 1025, 1620, 2048 })
            if(lhsSize <= rhsSize)
                runMultiplicationBenchmark(lhsSize, rhsSize, 30000000/(lhsSize*rhsSize));
    }
}

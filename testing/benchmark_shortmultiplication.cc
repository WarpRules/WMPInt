#include "../WMPInt.hh"
#include "benchmark_timer.hh"
#include <random>
#include <memory>

namespace
{
    std::mt19937_64 rngEngine(0);

    volatile std::uint64_t gValueSink1, gValueSink2;
}

template<std::size_t kSize>
void runMultiplicationBenchmark(std::size_t totalIterations)
{
    std::unique_ptr<WMPUInt<kSize>> value1(new WMPUInt<kSize>);
    std::uint64_t resultSumMSW = 0, resultSumLSW = 0;

    for(std::size_t i = 0; i < kSize; ++i)
    {
        value1->data()[i] = rngEngine();
    }

    Timer timer;

    std::uint64_t rhs = rngEngine();
    for(std::size_t i = 0; i < totalIterations; ++i)
    {
        *value1 *= rhs;
        resultSumMSW += value1->data()[0];
        resultSumLSW += value1->data()[kSize-1];
        rhs += 3;
    }

    gValueSink1 = resultSumMSW; gValueSink2 = resultSumLSW;
    timer.printResult(kSize, totalIterations);
}

int main()
{
    std::printf("Benchmarks for WMPUInt::operator*=(std::uint64_t):\n");

    runMultiplicationBenchmark<2>(400000000);
    runMultiplicationBenchmark<3>(250000000);
    runMultiplicationBenchmark<4>(200000000);
    runMultiplicationBenchmark<5>(180000000);
    runMultiplicationBenchmark<6>(150000000);
    runMultiplicationBenchmark<7>(120000000);
    runMultiplicationBenchmark<8>(100000000);
    runMultiplicationBenchmark<10>(80000000);
    runMultiplicationBenchmark<16>(50000000);
    runMultiplicationBenchmark<24>(35000000);
    runMultiplicationBenchmark<32>(25000000);
    runMultiplicationBenchmark<50>(15000000);
    runMultiplicationBenchmark<150>(5000000);
    runMultiplicationBenchmark<256>(3000000);
    runMultiplicationBenchmark<1024>(800000);
    runMultiplicationBenchmark<1024*16>(50000);
    runMultiplicationBenchmark<1024*64>(12000);
}

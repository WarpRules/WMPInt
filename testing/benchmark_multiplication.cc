#include "../WMPInt.hh"
#include "benchmark_timer.hh"
#include <random>

namespace
{
    volatile std::uint64_t gValueSink1, gValueSink2;
}

template<std::size_t kSize>
void runMultiplicationBenchmark(std::size_t totalIterations)
{
    std::mt19937_64 rngEngine(totalIterations);
    WMPUInt<kSize> value1, value2, result;
    std::uint64_t tempBuffer[kSize];
    std::uint64_t resultSumMSW = 0, resultSumLSW = 0;

    for(std::size_t i = 0; i < kSize; ++i)
    {
        value1.data()[i] = rngEngine();
        value2.data()[i] = rngEngine();
    }

    Timer timer;

    for(std::size_t i = 0; i < totalIterations; ++i)
    {
        value1.multiply(value2, result.data(), tempBuffer);
        resultSumMSW += result.data()[0];
        resultSumLSW += result.data()[kSize-1];
    }

    gValueSink1 = resultSumMSW; gValueSink2 = resultSumLSW;
    timer.printResult(kSize, totalIterations);
}

int main()
{
    std::printf("Benchmarks for WMPUInt::multiply():\n");

    runMultiplicationBenchmark<2>(540000000);
    runMultiplicationBenchmark<3>(68000000);
    runMultiplicationBenchmark<4>(40000000);
    runMultiplicationBenchmark<5>(25000000);
    runMultiplicationBenchmark<6>(18000000);
    runMultiplicationBenchmark<7>(13000000);
    runMultiplicationBenchmark<8>(9500000);
    runMultiplicationBenchmark<10>(6500000);
    runMultiplicationBenchmark<16>(2500000);
    runMultiplicationBenchmark<24>(1000000);
    runMultiplicationBenchmark<32>(520000);
    runMultiplicationBenchmark<50>(220000);
    runMultiplicationBenchmark<150>(22000);
    runMultiplicationBenchmark<256>(7800);
    runMultiplicationBenchmark<1024>(500);
}

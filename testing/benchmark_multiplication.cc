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
    std::unique_ptr<WMPUInt<kSize>> value1(new WMPUInt<kSize>), value2(new WMPUInt<kSize>), result(new WMPUInt<kSize>);
    std::unique_ptr<std::uint64_t[]> tempBuffer(new std::uint64_t[value1->multiplyBufferSize()]);
    std::uint64_t resultSumMSW = 0, resultSumLSW = 0;

    for(std::size_t i = 0; i < kSize; ++i)
    {
        value1->data()[i] = rngEngine();
        value2->data()[i] = rngEngine();
    }

    Timer timer;

    for(std::size_t i = 0; i < totalIterations; ++i)
    {
        value1->multiply(*value2, *result, tempBuffer.get());
        resultSumMSW += result->data()[0];
        resultSumLSW += result->data()[kSize-1];
    }

    gValueSink1 = resultSumMSW; gValueSink2 = resultSumLSW;
    timer.printResult(kSize, totalIterations);
}

int main()
{
    std::printf("Benchmarks for WMPUInt::multiply():\n");

    runMultiplicationBenchmark<2>(1050000000);
    runMultiplicationBenchmark<3>(180000000);
    runMultiplicationBenchmark<4>(100000000);
    runMultiplicationBenchmark<5>(80000000);
    runMultiplicationBenchmark<6>(55000000);
    runMultiplicationBenchmark<7>(40000000);
    runMultiplicationBenchmark<8>(35000000);
    runMultiplicationBenchmark<10>(20000000);
    runMultiplicationBenchmark<16>(9000000);
    runMultiplicationBenchmark<24>(3500000);
    runMultiplicationBenchmark<32>(2000000);
    runMultiplicationBenchmark<50>(800000);
    runMultiplicationBenchmark<150>(120000);
    runMultiplicationBenchmark<256>(45000);
    runMultiplicationBenchmark<1024>(5000);
    runMultiplicationBenchmark<1024*16>(50);
    runMultiplicationBenchmark<1024*64>(5);
}

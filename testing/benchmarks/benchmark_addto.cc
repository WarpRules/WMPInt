#include "../WMPInt.hh"
#include "benchmark_timer.hh"
#include <random>

namespace
{
    std::mt19937_64 rngEngine(0);

    volatile std::uint64_t gValueSink1, gValueSink2;
}

template<std::size_t kSize>
void runAddToBenchmark(std::size_t totalIterations)
{
    WMPUInt<kSize> sum1(0), sum2(1), value;

    for(std::size_t i = 0; i < kSize; ++i)
        value.data()[i] = rngEngine();

    Timer timer1;

    for(std::size_t i = 0; i < totalIterations; ++i)
    {
        sum1 += value;
        sum2 += value;
    }

    const double seconds1 = timer1.getElapsedSeconds();

    gValueSink1 = sum1.data()[0]; gValueSink2 = sum1.data()[kSize-1];
    gValueSink1 = sum2.data()[0]; gValueSink2 = sum2.data()[kSize-1];

    sum1.assign(0);
    sum2.assign(1);

    Timer timer2;

    for(std::size_t i = 0; i < totalIterations; ++i)
        value.addTo(sum1, sum2);

    const double seconds2 = timer2.getElapsedSeconds();

    gValueSink1 = sum1.data()[0]; gValueSink2 = sum1.data()[kSize-1];
    gValueSink1 = sum2.data()[0]; gValueSink2 = sum2.data()[kSize-1];

    std::printf("WMPUInt<%zu>: %.3f vs. %.3f s (%zu iterations, %.3f vs. %.3f us)\n",
                kSize, seconds1, seconds2, totalIterations,
                seconds1 * 1.0e6 / totalIterations, seconds2 * 1.0e6 / totalIterations);
}

int main()
{
    std::printf("Benchmarks for 2x operator+=() vs. WMPUInt::addto():\n");

    runAddToBenchmark<2>(300000000);
    runAddToBenchmark<3>(250000000);
    runAddToBenchmark<4>(250000000);
    runAddToBenchmark<5>(130000000);
    runAddToBenchmark<6>(100000000);
    runAddToBenchmark<7>(90000000);
    runAddToBenchmark<8>(75000000);
    runAddToBenchmark<10>(63000000);
    runAddToBenchmark<16>(45000000);
    runAddToBenchmark<24>(30000000);
    runAddToBenchmark<32>(22000000);
    runAddToBenchmark<50>(15000000);
    runAddToBenchmark<150>(5000000);
    runAddToBenchmark<256>(3000000);
    runAddToBenchmark<1024>(750000);
    //runAddToBenchmark<1024*8>(750000);
}

#include <chrono>
#include <cstdio>
#include <cstdint>

class Timer
{
    std::chrono::time_point<std::chrono::high_resolution_clock> mStartTime =
        std::chrono::high_resolution_clock::now();


 public:
    double getElapsedSeconds() const
    {
        const std::chrono::time_point endTime = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> diff = endTime - mStartTime;
        return diff.count();
    }

    void printResult(std::size_t kSize, std::size_t iterationCount)
    {
        const double seconds = getElapsedSeconds();
        std::printf("WMPUInt<%zu>: %.3f s (%zu iterations, %.3f us)\n",
                    kSize, seconds, iterationCount, seconds * 1.0e6 / iterationCount);
    }
};

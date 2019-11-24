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

    static void printTime(double seconds)
    {
        if(seconds < 1.0e-6)
            std::printf("%.2f ns", seconds * 1.0e9);
        else if(seconds < 1.0e-3)
            std::printf("%.2f us", seconds * 1.0e6);
        else if(seconds < 1.0)
            std::printf("%.2f ms", seconds * 1.0e3);
        else
            std::printf("%.2f s", seconds);
    }

    void printResult(std::size_t kSize, std::size_t iterationCount)
    {
        const double seconds = getElapsedSeconds();
        std::printf("WMPUInt<%zu>: %.3f s (%zu iterations, ", kSize, seconds, iterationCount);
        printTime(seconds / iterationCount);
        std::printf(")\n");
    }
};

#pragma once

#include <chrono>

namespace URay
{

class Timer
{
public:
    Timer();
    ~Timer() = default;

public:
    using Clock = std::chrono::steady_clock;

    void Tick();

    double GetDeltaTime() const
    {
        return deltaTime;
    }
    double GetElapsedTime() const
    {
        return elapsedTime;
    }

    int GetFPS() const
    {
        return fps;
    }

private:
    Clock::time_point previousTime;

    double deltaTime = 0.0;
    double elapsedTime = 0.0;

    double sampleTime = 0.0;
    int sampleFrames = 0;
    int fps = 0;
};

} // namespace URay

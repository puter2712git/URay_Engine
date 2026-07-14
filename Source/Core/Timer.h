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

private:
    Clock::time_point previousTime;

    double deltaTime = 0.0f;
    double elapsedTime = 0.0f;
};

} // namespace URay

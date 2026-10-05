#include "Timer.h"

namespace URay
{

Timer::Timer()
{
    previousTime = Clock::now();
}

void Timer::Tick()
{
    const auto currentTime = Clock::now();
    const auto elapsed = currentTime - previousTime;

    previousTime = currentTime;

    deltaTime = std::chrono::duration<double>(elapsed).count();
    elapsedTime += deltaTime;

    sampleTime += deltaTime;
    ++sampleFrames;

    if (sampleTime >= 0.5)
    {
        fps = static_cast<int>(sampleFrames / sampleTime);
        sampleTime = 0.0;
        sampleFrames = 0;
    }
}

} // namespace URay

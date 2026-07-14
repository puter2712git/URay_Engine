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
}

} // namespace URay

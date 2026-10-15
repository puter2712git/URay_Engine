#include "PerformanceAnalytics.h"

namespace URay
{

void PerformanceAnalytics::BeginFrame()
{
    frameBeginTime = std::chrono::steady_clock::now();
    currSamples.clear();
}

void PerformanceAnalytics::EndFrame()
{
    completedSamples = std::move(currSamples);
    ++completedFrameNumber;
}

ScopeBeginInfo PerformanceAnalytics::BeginScope()
{
    const auto now = std::chrono::steady_clock::now();

    const double startMs = std::chrono::duration<double, std::milli>(now - frameBeginTime).count();

    return ScopeBeginInfo{
        .startMs = startMs,
        .depth = activeScopeDepth++
    };
}

void PerformanceAnalytics::EndScope()
{
    --activeScopeDepth;
}

void PerformanceAnalytics::AddSample(const ScopeSample& sample)
{
    currSamples.push_back(sample);
}

} // namespace URay

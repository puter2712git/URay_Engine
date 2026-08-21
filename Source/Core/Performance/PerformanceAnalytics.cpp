#include "PerformanceAnalytics.h"

namespace URay
{

void PerformanceAnalytics::BeginFrame()
{
    currSamples.clear();
}

void PerformanceAnalytics::EndFrame()
{
    completedSamples = std::move(currSamples);
}

void PerformanceAnalytics::AddSample(const ScopeSample& sample)
{
    currSamples.push_back(sample);
}

} // namespace URay

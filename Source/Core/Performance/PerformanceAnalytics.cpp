#include "PerformanceAnalytics.h"

namespace URay
{

void PerformanceAnalytics::Reset()
{
    samples.clear();
}

void PerformanceAnalytics::AddSample(const ScopeSample& sample)
{
    samples.push_back(sample);
}

} // namespace URay

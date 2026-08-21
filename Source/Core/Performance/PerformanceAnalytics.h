#pragma once

#include <string>
#include <vector>

namespace URay
{

struct ScopeSample
{
    std::string_view name;
    double durationMs;
};

class PerformanceAnalytics
{
public:
    void Reset();

    void AddSample(const ScopeSample& sample);

    const std::vector<ScopeSample>& GetSamples() const { return samples; }

private:
    std::vector<ScopeSample> samples;
};

} // namespace URay

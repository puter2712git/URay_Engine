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
    void BeginFrame();
    void EndFrame();

    void AddSample(const ScopeSample& sample);

    const std::vector<ScopeSample>& GetCompletedSamples() const { return completedSamples; }

private:
    std::vector<ScopeSample> currSamples;
    std::vector<ScopeSample> completedSamples;
};

} // namespace URay

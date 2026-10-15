#pragma once

#include <Core/Type/Types.h>

#include <chrono>
#include <string>
#include <vector>

namespace URay
{

struct ScopeBeginInfo
{
    double startMs = 0.0;
    uint32 depth = 0;
};

struct ScopeSample
{
    std::string_view name;
    double startMs;
    double durationMs;
    uint32 depth;
};

class PerformanceAnalytics
{
public:
    void BeginFrame();
    void EndFrame();

    ScopeBeginInfo BeginScope();
    void EndScope();

    void AddSample(const ScopeSample& sample);

    uint64 GetCompletedFrameNumber() const { return completedFrameNumber; }

    const std::vector<ScopeSample>& GetCompletedSamples() const { return completedSamples; }

private:
    std::chrono::steady_clock::time_point frameBeginTime;
    uint32 activeScopeDepth = 0;
    uint64 completedFrameNumber = 0;

    std::vector<ScopeSample> currSamples;
    std::vector<ScopeSample> completedSamples;
};

} // namespace URay

#pragma once

#include <Core/Type/TypeAlias.h>

#include <chrono>

namespace URay
{

struct ScopeBeginInfo
{
    double startMs = 0.0;
    uint32 depth = 0;
};

struct ScopeSample
{
    StringView name;
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

    const Vector<ScopeSample>& GetCompletedSamples() const { return completedSamples; }

private:
    std::chrono::steady_clock::time_point frameBeginTime;
    uint32 activeScopeDepth = 0;
    uint64 completedFrameNumber = 0;

    Vector<ScopeSample> currSamples;
    Vector<ScopeSample> completedSamples;
};

} // namespace URay

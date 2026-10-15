#pragma once

#include <Core/Performance/PerformanceAnalytics.h>
#include <Core/Type/Types.h>

#include <deque>
#include <string>
#include <vector>

namespace URay
{

struct ProfileScopeRow
{
    std::string name;
    double startMs = 0.0;
    double durationMs = 0.0;
    double selfMs = 0.0;
    uint32 depth = 0;
};

struct ProfileFrame
{
    uint64 frameNumber = 0;
    double capturedCpuMs = 0.0;
    std::vector<ProfileScopeRow> scopes;
};

struct ProfileScopeStatistics
{
    std::string name;
    uint32 sampleCount = 0;

    double latestMs = 0.0;
    double averageMs = 0.0;
    double maxMs = 0.0;
    double p95Ms = 0.0;
};

class PerformanceViewModel
{
public:
    explicit PerformanceViewModel(size_t historyCapacity = 120);

public:
    void AddCompletedFrame(uint64 frameNumber, const std::vector<ScopeSample>& samples);

    const ProfileFrame* GetLatestFrame() const { return frames.empty() ? nullptr : &frames.back(); }
    const std::deque<ProfileFrame>& GetFrames() const { return frames; }
    const std::vector<ProfileScopeStatistics>& GetStatistics() const { return statistics; }

private:
    void RebuildStatistics();

private:
    size_t historyCapacity = 120;
    std::deque<ProfileFrame> frames;
    std::vector<ProfileScopeStatistics> statistics;
};

} // namespace URay

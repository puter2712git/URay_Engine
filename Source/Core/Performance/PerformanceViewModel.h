#pragma once

#include <Core/Performance/PerformanceAnalytics.h>
#include <Core/Type/Types.h>

#include <array>
#include <deque>
#include <string>
#include <vector>

namespace URay
{

enum class ProfileDomain : uint8
{
    CPU,
    GPU,
    Count
};

struct ProfileSample
{
    std::string name;
    double startMs = 0.0;
    double durationMs = 0.0;
    uint32 depth = 0;
};

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
    double capturedMs = 0.0;
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

struct ProfileChannel
{
    std::deque<ProfileFrame> frames;
    std::vector<ProfileScopeStatistics> statistics;
};

class PerformanceViewModel
{
public:
    explicit PerformanceViewModel(size_t historyCapacity = 120);

public:
    void AddCPUFrame(uint64 frameNumber, const std::vector<ScopeSample>& samples);
    void AddGPUFrame(uint64 submissionNumber, const std::vector<ProfileSample>& samples);

    const ProfileFrame* GetLatestFrame(ProfileDomain domain) const;
    const std::vector<ProfileScopeStatistics>& GetStatistics(ProfileDomain domain) const { return channels[static_cast<size_t>(domain)].statistics; }

private:
    void AddFrame(ProfileDomain domain, uint64 frameNumber, const std::vector<ProfileSample>& samples);

    void RebuildStatistics(ProfileChannel& channel);

private:
    size_t historyCapacity = 120;
    std::array<ProfileChannel, static_cast<size_t>(ProfileDomain::Count)> channels;
};

} // namespace URay

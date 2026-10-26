#include "PerformanceViewModel.h"
#include <Core/Type/TypeAlias.h>
#include <Core/Math/Math.h>

#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>

namespace URay
{

PerformanceViewModel::PerformanceViewModel(size_t historyCapacity)
    : historyCapacity(Math::Max<size_t>(1, historyCapacity)) {}

void PerformanceViewModel::AddCPUFrame(uint64 frameNumber, const std::vector<ScopeSample>& samples)
{
    std::vector<ProfileSample> profileSamples;
    profileSamples.reserve(samples.size());

    for (const ScopeSample& sample : samples)
    {
        profileSamples.push_back(ProfileSample{
            .name = String(sample.name),
            .startMs = sample.startMs,
            .durationMs = sample.durationMs,
            .depth = sample.depth });
    }

    AddFrame(ProfileDomain::CPU, frameNumber, profileSamples);
}

void PerformanceViewModel::AddGPUFrame(uint64 submissionNumber, const std::vector<ProfileSample>& samples)
{
    AddFrame(ProfileDomain::GPU, submissionNumber, samples);
}

const ProfileFrame* PerformanceViewModel::GetLatestFrame(ProfileDomain domain) const
{
    const ProfileChannel& channel = channels[static_cast<size_t>(domain)];
    return channel.frames.empty() ? nullptr : &channel.frames.back();
}

void PerformanceViewModel::AddFrame(
    ProfileDomain domain,
    uint64 frameNumber,
    const std::vector<ProfileSample>& samples)
{
    std::vector<ProfileSample> orderedSamples = samples;

    std::sort(orderedSamples.begin(), orderedSamples.end(),
              [](const ProfileSample& lhs, const ProfileSample& rhs)
              {
                  if (lhs.startMs != rhs.startMs)
                      return lhs.startMs < rhs.startMs;

                  return lhs.depth < rhs.depth;
              });

    ProfileFrame frame = {};
    frame.frameNumber = frameNumber;

    std::vector<size_t> scopeStack;
    std::vector<double> directChildDuration;

    for (const ProfileSample& sample : orderedSamples)
    {
        while (!scopeStack.empty() && frame.scopes[scopeStack.back()].depth >= sample.depth)
        {
            scopeStack.pop_back();
        }

        const size_t rowIndex = frame.scopes.size();

        frame.scopes.push_back(ProfileScopeRow{
            .name = sample.name,
            .startMs = sample.startMs,
            .durationMs = sample.durationMs,
            .selfMs = sample.durationMs,
            .depth = sample.depth });

        directChildDuration.push_back(0.0);

        frame.capturedMs = Math::Max(
            frame.capturedMs,
            sample.startMs + sample.durationMs);

        if (sample.depth > 0 && !scopeStack.empty())
        {
            const size_t parentIndex = scopeStack.back();

            if (frame.scopes[parentIndex].depth + 1 == sample.depth)
            {
                directChildDuration[parentIndex] += sample.durationMs;
            }
        }

        scopeStack.push_back(rowIndex);
    }

    for (size_t index = 0; index < frame.scopes.size(); ++index)
    {
        frame.scopes[index].selfMs = Math::Max(
            0.0,
            frame.scopes[index].durationMs - directChildDuration[index]);
    }

    ProfileChannel& channel = channels[static_cast<size_t>(domain)];
    channel.frames.push_back(std::move(frame));

    while (channel.frames.size() > historyCapacity)
    {
        channel.frames.pop_front();
    }

    RebuildStatistics(channel);
}

void PerformanceViewModel::RebuildStatistics(ProfileChannel& channel)
{
    struct ScopeValues
    {
        std::vector<double> durations;
        double latestMs = 0.0;
    };

    std::unordered_map<String, ScopeValues> valuesByName;

    for (const ProfileFrame& frame : channel.frames)
    {
        for (const ProfileScopeRow& scope : frame.scopes)
        {
            ScopeValues& values = valuesByName[scope.name];
            values.durations.push_back(scope.durationMs);

            if (&frame == &channel.frames.back())
            {
                values.latestMs += scope.durationMs;
            }
        }
    }

    channel.statistics.clear();
    channel.statistics.reserve(valuesByName.size());

    for (auto& [name, values] : valuesByName)
    {
        std::sort(values.durations.begin(), values.durations.end());

        const double totalMs = std::accumulate(
            values.durations.begin(),
            values.durations.end(),
            0.0);

        const size_t count = values.durations.size();
        const size_t p95Index = static_cast<size_t>(Math::Ceil(static_cast<double>(count) * 0.95)) - 1;

        channel.statistics.push_back(ProfileScopeStatistics{
            .name = std::move(name),
            .sampleCount = static_cast<uint32>(count),
            .latestMs = values.latestMs,
            .averageMs = totalMs / static_cast<double>(count),
            .maxMs = values.durations.back(),
            .p95Ms = values.durations[p95Index] });
    }

    std::sort(
        channel.statistics.begin(),
        channel.statistics.end(),
        [](const ProfileScopeStatistics& lhs, const ProfileScopeStatistics& rhs)
        {
            if (lhs.latestMs != rhs.latestMs)
                return lhs.latestMs > rhs.latestMs;

            return lhs.name < rhs.name;
        });
}

} // namespace URay

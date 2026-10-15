#include "PerformanceViewModel.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <string>
#include <unordered_map>

namespace URay
{

PerformanceViewModel::PerformanceViewModel(size_t historyCapacity)
    : historyCapacity(std::max<size_t>(1, historyCapacity)) {}

void PerformanceViewModel::AddCompletedFrame(uint64 frameNumber, const std::vector<ScopeSample>& samples)
{
    std::vector<ScopeSample> orderedSamples = samples;

    std::sort(orderedSamples.begin(), orderedSamples.end(),
              [](const ScopeSample& lhs, const ScopeSample& rhs)
              {
                  if (lhs.startMs != rhs.startMs)
                      return lhs.startMs < rhs.startMs;

                  return lhs.depth < rhs.depth;
              });

    ProfileFrame frame = {};
    frame.frameNumber = frameNumber;

    std::vector<size_t> scopeStack;
    std::vector<double> directChildDuration;

    for (const ScopeSample& sample : orderedSamples)
    {
        while (!scopeStack.empty() && frame.scopes[scopeStack.back()].depth >= sample.depth)
        {
            scopeStack.pop_back();
        }

        const size_t rowIndex = frame.scopes.size();

        frame.scopes.push_back(ProfileScopeRow{
            .name = std::string(sample.name),
            .startMs = sample.startMs,
            .durationMs = sample.durationMs,
            .selfMs = sample.durationMs,
            .depth = sample.depth });

        directChildDuration.push_back(0.0);

        frame.capturedCpuMs = std::max(
            frame.capturedCpuMs,
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
        frame.scopes[index].selfMs = std::max(
            0.0,
            frame.scopes[index].durationMs - directChildDuration[index]);
    }

    frames.push_back(std::move(frame));

    while (frames.size() > historyCapacity)
    {
        frames.pop_front();
    }

    RebuildStatistics();
}

void PerformanceViewModel::RebuildStatistics()
{
    struct ScopeValues
    {
        std::vector<double> durations;
        double latestMs = 0.0;
    };

    std::unordered_map<std::string, ScopeValues> valuesByName;

    for (const ProfileFrame& frame : frames)
    {
        for (const ProfileScopeRow& scope : frame.scopes)
        {
            ScopeValues& values = valuesByName[scope.name];
            values.durations.push_back(scope.durationMs);

            if (&frame == &frames.back())
            {
                values.latestMs += scope.durationMs;
            }
        }
    }

    statistics.clear();
    statistics.reserve(valuesByName.size());

    for (auto& [name, values] : valuesByName)
    {
        std::sort(values.durations.begin(), values.durations.end());

        const double totalMs = std::accumulate(
            values.durations.begin(),
            values.durations.end(),
            0.0);

        const size_t count = values.durations.size();
        const size_t p95Index = static_cast<size_t>(std::ceil(static_cast<double>(count) * 0.95)) - 1;

        statistics.push_back(ProfileScopeStatistics{
            .name = std::move(name),
            .sampleCount = static_cast<uint32>(count),
            .latestMs = values.latestMs,
            .averageMs = totalMs / static_cast<double>(count),
            .maxMs = values.durations.back(),
            .p95Ms = values.durations[p95Index] });
    }

    std::sort(
        statistics.begin(),
        statistics.end(),
        [](const ProfileScopeStatistics& lhs, const ProfileScopeStatistics& rhs)
        {
            if (lhs.latestMs != rhs.latestMs)
                return lhs.latestMs > rhs.latestMs;

            return lhs.name < rhs.name;
        });
}

} // namespace URay

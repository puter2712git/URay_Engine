#include "ScopeTimer.h"
#include <Core/Type/Types.h>

#include <Core/Performance/PerformanceAnalytics.h>

namespace URay
{

ScopeTimer::ScopeTimer(PerformanceAnalytics& analytics, StringView name)
    : analytics(analytics),
      name(name),
      begin(std::chrono::steady_clock::now())
{
    const ScopeBeginInfo scope = analytics.BeginScope();
    startMs = scope.startMs;
    depth = scope.depth;
}

ScopeTimer::~ScopeTimer()
{
    const auto end = std::chrono::steady_clock::now();

    const double durationMs = std::chrono::duration<double, std::milli>(end - begin).count();

    analytics.AddSample(ScopeSample{
        .name = name,
        .startMs = startMs,
        .durationMs = durationMs,
        .depth = depth });

    analytics.EndScope();
}

} // namespace URay

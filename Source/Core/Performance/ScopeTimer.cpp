#include "ScopeTimer.h"

#include "Core/Performance/PerformanceAnalytics.h"

namespace URay
{

ScopeTimer::ScopeTimer(PerformanceAnalytics& analytics, std::string_view name)
    : analytics(analytics),
      name(name),
      begin(std::chrono::steady_clock::now())
{
}

ScopeTimer::~ScopeTimer()
{
    const auto end = std::chrono::steady_clock::now();

    const double durationMs = std::chrono::duration<double, std::milli>(end - begin).count();

    analytics.AddSample({
        .name = name,
        .durationMs = durationMs,
    });
}

} // namespace URay

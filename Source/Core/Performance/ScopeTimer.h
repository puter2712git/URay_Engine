#pragma once

#include <Core/Type/TypeAlias.h>

#include <chrono>
#include <string>

namespace URay
{

class PerformanceAnalytics;

class ScopeTimer
{
public:
    ScopeTimer(PerformanceAnalytics& analytics, StringView name);
    ~ScopeTimer();

private:
    PerformanceAnalytics& analytics;

    double startMs = 0.0;
    uint32 depth = 0;

    StringView name;
    std::chrono::steady_clock::time_point begin;
};

} // namespace URay

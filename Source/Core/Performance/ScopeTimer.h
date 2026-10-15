#pragma once

#include <Core/Type/Types.h>

#include <chrono>
#include <string>

namespace URay
{

class PerformanceAnalytics;

class ScopeTimer
{
public:
    ScopeTimer(PerformanceAnalytics& analytics, std::string_view name);
    ~ScopeTimer();

private:
    PerformanceAnalytics& analytics;

    double startMs = 0.0;
    uint32 depth = 0;

    std::string_view name;
    std::chrono::steady_clock::time_point begin;
};

} // namespace URay

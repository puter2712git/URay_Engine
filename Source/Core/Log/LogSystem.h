#pragma once

#include "Engine/Engine.h"

#include <cstdarg>
#include <vector>

namespace URay
{

class LogSink;

class LogSystem
{
public:
    void RegisterSink(LogSink* sink);
    void UnregisterSink(LogSink* sink);

    void Log(const char* format, ...);

private:
    void LogV(const char* format, va_list args);

private:
    std::vector<LogSink*> sinks;
};

#define URAY_LOG(...)                             \
    do                                            \
    {                                             \
        gEngine->GetLogSystem().Log(__VA_ARGS__); \
    } while (false)

} // namespace URay

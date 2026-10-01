#pragma once

#include "Engine/Engine.h"

#include <memory>
#include <string_view>
#include <vector>

namespace URay
{

class LogSink;

class LogSystem
{
public:
    void RegisterSink(LogSink* sink);
    void UnregisterSink(LogSink* sink);

    void Log(std::string_view msg);

private:
    std::vector<LogSink*> sinks;
};

#define URAY_LOG(msg)                               \
    LogSystem& logSystem = gEngine->GetLogSystem(); \
    logSystem.Log(msg);

} // namespace URay

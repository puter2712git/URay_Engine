#include "LogSystem.h"

#include "Core/Log/LogSink.h"

namespace URay
{

void LogSystem::RegisterSink(LogSink* sink)
{
    if (!sink)
        return;

    sinks.push_back(sink);
}

void LogSystem::UnregisterSink(LogSink* sink)
{
    std::erase(sinks, sink);
}

void LogSystem::Log(std::string_view msg)
{
    for (LogSink* sink : sinks)
    {
        sink->Write(msg);
    }
}

} // namespace URay

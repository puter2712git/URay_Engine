#include "Log.h"

#include "Core/Log/LogSink.h"

namespace URay
{

std::vector<ILogSink*> Logger::sinks;

void Logger::RegisterSink(ILogSink* sink)
{
    sinks.push_back(sink);
}

void Logger::UnregisterSink(ILogSink* sink)
{
    std::erase(sinks, sink);
}

void Logger::Log(const std::string& message)
{
    for (ILogSink* sink : sinks)
    {
        sink->Write(message);
    }
}

} // namespace URay

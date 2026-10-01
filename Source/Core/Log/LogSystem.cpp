#include "LogSystem.h"

#include "Core/Log/LogSink.h"

#include <cstdio>
#include <string>

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

void LogSystem::Log(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    LogV(format, args);
    va_end(args);
}

void LogSystem::LogV(const char* format, va_list args)
{
    va_list copiedArgs;
    va_copy(copiedArgs, args);

    const int length = std::vsnprintf(nullptr, 0, format, copiedArgs);
    va_end(copiedArgs);

    if (length < 0)
        return;

    std::string message(static_cast<size_t>(length), '\0');
    std::vsnprintf(message.data(), message.size() + 1, format, args);

    for (LogSink* sink : sinks)
    {
        sink->Write(message);
    }
}

} // namespace URay

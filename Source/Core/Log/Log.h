#pragma once

#include <string>
#include <vector>

namespace URay
{

class ILogSink;

class Logger
{
public:
    static void RegisterSink(ILogSink* sink);
    static void UnregisterSink(ILogSink* sink);

    static void Log(const std::string& message);

private:
    static std::vector<ILogSink*> sinks;
};

} // namespace URay

#pragma once

#include <string>

namespace URay
{

class ILogSink
{
public:
    virtual ~ILogSink() = default;

public:
    virtual void Write(const std::string& message) = 0;
};

} // namespace URay

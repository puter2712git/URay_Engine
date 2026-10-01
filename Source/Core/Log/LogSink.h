#pragma once

#include <string_view>

namespace URay
{

class LogSink
{
public:
    virtual ~LogSink() = default;

public:
    virtual void Write(std::string_view msg) = 0;
};

} // namespace URay

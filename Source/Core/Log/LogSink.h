#pragma once
#include <Core/Type/Types.h>


namespace URay
{

class LogSink
{
public:
    virtual ~LogSink() = default;

public:
    virtual void Write(StringView msg) = 0;
};

} // namespace URay

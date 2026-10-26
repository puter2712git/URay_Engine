#pragma once
#include <Core/Type/TypeAlias.h>


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

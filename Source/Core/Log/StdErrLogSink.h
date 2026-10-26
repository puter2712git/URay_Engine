#pragma once
#include <Core/Type/TypeAlias.h>

#include <Core/Log/LogSink.h>

namespace URay
{

class StdErrLogSink final : public LogSink
{
public:
    StdErrLogSink();
    ~StdErrLogSink() override;

public:
    void Write(StringView msg) override;
};

} // namespace URay

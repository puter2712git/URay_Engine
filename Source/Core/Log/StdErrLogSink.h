#pragma once

#include <Core/Log/LogSink.h>
#include <Core/Type/TypeAlias.h>

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

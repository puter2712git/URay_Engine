#pragma once

#include "Core/Log/LogSink.h"

namespace URay
{

class StdErrLogSink final : public LogSink
{
public:
    StdErrLogSink();
    ~StdErrLogSink() override;

public:
    void Write(std::string_view msg) override;
};

} // namespace URay

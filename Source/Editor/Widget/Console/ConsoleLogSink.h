#pragma once

#include "Core/Log/LogSink.h"

namespace URay
{

class ConsoleWidget;

class EditorConsoleLogSink final : public LogSink
{
public:
    EditorConsoleLogSink(ConsoleWidget& console);
    ~EditorConsoleLogSink() override;

public:
    void Write(std::string_view msg) override;

private:
    ConsoleWidget& console;
};

} // namespace URay

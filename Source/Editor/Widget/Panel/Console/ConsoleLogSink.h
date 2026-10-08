#pragma once

#include <Core/Log/LogSink.h>

namespace URay
{

class ConsolePanel;

class EditorConsoleLogSink final : public LogSink
{
public:
    EditorConsoleLogSink(ConsolePanel& console);
    ~EditorConsoleLogSink() override;

public:
    void Write(std::string_view msg) override;

private:
    ConsolePanel& console;
};

} // namespace URay

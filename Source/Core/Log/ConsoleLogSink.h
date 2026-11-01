#pragma once

#include <Core/Log/LogSink.h>
#include <Core/Type/TypeAlias.h>

namespace URay
{

class ConsolePanel;

class EditorConsoleLogSink final : public LogSink
{
public:
    EditorConsoleLogSink(ConsolePanel& console);
    ~EditorConsoleLogSink() override;

public:
    void Write(StringView msg) override;

private:
    ConsolePanel& console;
};

} // namespace URay

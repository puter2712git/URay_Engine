#pragma once
#include <Core/Type/Types.h>

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
    void Write(StringView msg) override;

private:
    ConsolePanel& console;
};

} // namespace URay

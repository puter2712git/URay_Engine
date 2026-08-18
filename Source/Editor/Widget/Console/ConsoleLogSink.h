#pragma once

#include "Core/Log/LogSink.h"

namespace URay
{

class ConsoleWidget;

class EditorConsoleLogSink : public ILogSink
{
public:
    EditorConsoleLogSink(ConsoleWidget& console);
    virtual ~EditorConsoleLogSink() override = default;

public:
    void Write(const std::string& message) override;

private:
    ConsoleWidget& console;
};

} // namespace URay

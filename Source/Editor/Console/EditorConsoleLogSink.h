#pragma once

#include "Core/Log/LogSink.h"

namespace URay
{

class EditorConsole;

class EditorConsoleLogSink : public ILogSink
{
public:
    EditorConsoleLogSink(EditorConsole& console);
    virtual ~EditorConsoleLogSink() override = default;

public:
    void Write(const std::string& message) override;

private:
    EditorConsole& console;
};

} // namespace URay

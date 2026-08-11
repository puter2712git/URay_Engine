#include "EditorConsoleLogSink.h"
#include "EditorConsole.h"

namespace URay
{

EditorConsoleLogSink::EditorConsoleLogSink(EditorConsole& console)
    : console(console)
{
}

void EditorConsoleLogSink::Write(const std::string& message)
{
    console.AddLog("%s", message.c_str());
}

} // namespace URay

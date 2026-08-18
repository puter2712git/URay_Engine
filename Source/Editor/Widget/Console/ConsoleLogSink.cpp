#include "ConsoleLogSink.h"
#include "ConsoleWidget.h"

namespace URay
{

EditorConsoleLogSink::EditorConsoleLogSink(ConsoleWidget& console)
    : console(console)
{
}

void EditorConsoleLogSink::Write(const std::string& message)
{
    console.AddLog("%s", message.c_str());
}

} // namespace URay

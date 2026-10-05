#include "Editor/Widget/Panel/Console/ConsoleLogSink.h"
#include "Editor/Widget/Panel/Console/ConsoleWidget.h"

namespace URay
{

EditorConsoleLogSink::EditorConsoleLogSink(ConsoleWidget& console)
    : console(console) {}

EditorConsoleLogSink::~EditorConsoleLogSink() = default;

void EditorConsoleLogSink::Write(std::string_view msg)
{
    console.AddLog("%s", msg.data());
}

} // namespace URay

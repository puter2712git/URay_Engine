#include "ConsoleLogSink.h"
#include <Core/Type/TypeAlias.h>

#include <Editor/Widget/Panel/Console/ConsolePanel.h>

namespace URay
{

EditorConsoleLogSink::EditorConsoleLogSink(ConsolePanel& console)
    : console(console) {}

EditorConsoleLogSink::~EditorConsoleLogSink() = default;

void EditorConsoleLogSink::Write(StringView msg)
{
    console.AddLog(msg);
}

} // namespace URay

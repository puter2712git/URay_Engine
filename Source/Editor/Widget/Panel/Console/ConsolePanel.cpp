#include "ConsolePanel.h"

#include <Editor/Widget/Panel/PanelFactory.h>
#include <Core/Type/TypeAlias.h>

#include <Editor/Widget/EditorWidgetFactory.h>
#include <Editor/Widget/Style/EditorStyles.h>

#include <Core/Log/ConsoleLogSink.h>
#include <Core/Log/LogSystem.h>
#include <Editor/Widget/Panel/ScrollView.h>
#include <Engine/Engine.h>
#include <Engine/UI/Widget/Element/Label.h>
#include <Engine/UI/Widget/Layout/VerticalLayout.h>

namespace URay
{

URAY_REGISTER_PANEL(ConsolePanel, "Console")

ConsolePanel::ConsolePanel()
{
    logSink = new EditorConsoleLogSink(*this);

    LogSystem& logSystem = gEngine->GetLogSystem();
    logSystem.RegisterSink(logSink);

    scrollView = AddChild<ScrollView>();

    logLayout = scrollView->SetContent<VerticalLayout>();
    logLayout->SetCrossAxisAlignment(CrossAxisAlignment::Stretch);
    logLayout->SetGap(1.0f);
}

ConsolePanel::~ConsolePanel()
{
    LogSystem& logSystem = gEngine->GetLogSystem();
    logSystem.UnregisterSink(logSink);

    delete logSink;
}

void ConsolePanel::Arrange(const Rect& rect)
{
    Panel::Arrange(rect);

    scrollView->Arrange(GetContentRect());
}

EventReply ConsolePanel::OnPointerDown(const PointerEvent& event)
{
    return EventReply{
        .requestFocus = true,
    };
}

void ConsolePanel::AddLog(StringView message)
{
    logEntries.emplace_back(message);
    isLogDirty = true;
}

void ConsolePanel::OnUpdate(float deltaTime)
{
    if (!isLogDirty)
        return;

    isLogDirty = false;

    logLayout->ClearChildren();

    for (const String& entry : logEntries)
    {
        Label* line = CreateEditorLabel(*logLayout, entry);
        line->SetTextStyle(EditorStyle::SmallText());
    }

    scrollView->RequestScrollToBottom();
}

} // namespace URay

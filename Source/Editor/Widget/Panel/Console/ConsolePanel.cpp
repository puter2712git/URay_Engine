#include "ConsolePanel.h"

#include <Core/Log/ConsoleLogSink.h>
#include <Core/Log/LogSystem.h>
#include <Editor/Widget/Element/Label.h>
#include <Engine/UI/Layout/VerticalLayout.h>
#include <Editor/Widget/Panel/ScrollView.h>
#include <Engine/Engine.h>

namespace URay
{

ConsolePanel::ConsolePanel() : Panel("Console")
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

void ConsolePanel::AddLog(std::string_view message)
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

    for (const std::string& entry : logEntries)
    {
        Label* line = logLayout->AddChild<Label>(entry);
        line->SetTextStyle(EditorStyle::SmallText());
    }

    scrollView->RequestScrollToBottom();
}

} // namespace URay

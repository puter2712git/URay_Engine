#pragma once
#include <Core/Type/TypeAlias.h>

#include <Editor/Widget/Panel/Panel.h>

#include <string>
#include <vector>

namespace URay
{

class EditorConsoleLogSink;
class ScrollView;
class VerticalLayout;

class ConsolePanel final : public Panel
{
    URAY_CLASS(ConsolePanel, Panel)

public:
    ConsolePanel();
    ~ConsolePanel() override;

public:
    void Arrange(const Rect& rect) override;

    EventReply OnPointerDown(const PointerEvent& event) override;

    void AddLog(StringView message);

protected:
    void OnUpdate(float deltaTime) override;

private:
    EditorConsoleLogSink* logSink = nullptr;

    ScrollView* scrollView = nullptr;
    VerticalLayout* logLayout = nullptr;

    std::vector<String> logEntries;
    bool isLogDirty = false;
};

} // namespace URay

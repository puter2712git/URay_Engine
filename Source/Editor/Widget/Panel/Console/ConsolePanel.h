#pragma once

#include <Editor/Widget/Panel/Panel.h>

#include <string>
#include <string_view>
#include <vector>

namespace URay
{

class EditorConsoleLogSink;
class ScrollView;
class VerticalLayout;

class ConsolePanel final : public Panel
{
    URAY_TYPE(ConsolePanel, Panel)

public:
    ConsolePanel();
    ~ConsolePanel() override;

public:
    void Arrange(const Rect& rect) override;

    EventReply OnPointerDown(const PointerEvent& event) override;

    void AddLog(std::string_view message);

protected:
    void OnUpdate(float deltaTime) override;

private:
    EditorConsoleLogSink* logSink = nullptr;

    ScrollView* scrollView = nullptr;
    VerticalLayout* logLayout = nullptr;

    std::vector<std::string> logEntries;
    bool isLogDirty = false;
};

} // namespace URay

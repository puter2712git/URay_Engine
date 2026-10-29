#pragma once

#include <Engine/UI/Widget/Widget.h>

#include <memory>
#include <optional>
#include <vector>

namespace URay
{

class Button;
class UISystem;

struct DetachedTab
{
    String title;
    std::unique_ptr<Widget> panel;
};

class DockTabGroup final : public Widget
{
    URAY_CLASS(DockTabGroup, Widget)

public:
    explicit DockTabGroup(UISystem& uiSystem);

public:
    void AddTab(const String& title, std::unique_ptr<Widget> panel);

    std::optional<DetachedTab> DetachTab(ObjectId panelId);
    bool IsEmpty() const { return tabs.empty(); }

    void Arrange(const Rect& rect) override;

protected:
    void OnUpdate(float deltaTime) override;
    void OnPaintForeground(Render::UIDrawContext& context) override;

private:
    struct Tab
    {
        String title;
        Button* button = nullptr;
        Widget* panel = nullptr;
    };

private:
    void RequestActivate(ObjectId panelId);
    void UpdateTabColors();

    const Tab* FindTab(ObjectId panelId);

private:
    UISystem& uiSystem;
    std::vector<Tab> tabs;

    std::optional<ObjectId> activeId;
    std::optional<ObjectId> pendingId;
};

} // namespace URay

#pragma once

#include <Engine/UI/Widget/Widget.h>

#include <memory>
#include <optional>
#include <vector>

namespace URay
{

class Button;
class UISystem;

struct DockedTab
{
    String title;
    Button* button = nullptr;
    Widget* panel = nullptr;
};

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

    bool HasTab(ObjectId panelId) const;
    const DockedTab* FindTab(ObjectId panelId) const;

    std::optional<DetachedTab> DetachTab(ObjectId panelId);
    bool IsEmpty() const { return tabs.empty(); }

    void Arrange(const Rect& rect) override;

protected:
    void OnUpdate(float deltaTime) override;
    void OnPaintForeground(Render::UIDrawContext& context) override;

private:
    void RequestActivate(ObjectId panelId);
    void UpdateTabColors();

private:
    UISystem& uiSystem;
    std::vector<DockedTab> tabs;

    std::optional<ObjectId> activeId;
    std::optional<ObjectId> pendingId;
};

} // namespace URay

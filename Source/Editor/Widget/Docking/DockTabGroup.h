#pragma once

#include <Editor/Widget/Docking/DockDropTarget.h>
#include <Engine/UI/Widget/Widget.h>

#include <memory>
#include <optional>
#include <vector>

namespace URay
{

class DockTabButton;
class UISystem;

struct DockedTab
{
    String title;
    DockTabButton* button = nullptr;
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

    Rect GetPanelRect() const;
    size_t GetTabCount() const { return tabs.size(); }

    void SetDropHighlight(std::optional<DockDropZone> zone) { dropHighlight = zone; }

    std::optional<DetachedTab> DetachTab(ObjectId panelId);
    bool IsEmpty() const { return tabs.empty(); }

    void Arrange(const Rect& rect) override;

protected:
    void OnUpdate(float deltaTime) override;
    void OnPaintForeground(Render::UIDrawContext& context) override;

private:
    void RequestActivate(ObjectId panelId);
    void UpdateTabColors();

    void OnTabDragMoved(Vector2 position, ObjectId panelId);
    void OnTabDragReleased(Vector2 position, ObjectId panelId);

    Rect GetDropHighlightRect(DockDropZone zone) const;

private:
    UISystem& uiSystem;
    std::vector<DockedTab> tabs;

    std::optional<ObjectId> activeId;
    std::optional<ObjectId> pendingId;

    std::optional<DockDropZone> dropHighlight;
};

} // namespace URay

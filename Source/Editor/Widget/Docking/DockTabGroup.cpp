#include "DockTabGroup.h"

#include <Core/Math/Math.h>
#include <Editor/Widget/EditorWidgetFactory.h>
#include <Editor/Widget/Style/EditorStyles.h>
#include <Engine/UI/UISystem.h>
#include <Engine/UI/Widget/Element/Button.h>
#include <Render/Rendering/Batch/UIDrawContext.h>

#include <algorithm>
#include <cassert>

namespace URay
{

DockTabGroup::DockTabGroup(UISystem& uiSystem) : uiSystem(uiSystem) {}

void DockTabGroup::AddTab(const String& title, std::unique_ptr<Widget> panel)
{
    if (!panel)
        return;

    Widget* panelPtr = panel.get();

    panelPtr->SetVisible(!activeId.has_value());
    AddChild(std::move(panel));

    Button* button = CreateDockTabButton(*this, title);
    button->Register("Clicked", this, &DockTabGroup::RequestActivate, panelPtr->GetId());

    tabs.push_back(DockedTab{ .title = title, .button = button, .panel = panelPtr });

    if (!activeId.has_value())
        activeId = panelPtr->GetId();

    UpdateTabColors();
}

bool DockTabGroup::HasTab(ObjectId panelId) const
{
    return FindTab(panelId) != nullptr;
}

const DockedTab* DockTabGroup::FindTab(ObjectId panelId) const
{
    for (const DockedTab& tab : tabs)
    {
        if (tab.panel->GetId() == panelId)
            return &tab;
    }
    return nullptr;
}

std::optional<DetachedTab> DockTabGroup::DetachTab(ObjectId panelId)
{
    const auto it = std::find_if(tabs.begin(), tabs.end(), [panelId](const DockedTab& tab)
                                 { return tab.panel->GetId() == panelId; });

    if (it == tabs.end())
        return std::nullopt;

    // Preserve ongoing pointer interactions anywhere in the source group.
    if (!uiSystem.PrepareForDetach(*this))
        return std::nullopt;

    const bool wasActive = activeId.has_value() && *activeId == panelId;
    String title = it->title;
    Widget* panel = it->panel;
    Button* button = it->button;

    std::unique_ptr<Widget> removedButton = RemoveChild(button);
    std::unique_ptr<Widget> detachedPanel = RemoveChild(panel);
    assert(removedButton && detachedPanel);

    tabs.erase(it);

    if (pendingId.has_value() && *pendingId == panelId)
        pendingId.reset();

    if (wasActive)
    {
        activeId.reset();
        if (!tabs.empty())
        {
            tabs.front().panel->SetVisible(true);
            activeId = tabs.front().panel->GetId();
        }
    }

    detachedPanel->SetVisible(false);
    UpdateTabColors();

    return DetachedTab{ .title = std::move(title), .panel = std::move(detachedPanel) };
}

void DockTabGroup::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    constexpr float tabHeight = 25.0f;
    constexpr float tabWidth = 120.0f;

    const float actualTabHeight = Math::Min(tabHeight, Math::Max(0.0f, rect.size.y));

    Rect panelRect = {};
    panelRect.position = Vector2(rect.position.x, rect.position.y + actualTabHeight);
    panelRect.size = Vector2(rect.size.x, Math::Max(0.0f, rect.size.y - actualTabHeight));

    for (size_t i = 0; i < tabs.size(); ++i)
    {
        Rect tabRect = {};
        tabRect.position = Vector2(rect.position.x + static_cast<float>(i) * tabWidth, rect.position.y);
        tabRect.size = Vector2(tabWidth, actualTabHeight);

        tabs[i].button->Arrange(tabRect);
        tabs[i].panel->Arrange(panelRect);
    }
}

void DockTabGroup::OnUpdate(float deltaTime)
{
    if (!pendingId.has_value())
        return;

    const ObjectId nextId = *pendingId;

    if (activeId == nextId)
    {
        pendingId.reset();
        return;
    }

    const DockedTab* nextTab = FindTab(nextId);
    if (!nextTab)
    {
        pendingId.reset();
        return;
    }

    if (activeId.has_value())
    {
        const DockedTab* activeTab = FindTab(*activeId);

        if (activeTab && !uiSystem.TryHideWidget(*activeTab->panel))
            return;
    }

    nextTab->panel->SetVisible(true);
    activeId = nextId;
    pendingId.reset();
    UpdateTabColors();
}

void DockTabGroup::OnPaintForeground(Render::UIDrawContext& context)
{
    context.AddOutlineRect(rect, 1.0f, EditorStyle::Border);

    for (const DockedTab& tab : tabs)
    {
        const Rect& tabRect = tab.button->GetRect();
        if (tabRect.size.x <= 0.0f || tabRect.size.y <= 0.0f)
            continue;

        const float thickness = Math::Min(1.0f, Math::Min(tabRect.size.x, tabRect.size.y) * 0.5f);
        context.AddFilledRect({ .position = tabRect.position, .size = Vector2(tabRect.size.x, thickness) }, EditorStyle::Border);
        context.AddFilledRect({ .position = Vector2(tabRect.position.x, tabRect.position.y + thickness),
                                .size = Vector2(thickness, tabRect.size.y - thickness) },
                              EditorStyle::Border);
        context.AddFilledRect({ .position = Vector2(tabRect.position.x + tabRect.size.x - thickness, tabRect.position.y + thickness),
                                .size = Vector2(thickness, tabRect.size.y - thickness) },
                              EditorStyle::Border);
    }
}

void DockTabGroup::UpdateTabColors()
{
    for (size_t i = 0; i < tabs.size(); ++i)
    {
        const bool active = activeId.has_value() && *activeId == tabs[i].panel->GetId();
        Button& button = *tabs[i].button;
        button.SetNormalColor(active ? EditorStyle::DockTabActive : EditorStyle::DockTabInactive);
        button.SetHoverColor(active ? EditorStyle::DockTabActiveHovered : EditorStyle::DockTabInactiveHovered);
        button.SetPressedColor(active ? EditorStyle::DockTabActivePressed : EditorStyle::DockTabInactivePressed);
    }
}

void DockTabGroup::RequestActivate(ObjectId panelId)
{
    if (panelId == ObjectId{})
        return;

    pendingId = panelId;
}

} // namespace URay

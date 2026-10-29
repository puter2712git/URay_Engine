#include "DockTabGroup.h"

#include <Core/Math/Math.h>
#include <Editor/Widget/EditorWidgetFactory.h>
#include <Editor/Widget/Style/EditorStyles.h>
#include <Engine/UI/UISystem.h>
#include <Engine/UI/Widget/Element/Button.h>
#include <Render/Rendering/Batch/UIDrawContext.h>

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

    tabs.push_back(Tab{ .button = button, .panel = panelPtr });

    if (!activeId.has_value())
        activeId = panelPtr->GetId();

    UpdateTabColors();
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

    const Tab* nextTab = FindTab(nextId);
    if (!nextTab)
    {
        pendingId.reset();
        return;
    }

    if (activeId.has_value())
    {
        const Tab* activeTab = FindTab(*activeId);

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

    for (const Tab& tab : tabs)
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

const DockTabGroup::Tab* DockTabGroup::FindTab(ObjectId panelId)
{
    for (Tab& tab : tabs)
    {
        if (tab.panel->GetId() == panelId)
            return &tab;
    }
    return nullptr;
}

} // namespace URay

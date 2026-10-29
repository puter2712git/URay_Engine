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

    const int32 index = static_cast<int32>(tabs.size());

    Widget* panelPtr = panel.get();

    panelPtr->SetVisible(!activeIndex.has_value());
    AddChild(std::move(panel));

    Button* button = CreateDockTabButton(*this, title);
    button->Register("Clicked", this, &DockTabGroup::RequestActivate, index);

    tabs.push_back(Tab{ .button = button, .panel = panelPtr });

    if (!activeIndex.has_value())
        activeIndex = index;

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
    if (!pendingIndex.has_value())
        return;

    const int32 nextIndex = *pendingIndex;

    if (activeIndex == nextIndex)
    {
        pendingIndex.reset();
        return;
    }

    if (activeIndex.has_value())
    {
        Widget* currentPanel = tabs[*activeIndex].panel;

        if (!uiSystem.TryHideWidget(*currentPanel))
            return;
    }

    tabs[nextIndex].panel->SetVisible(true);
    activeIndex = nextIndex;
    UpdateTabColors();
    pendingIndex.reset();
}

void DockTabGroup::OnPaintForeground(Render::UIDrawContext& context)
{
    for (const Tab& tab : tabs)
        context.AddOutlineRect(tab.button->GetRect(), 1.0f, EditorStyle::Border);
}

void DockTabGroup::UpdateTabColors()
{
    for (size_t i = 0; i < tabs.size(); ++i)
    {
        const bool active = activeIndex.has_value() && static_cast<size_t>(*activeIndex) == i;
        Button& button = *tabs[i].button;
        button.SetNormalColor(active ? EditorStyle::DockTabActive : EditorStyle::DockTabInactive);
        button.SetHoverColor(active ? EditorStyle::DockTabActiveHovered : EditorStyle::DockTabInactiveHovered);
        button.SetPressedColor(active ? EditorStyle::DockTabActivePressed : EditorStyle::DockTabInactivePressed);
    }
}

void DockTabGroup::RequestActivate(int32 index)
{
    if (index < 0 || index >= static_cast<int32>(tabs.size()))
        return;

    pendingIndex = index;
}

} // namespace URay

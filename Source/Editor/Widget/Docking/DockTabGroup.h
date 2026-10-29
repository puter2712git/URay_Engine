#pragma once

#include <Engine/UI/Widget/Widget.h>

#include <memory>
#include <optional>
#include <vector>

namespace URay
{

class Button;
class UISystem;

class DockTabGroup final : public Widget
{
    URAY_CLASS(DockTabGroup, Widget)

public:
    explicit DockTabGroup(UISystem& uiSystem);

public:
    void AddTab(const String& title, std::unique_ptr<Widget> panel);

    void Arrange(const Rect& rect) override;

protected:
    void OnUpdate(float deltaTime) override;
    void OnPaintForeground(Render::UIDrawContext& context) override;

private:
    void RequestActivate(int32 index);
    void UpdateTabColors();

private:
    struct Tab
    {
        Button* button = nullptr;
        Widget* panel = nullptr;
    };

    UISystem& uiSystem;
    std::vector<Tab> tabs;

    std::optional<int32> activeIndex;
    std::optional<int32> pendingIndex;
};

} // namespace URay

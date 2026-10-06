#pragma once

#include "Editor/Gizmo/Strategy/GizmoStrategy.h"
#include "Editor/Widget/Widget.h"

#include "Core/Type/Types.h"

#include <array>

namespace URay
{

class Button;
class HorizontalLayout;

class ViewportToolbar final : public Widget
{
    URAY_TYPE(ViewportToolbar, Widget)

public:
    ViewportToolbar();
    ~ViewportToolbar() override;

public:
    void Arrange(const Rect& rect) override;

    const std::array<Button*, static_cast<size_t>(GizmoMode::Count)>& GetGizmoModeButtons() const { return gizmoModeButtons; }
    Button* GetGizmoSpaceToggleButton() const { return gizmoSpaceToggleButton; }

private:
    HorizontalLayout* gizmoModeButtonLayout = nullptr;
    std::array<Button*, static_cast<size_t>(GizmoMode::Count)> gizmoModeButtons;
    Button* gizmoSpaceToggleButton = nullptr;
    Button* viewModeToggleButton = nullptr;
};

} // namespace URay

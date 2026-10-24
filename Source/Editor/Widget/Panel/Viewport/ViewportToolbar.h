#pragma once

#include <Editor/Gizmo/Strategy/GizmoStrategy.h>
#include <Engine/UI/Widget/Widget.h>

#include <Core/Type/Types.h>

#include <array>

namespace URay
{

class Button;
class HorizontalLayout;
class Image;

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
    Button* GetViewModeToggleButton() const { return viewModeToggleButton; }
    void SetGizmoSpaceLocal(bool isLocal);

    Button* GetPlayButton() const { return playButton; }
    Button* GetStopButton() const { return stopButton; }

private:
    HorizontalLayout* gizmoModeButtonLayout = nullptr;
    HorizontalLayout* playControlLayout = nullptr;

    std::array<Button*, static_cast<size_t>(GizmoMode::Count)> gizmoModeButtons;
    Button* gizmoSpaceToggleButton = nullptr;
    Image* gizmoSpaceImage = nullptr;
    Button* viewModeToggleButton = nullptr;

    Button* playButton = nullptr;
    Button* stopButton = nullptr;
};

} // namespace URay

#pragma once

#include <Core/Type/TypeAlias.h>
#include <Editor/Widget/Panel/Panel.h>

namespace URay
{

class ViewportController;
class ViewportToolbar;

class ViewportPanel final : public Panel
{
    URAY_CLASS(ViewportPanel, Panel)

public:
    explicit ViewportPanel(ViewportController& controller);
    ~ViewportPanel() override;

public:
    void Arrange(const Rect& rect) override;

    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnPointerMove(const PointerEvent& event) override;
    EventReply OnPointerUp(const PointerEvent& event) override;

protected:
    void OnUpdate(float deltaTime) override;
    void OnPaint(Render::UIDrawContext& context) override;

private:
    void OnGizmoModeClicked(int32 index);
    void OnGizmoSpaceClicked();
    void OnViewModeClicked();
    void OnPlayClicked();
    void OnStopClicked();

private:
    ViewportController& controller;
    ViewportToolbar* toolbar = nullptr;
};

} // namespace URay

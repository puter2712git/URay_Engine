#include <Editor/Widget/Panel/Viewport/ViewportPanel.h>

#include <Editor/Viewport/ViewportController.h>
#include <Editor/Widget/Panel/Viewport/ViewportToolbar.h>
#include <Editor/Widget/Style/EditorStyles.h>
#include <Engine/UI/Widget/Element/Button.h>
#include <Render/Rendering/Batch/UIDrawContext.h>

#include <memory>
#include <optional>

namespace URay
{

ViewportPanel::ViewportPanel(ViewportController& controller)
    : controller(controller)
{
    SetBackgroundColor(EditorStyle::Surface);

    auto viewportToolbar = std::make_unique<ViewportToolbar>();
    toolbar = viewportToolbar.get();
    AddChild(std::move(viewportToolbar));

    const auto gizmoButtons = toolbar->GetGizmoModeButtons();
    for (size_t index = 0; index < gizmoButtons.size(); ++index)
    {
        gizmoButtons[index]->Register("Clicked", this, &ViewportPanel::OnGizmoModeClicked, static_cast<int32>(index));
    }

    toolbar->GetGizmoSpaceToggleButton()->Register("Clicked", this, &ViewportPanel::OnGizmoSpaceClicked);
    toolbar->GetViewModeToggleButton()->Register("Clicked", this, &ViewportPanel::OnViewModeClicked);
    toolbar->GetPlayButton()->Register("Clicked", this, &ViewportPanel::OnPlayClicked);
    toolbar->GetStopButton()->Register("Clicked", this, &ViewportPanel::OnStopClicked);

    toolbar->SetGizmoSpaceLocal(controller.IsGizmoSpaceLocal());
}

ViewportPanel::~ViewportPanel() = default;

void ViewportPanel::Arrange(const Rect& rect)
{
    Panel::Arrange(rect);

    constexpr float toolbarHeight = 40.0f;

    Rect toolbarRect = {};
    toolbarRect.position = rect.position;
    toolbarRect.size = Vector2(rect.size.x, toolbarHeight);
    toolbar->Arrange(toolbarRect);
}

EventReply ViewportPanel::OnPointerDown(const PointerEvent& event)
{
    return controller.OnPointerDown(event);
}

EventReply ViewportPanel::OnPointerMove(const PointerEvent& event)
{
    return controller.OnPointerMove(event);
}

EventReply ViewportPanel::OnPointerUp(const PointerEvent& event)
{
    return controller.OnPointerUp(event);
}

void ViewportPanel::OnUpdate(float deltaTime)
{
    controller.Update(deltaTime, IsFocused());
}

void ViewportPanel::OnPaint(Render::UIDrawContext& context)
{
    Panel::OnPaint(context);

    constexpr float toolbarHeight = 40.0f;

    Rect imageRect = rect;
    imageRect.position.y += toolbarHeight;
    imageRect.size.y -= toolbarHeight;

    if (imageRect.size.x <= 0.0f || imageRect.size.y <= 0.0f)
        return;

    controller.SetDisplayRect(imageRect);

    const std::optional<ViewportImage> image = controller.GetDisplayImage();
    if (!image)
        return;

    context.AddImage(imageRect, { .view = image->view, .sampler = image->sampler });
}

void ViewportPanel::OnGizmoModeClicked(int32 index)
{
    controller.SetGizmoMode(static_cast<GizmoMode>(index));
}

void ViewportPanel::OnGizmoSpaceClicked()
{
    controller.ToggleGizmoSpace();
}

void ViewportPanel::OnViewModeClicked()
{
    controller.CycleViewMode();
}

void ViewportPanel::OnPlayClicked()
{
    controller.Play();
}

void ViewportPanel::OnStopClicked()
{
    controller.Stop();
}

} // namespace URay

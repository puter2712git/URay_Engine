#include "ViewportWidget.h"

#include "Core/Log/Log.h"

#include <imgui/imgui.h>

namespace URay
{

EventReply ViewportWidget::OnPointerEnter()
{
    Logger::Log("Viewport pointer enter.");
    return {};
}

EventReply ViewportWidget::OnPointerLeave()
{
    Logger::Log("Viewport pointer leave.");
    return {};
}

EventReply ViewportWidget::OnPointerDown(const PointerEvent& event)
{
    Logger::Log("Viewport pointer down.");
    return {
        .capturePointer = true
    };
}

EventReply ViewportWidget::OnPointerMove(const PointerEvent& event)
{
    return {};
}

EventReply ViewportWidget::OnPointerUp(const PointerEvent& event)
{
    Logger::Log("Viewport pointer up.");
    return {
        .releasePointer = true
    };
}

void ViewportWidget::OnDraw()
{
    ApplyRect();

    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    ImGui::End();
}

} // namespace URay

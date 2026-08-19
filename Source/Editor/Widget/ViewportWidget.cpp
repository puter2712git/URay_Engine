#include "ViewportWidget.h"

#include "Core/Log/Log.h"

#include <imgui/imgui.h>

namespace URay
{

EventReply ViewportWidget::OnPointerDown(const PointerEvent& event)
{
    Logger::Log("Viewport pointer down.");
    return {
        .capturePointer = true
    };
}

EventReply ViewportWidget::OnPointerMove(const PointerEvent& event)
{
    Logger::Log("Viewport pointer move.");
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

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
        .requestFocus = true,
        .capturePointer = true,
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

EventReply ViewportWidget::OnKeyDown(const KeyEvent& event)
{
    Logger::Log("Viewport key down: " + std::to_string(static_cast<uint16_t>(event.key)));
    return {};
}

EventReply ViewportWidget::OnKeyUp(const KeyEvent& event)
{
    Logger::Log("Viewport key up: " + std::to_string(static_cast<uint16_t>(event.key)));
    return {};
}

void ViewportWidget::OnDraw()
{
    ApplyRect();

    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    ImGui::End();
}

} // namespace URay

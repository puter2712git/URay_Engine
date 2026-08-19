#include "ViewportWidget.h"

#include "Render/Renderer.h"

#include "Core/Log/Log.h"

#include <imgui/imgui.h>

namespace URay
{

ViewportWidget::ViewportWidget(RHI::Renderer& renderer)
    : renderer(renderer)
{
}

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

    constexpr ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize;

    ImGui::Begin("Viewport", nullptr, flags);

    const VkDescriptorSet descriptorSet = renderer.GetSceneImGuiTexture();

    if (descriptorSet != VK_NULL_HANDLE)
    {
        const ImTextureID textureId = static_cast<ImTextureID>(reinterpret_cast<uintptr_t>(descriptorSet));
        const ImVec2 imageSize = ImGui::GetContentRegionAvail();

        ImGui::Image(ImTextureRef(textureId), imageSize);
    }

    ImGui::End();
}

} // namespace URay

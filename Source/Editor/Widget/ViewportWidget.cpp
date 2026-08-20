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

    imageRect = {};
    targetExtent = {};

    constexpr ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize;

    ImGui::Begin("Viewport", nullptr, flags);

    const ImVec2 imagePosition = ImGui::GetCursorScreenPos();
    const ImVec2 logicalSize = ImGui::GetContentRegionAvail();

    const VkDescriptorSet descriptorSet = renderer.GetSceneImGuiTexture();
    if (descriptorSet)
    {
        imageRect = {
            .position = Vector2(imagePosition.x, imagePosition.y),
            .size = Vector2(logicalSize.x, logicalSize.y),
        };

        targetExtent = renderer.GetSceneRenderTargetExtent();

        const ImVec2 framebufferScale = ImGui::GetIO().DisplayFramebufferScale;
        const Extent2D requestedExtent = {
            .width = static_cast<uint32_t>(std::round(logicalSize.x * framebufferScale.x)),
            .height = static_cast<uint32_t>(std::round(logicalSize.y * framebufferScale.y)),
        };

        renderer.RequestSceneRenderTargetResize(requestedExtent);

        const ImTextureID textureId = static_cast<ImTextureID>(reinterpret_cast<uintptr_t>(descriptorSet));
        ImGui::Image(ImTextureRef(textureId), logicalSize);
    }

    ImGui::End();
}

std::optional<Vector2> ViewportWidget::WindowToRenderTarget(const Vector2& windowPosition) const
{
    if (imageRect.size.x <= 0.0f || imageRect.size.y <= 0.0f)
        return std::nullopt;

    const Vector2 viewportPosition = Vector2(
        windowPosition.x - imageRect.position.x,
        windowPosition.y - imageRect.position.y);

    if (viewportPosition.x < 0.0f ||
        viewportPosition.y < 0.0f ||
        viewportPosition.x >= imageRect.size.x ||
        viewportPosition.y >= imageRect.size.y)
    {
        return std::nullopt;
    }

    if (targetExtent.width == 0 || targetExtent.height == 0)
        return std::nullopt;

    return Vector2(
        viewportPosition.x / imageRect.size.x * targetExtent.width,
        viewportPosition.y / imageRect.size.y * targetExtent.height);
}

} // namespace URay

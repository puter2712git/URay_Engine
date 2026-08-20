#include "ViewportWidget.h"

#include "Editor/EditorPicker.h"
#include "Editor/GizmoController.h"

#include "Engine/Component/CameraComponent.h"
#include "Engine/Engine.h"

#include "Render/Renderer.h"

#include "Core/Log/Log.h"

#include <imgui/imgui.h>

namespace URay
{

ViewportWidget::ViewportWidget(RHI::Renderer& renderer, CameraComponent& camera, Engine& engine, std::function<void(Unit*)> onSelectUnitFunc)
    : renderer(renderer), camera(camera), onSelectUnit(onSelectUnitFunc)
{
    camera.SetViewportExtent(renderer.GetSceneRenderTargetExtent());

    gizmo = std::make_unique<GizmoController>(*engine.GetMeshManager(), *engine.GetMaterialManager());
    picker = std::make_unique<EditorPicker>(engine, gizmo.get());
}

ViewportWidget::~ViewportWidget()
{
    if (picker)
    {
        picker.reset();
        picker = nullptr;
    }
    if (gizmo)
    {
        gizmo.reset();
        gizmo = nullptr;
    }
}

EventReply ViewportWidget::OnPointerDown(const PointerEvent& event)
{
    if (event.changedButton == MouseButton::Left)
    {
        const auto targetPosition = WindowToRenderTarget(event.position);
        if (!targetPosition)
            return {};

        const PickResult pickResult = picker->Pick(&camera, targetPosition->x, targetPosition->y);
        if (!pickResult.hit)
        {
            onSelectUnit(nullptr);
        }
        else
        {
            if (pickResult.gizmoAxis != -1)
            {
                gizmo->StartDragging(*targetPosition, pickResult.gizmoAxis, camera);
            }
            else
            {
                onSelectUnit(pickResult.pickedUnit);
            }
        }
    }

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
    if (event.changedButton == MouseButton::Left)
    {
        if (gizmo && gizmo->IsDragging())
        {
            gizmo->EndDragging();
        }
    }

    return EventReply{
        .releasePointer = true,
    };
}

EventReply ViewportWidget::OnKeyDown(const KeyEvent& event)
{
    if (event.key == KeyCode::Space)
    {
        if (gizmo)
        {
            GizmoMode mode = gizmo->GetMode();
            int modeIndex = static_cast<int>(mode);
            modeIndex = (modeIndex + 1) % static_cast<int>(GizmoMode::Count);

            GizmoMode newMode = static_cast<GizmoMode>(modeIndex);
            gizmo->SetMode(newMode);
        }
    }

    return {};
}

EventReply ViewportWidget::OnKeyUp(const KeyEvent& event)
{
    return {};
}

void ViewportWidget::SetSelectedUnit(Unit* unit)
{
    gizmo->SetTarget(unit);
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
        camera.SetViewportExtent(targetExtent);

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

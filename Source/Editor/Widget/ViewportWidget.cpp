#include "ViewportWidget.h"

#include "Editor/EditorPicker.h"
#include "Editor/GizmoController.h"
#include "Editor/SelectionSystem.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Component/Render/CameraComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/SceneSystem.h"
#include "Engine/Scene/Unit.h"

#include "Render/Renderer.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/Log/Log.h"

#include <imgui/imgui.h>

namespace URay
{

ViewportWidget::ViewportWidget(Render::Renderer& renderer, CameraComponent& camera, Engine& engine, SelectionSystem& selectionSystem)
    : renderer(renderer), camera(camera), engine(engine), selectionSystem(selectionSystem)
{
    camera.SetViewportExtent(renderer.GetSceneRenderTargetExtent());

    gizmo = std::make_unique<GizmoController>(engine.GetAssetSystem());
    picker = std::make_unique<EditorPicker>(engine, gizmo.get());

    onSelectedRayHandle = selectionSystem.RegisterOnSelected([this](Unit* unit)
                                                             { gizmo->SetTarget(unit); });
}

ViewportWidget::~ViewportWidget()
{
    selectionSystem.UnregisterOnSelected(onSelectedRayHandle);

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
    if (event.changedButton == MouseButton::Right)
    {
        isCameraRotating = true;

        return EventReply{
            .requestFocus = true,
            .capturePointer = true,
        };
    }

    if (event.changedButton == MouseButton::Left)
    {
        const auto targetPosition = WindowToRenderTarget(event.position);
        if (!targetPosition)
            return {};

        const PickResult pickResult = picker->Pick(&camera, targetPosition->x, targetPosition->y);
        if (!pickResult.hit)
        {
            selectionSystem.SelectUnit(nullptr);
        }
        else
        {
            if (pickResult.gizmoAxis != -1)
            {
                gizmo->StartDragging(*targetPosition, pickResult.gizmoAxis, camera);
            }
            else
            {
                selectionSystem.SelectUnit(pickResult.pickedUnit);
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
    if (isCameraRotating)
    {
        pendingCameraLookDelta.x += event.delta.x;
        pendingCameraLookDelta.y += event.delta.y;
        return {};
    }

    if (gizmo)
    {
        const auto targetPosition = WindowToRenderTarget(event.position);
        if (!targetPosition)
            return {};

        cachedPosition = *targetPosition;
    }

    return {};
}

EventReply ViewportWidget::OnPointerUp(const PointerEvent& event)
{
    if (event.changedButton == MouseButton::Right)
    {
        isCameraRotating = false;
    }

    if (event.changedButton == MouseButton::Left)
    {
        if (gizmo && gizmo->IsDragging())
        {
            gizmo->EndDragging();
        }
    }

    return EventReply{
        .releasePointer = event.pressedButtons == 0,
    };
}

EventReply ViewportWidget::OnKeyDown(const KeyEvent& event)
{
    if (event.key == KeyCode::S &&
        event.action == KeyAction::Pressed &&
        (event.modifiers & ModifierKey::Ctrl) != ModifierKey::None)
    {
        SaveCurrScene();
        return {};
    }

    if (event.key == KeyCode::Space && event.action == KeyAction::Pressed)
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

    if (event.key == KeyCode::W)
    {
        cameraForward = true;
    }
    if (event.key == KeyCode::S)
    {
        cameraBackward = true;
    }
    if (event.key == KeyCode::D)
    {
        cameraRight = true;
    }
    if (event.key == KeyCode::A)
    {
        cameraLeft = true;
    }
    if (event.key == KeyCode::E)
    {
        cameraUp = true;
    }
    if (event.key == KeyCode::Q)
    {
        cameraDown = true;
    }

    return {};
}

EventReply ViewportWidget::OnKeyUp(const KeyEvent& event)
{
    if (event.key == KeyCode::W)
    {
        cameraForward = false;
    }
    if (event.key == KeyCode::S)
    {
        cameraBackward = false;
    }
    if (event.key == KeyCode::D)
    {
        cameraRight = false;
    }
    if (event.key == KeyCode::A)
    {
        cameraLeft = false;
    }
    if (event.key == KeyCode::E)
    {
        cameraUp = false;
    }
    if (event.key == KeyCode::Q)
    {
        cameraDown = false;
    }

    return {};
}

void ViewportWidget::OnUpdate(float deltaTime)
{
    PickResult pickResult = picker->Pick(&camera, cachedPosition.x, cachedPosition.y);
    if (!pickResult.hit || pickResult.gizmoAxis == -1)
    {
        gizmo->SetHoveredAxis(-1);
    }

    if (pickResult.gizmoAxis != -1)
    {
        gizmo->SetHoveredAxis(pickResult.gizmoAxis);
    }

    gizmo->Update(cachedPosition, camera);

    UpdateCameraMovement(deltaTime);
    UpdateCameraRotation();
}

void ViewportWidget::OnPrepareRender(Render::DrawCommandBuilder& builder)
{
    gizmo->Draw(builder);
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

void ViewportWidget::UpdateCameraMovement(float deltaTime)
{
    Vector3 cameraMove = Vector3::Zero;

    if (cameraForward)
    {
        cameraMove.y += 5.0f * deltaTime;
    }
    if (cameraBackward)
    {
        cameraMove.y -= 5.0f * deltaTime;
    }
    if (cameraRight)
    {
        cameraMove.x += 5.0f * deltaTime;
    }
    if (cameraLeft)
    {
        cameraMove.x -= 5.0f * deltaTime;
    }

    TransformComponent* cameraTransform = camera.GetOwner()->GetTransform();
    cameraMove = cameraTransform->TransformVectorNoScale(cameraMove);

    if (cameraUp)
    {
        cameraMove.z += 5.0f * deltaTime;
    }
    if (cameraDown)
    {
        cameraMove.z -= 5.0f * deltaTime;
    }

    const Vector3 currCameraPosition = cameraTransform->GetPosition();
    cameraTransform->SetPosition(currCameraPosition + cameraMove);
}

void ViewportWidget::UpdateCameraRotation()
{
    if (!isCameraRotating)
        return;

    TransformComponent* cameraTransform = camera.GetOwner()->GetTransform();

    Vector3 rotation = cameraTransform->GetRotation();
    rotation.x -= pendingCameraLookDelta.y * 0.1f;
    rotation.z -= pendingCameraLookDelta.x * 0.1f;

    rotation.x = std::clamp(rotation.x, -89.0f, 89.0f);

    cameraTransform->SetRotation(rotation);

    pendingCameraLookDelta = Vector2::Zero;
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

bool ViewportWidget::SaveCurrScene()
{
    SceneSystem& sceneSystem = engine.GetSceneSystem();
    Scene* scene = sceneSystem.GetSceneByType(SceneType::Game);

    if (!scene)
        return false;

    const VirtualPath& scenePath = scene->GetFilePath();

    if (scenePath.ToString().empty())
    {
        Logger::Log("Failed to save scene: Scene Path is empty.");
        return false;
    }

    VirtualFilesystem& filesystem = engine.GetAssetSystem().GetFilesystem();

    const std::string sceneText = YAML::Dump(scene->Serialize());

    if (!filesystem.WriteText(scenePath, sceneText))
    {
        Logger::Log("Failed to save scene: " + scenePath.ToString());
        return false;
    }

    Logger::Log("Scene saved: " + scenePath.ToString());
    return true;
}

} // namespace URay

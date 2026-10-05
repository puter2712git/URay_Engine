#include "TransformGizmo.h"

#include "Editor/Gizmo/Strategy/TranslationGizmoStrategy.h"
#include "Editor/Gizmo/Strategy/RotationGizmoStrategy.h"
#include "Editor/Gizmo/Strategy/ScaleGizmoStrategy.h"

#include "Engine/Asset/AssetDatabase.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/EngineAsset.h"
#include "Engine/Component/Render/CameraComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneSystem.h"
#include "Engine/Scene/Unit.h"

#include "Render/RenderSystem.h"
#include "Render/Rendering/Object/Drawable/GizmoObject.h"
#include "Render/Rendering/Scene/RenderScene.h"
#include "Render/Rendering/Scene/SceneSystem.h"

#include <algorithm>

namespace URay
{

TransformGizmo::TransformGizmo()
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();
    material = assetDatabase.Find<Material>(EngineAsset::MeshMaterial);

    strategies.push_back(std::make_unique<TranslationGizmoStrategy>(*this));
    strategies.push_back(std::make_unique<RotationGizmoStrategy>(*this));
    strategies.push_back(std::make_unique<ScaleGizmoStrategy>(*this));
    currentStrategy = FindStrategy(GizmoMode::Translation);

    Scene* editorScene = gEngine->GetSceneSystem().GetSceneByType(SceneType::Editor);
    Render::RenderScene* renderScene = gEngine->GetRenderSystem().GetSceneSystem().GetRenderScene(editorScene);
    auto renderGizmo = std::make_unique<Render::GizmoObject>(Render::GizmoObjectState{});
    renderObject = renderGizmo.get();
    gEngine->GetRenderSystem().GetSceneSystem().AddObject(*renderScene, std::move(renderGizmo), nullptr);
}

TransformGizmo::~TransformGizmo() = default;

TransformComponent* TransformGizmo::GetTargetTransform()
{
    return targetUnit ? targetUnit->GetTransform() : nullptr;
}

const TransformComponent* TransformGizmo::GetTargetTransform() const
{
    return targetUnit ? targetUnit->GetTransform() : nullptr;
}

void TransformGizmo::SetMode(GizmoMode newMode)
{
    if (dragging)
        EndDragging();

    currentStrategy = FindStrategy(newMode);
}

GizmoMode TransformGizmo::GetMode() const
{
    return currentStrategy ? currentStrategy->GetMode() : GizmoMode::Translation;
}

void TransformGizmo::Update(const Vector2& position, CameraComponent& camera)
{
    if (!targetUnit || !GetTargetTransform())
    {
        if (renderObject)
            renderObject->SetEnabled(false);
        return;
    }

    UpdateDragging(position, camera);

    const Matrix& targetWorld = GetTargetTransform()->GetWorldMatrix();
    Vector3 xAxis(targetWorld.m00, targetWorld.m01, targetWorld.m02);
    Vector3 yAxis(targetWorld.m10, targetWorld.m11, targetWorld.m12);
    Vector3 zAxis(targetWorld.m20, targetWorld.m21, targetWorld.m22);
    Matrix noScaleWorld = Matrix::Identity;
    noScaleWorld.m00 = xAxis.GetNormalized().x; noScaleWorld.m01 = xAxis.GetNormalized().y; noScaleWorld.m02 = xAxis.GetNormalized().z;
    noScaleWorld.m10 = yAxis.GetNormalized().x; noScaleWorld.m11 = yAxis.GetNormalized().y; noScaleWorld.m12 = yAxis.GetNormalized().z;
    noScaleWorld.m20 = zAxis.GetNormalized().x; noScaleWorld.m21 = zAxis.GetNormalized().y; noScaleWorld.m22 = zAxis.GetNormalized().z;
    noScaleWorld.m30 = targetWorld.m30; noScaleWorld.m31 = targetWorld.m31; noScaleWorld.m32 = targetWorld.m32;

    const float distance = (camera.GetOwner()->GetTransform()->GetPosition() - GetTargetTransform()->GetPosition()).GetLength();
    const float scale = std::clamp(distance * scaleFactor, minScale, maxScale);
    currentStrategy->UpdateHandles(Matrix::MakeScale(Vector3(scale, scale, scale)) * noScaleWorld);
    UpdateRenderObject();
}

void TransformGizmo::StartDragging(
    const Vector2& position,
    GizmoHandleId handleId,
    const CameraComponent& camera)
{
    if (!currentStrategy)
        return;

    selectedHandle = handleId;
    dragging = true;
    currentStrategy->StartDragging(position, handleId, camera);
}

void TransformGizmo::UpdateDragging(
    const Vector2& position,
    const CameraComponent& camera)
{
    if (!currentStrategy || !dragging)
        return;

    currentStrategy->UpdateDragging(position, camera);
}

void TransformGizmo::EndDragging()
{
    if (!currentStrategy)
        return;

    currentStrategy->EndDragging();
    dragging = false;
    selectedHandle = GizmoHandleId::None;
}

const std::vector<GizmoHandle>& TransformGizmo::GetHandles() const
{
    static const std::vector<GizmoHandle> empty;
    return currentStrategy ? currentStrategy->GetHandles() : empty;
}

GizmoStrategy* TransformGizmo::FindStrategy(GizmoMode mode) const
{
    for (const auto& strategy : strategies)
        if (strategy->GetMode() == mode)
            return strategy.get();
    return nullptr;
}

void TransformGizmo::UpdateRenderObject()
{
    if (!renderObject || !currentStrategy)
        return;

    Render::GizmoObjectState state = {};
    state.material = material;
    for (const GizmoHandle& handle : currentStrategy->GetHandles())
    {
        Color color = handle.color;
        if (handle.id == selectedHandle || (!dragging && handle.id == hoveredHandle))
            color = Color::Yellow;
        state.handles.push_back({ .worldMatrix = handle.worldMatrix, .colorTint = color, .mesh = handle.mesh });
    }
    renderObject->SetEnabled(true);
    renderObject->Update(state);
}

} // namespace URay

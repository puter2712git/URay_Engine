#include "TransformGizmo.h"

#include <Editor/Gizmo/Strategy/RotationGizmoStrategy.h>
#include <Editor/Gizmo/Strategy/ScaleGizmoStrategy.h>
#include <Editor/Gizmo/Strategy/TranslationGizmoStrategy.h>

#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/EngineAsset.h>
#include <Engine/Asset/Mesh/Mesh.h>
#include <Engine/Component/Render/CameraComponent.h>
#include <Engine/Component/TransformComponent.h>
#include <Engine/Engine.h>
#include <Engine/Scene/Scene.h>
#include <Engine/Scene/SceneSystem.h>
#include <Engine/Scene/Unit.h>

#include <Render/RenderSystem.h>
#include <Render/Rendering/Object/Drawable/GizmoObject.h>
#include <Render/Rendering/Scene/RenderScene.h>
#include <Render/Rendering/Scene/SceneSystem.h>
#include <Render/RHI/Vertex/Vertex.h>

#include <algorithm>
#include <Core/Math/Math.h>
#include <Core/Math/Ray.h>
#include <limits>
#include <unordered_set>

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

void TransformGizmo::SetTargets(const std::vector<Unit*>& units, Unit* newActiveUnit)
{
    if (dragging)
        EndDragging();

    selectedUnits = units;
    activeUnit = newActiveUnit;
}

TransformComponent* TransformGizmo::GetActiveTransform()
{
    return activeUnit ? activeUnit->GetTransform() : nullptr;
}

const TransformComponent* TransformGizmo::GetActiveTransform() const
{
    return activeUnit ? activeUnit->GetTransform() : nullptr;
}

std::vector<TransformComponent*> TransformGizmo::GetTargetTransforms() const
{
    std::unordered_set<Unit*> selectedSet;
    selectedSet.reserve(selectedUnits.size());
    for (Unit* unit : selectedUnits)
        if (unit)
            selectedSet.insert(unit);

    std::vector<TransformComponent*> transforms;
    transforms.reserve(selectedSet.size());
    for (Unit* unit : selectedUnits)
    {
        if (!unit)
            continue;

        bool hasSelectedAncestor = false;
        for (Unit* ancestor = unit->GetParent(); ancestor; ancestor = ancestor->GetParent())
        {
            if (selectedSet.contains(ancestor))
            {
                hasSelectedAncestor = true;
                break;
            }
        }

        if (!hasSelectedAncestor)
            if (TransformComponent* transform = unit->GetTransform())
                transforms.push_back(transform);
    }
    return transforms;
}

Vector3 TransformGizmo::GetPivotPosition() const
{
    const std::vector<TransformComponent*> transforms = GetTargetTransforms();
    if (transforms.empty())
        return Vector3::Zero;

    Vector3 position = Vector3::Zero;
    for (const TransformComponent* transform : transforms)
        position = position + transform->GetWorldPosition();
    return position / static_cast<float>(transforms.size());
}

void TransformGizmo::SetMode(GizmoMode newMode)
{
    if (dragging)
        EndDragging();

    currentStrategy = FindStrategy(newMode);
}

std::optional<GizmoHandleId> TransformGizmo::HitTest(const Ray& worldRay) const
{
    if (!HasTargets() || !GetActiveTransform())
        return std::nullopt;

    float nearestDistance = std::numeric_limits<float>::max();
    std::optional<GizmoHandleId> hitHandle = std::nullopt;

    for (const GizmoHandle& handle : GetHandles())
    {
        const Mesh* mesh = handle.mesh;
        if (!mesh)
            continue;

        const Matrix inverseWorld = handle.worldMatrix.Inverse();
        const Ray localRay{
            .origin = inverseWorld.TransformPoint(worldRay.origin),
            .direction = inverseWorld.TransformVector(worldRay.direction),
        };

        const std::vector<Render::VertexPNT>& vertices = mesh->GetVertices();
        const std::vector<uint32>& indices = mesh->GetIndices();

        for (size_t index = 0; index + 2 < indices.size(); index += 3)
        {
            const Vector3 p0 = vertices[indices[index]].position;
            const Vector3 p1 = vertices[indices[index + 1]].position;
            const Vector3 p2 = vertices[indices[index + 2]].position;

            float distance = 0.0f;
            if (!Math::IntersectLineTriangle(localRay, p0, p1, p2, distance) ||
                distance >= nearestDistance)
            {
                continue;
            }

            nearestDistance = distance;
            hitHandle = handle.id;
        }
    }

    return hitHandle;
}

GizmoMode TransformGizmo::GetMode() const
{
    return currentStrategy ? currentStrategy->GetMode() : GizmoMode::Translation;
}

void TransformGizmo::Update(const Vector2& position, CameraComponent& camera)
{
    if (!HasTargets() || !GetActiveTransform())
    {
        if (renderObject)
            renderObject->SetEnabled(false);
        return;
    }

    UpdateDragging(position, camera);

    const Vector3 targetPosition = GetPivotPosition();
    const Vector3 targetRotation = GetActiveTransform()->GetWorldRotation();

    const float distance = (camera.GetOwner()->GetTransform()->GetWorldPosition() - targetPosition).GetLength();
    const float scale = std::clamp(distance * scaleFactor, minScale, maxScale);

    if (isLocal || GetMode() == GizmoMode::Scale)
    {
        const Matrix relativeMatrix =
            Matrix::MakeScale(Vector3(scale, scale, scale)) * Matrix::MakeRotation(targetRotation) * Matrix::MakeTranslation(targetPosition);

        currentStrategy->UpdateHandles(relativeMatrix);
    }
    else
    {
        const Matrix relativeMatrix =
            Matrix::MakeScale(Vector3(scale, scale, scale)) * Matrix::MakeTranslation(targetPosition);

        currentStrategy->UpdateHandles(relativeMatrix);
    }
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

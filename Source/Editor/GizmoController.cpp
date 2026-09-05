#include "GizmoController.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Component/Render/CameraComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneSystem.h"
#include "Engine/Scene/Unit.h"

#include "Core/Math/Math.h"

#include "Render/DrawCommand/DrawCommandBuilder.h"
#include "Render/DrawCommand/DrawCommandContext.h"
#include "Render/Scene/Object/Drawable/GizmoObject.h"
#include "Render/Scene/RenderScene.h"

#include <algorithm>

namespace URay
{

using namespace Render;

GizmoController::GizmoController(Engine& engine)
{
    AssetSystem& assetSystem = engine.GetAssetSystem();
    SceneSystem& sceneSystem = engine.GetSceneSystem();

    meshes[static_cast<size_t>(GizmoMode::Translation)] = assetSystem.GetDefaultAssets().arrowMesh;
    meshes[static_cast<size_t>(GizmoMode::Rotation)] = assetSystem.GetDefaultAssets().rotationGizmoMesh;
    meshes[static_cast<size_t>(GizmoMode::Scale)] = assetSystem.GetDefaultAssets().scaleGizmoMesh;

    material = assetSystem.GetDefaultAssets().meshMaterial;

    Scene* editorScene = sceneSystem.GetSceneByType(SceneType::Editor);
    RenderScene* renderScene = editorScene->GetRenderScene();

    GizmoObjectState objectState = {};
    objectState.worldMatrices.push_back(GetWorldMatrix(0));
    objectState.worldMatrices.push_back(GetWorldMatrix(1));
    objectState.worldMatrices.push_back(GetWorldMatrix(2));
    objectState.mesh = GetMesh();
    objectState.material = material;
    objectState.colorTints.push_back(Color::Red);
    objectState.colorTints.push_back(Color::Green);
    objectState.colorTints.push_back(Color::Blue);

    std::unique_ptr<GizmoObject> gizmoObject = std::make_unique<GizmoObject>(objectState);
    renderObject = gizmoObject.get();
    renderScene->Add(std::move(gizmoObject));
}

void GizmoController::Update(const Vector2& targetPosition, CameraComponent& camera)
{
    if (!targetUnit || !targetUnit->GetTransform())
    {
        if (renderObject)
        {
            renderObject->SetEnabled(false);
        }
        return;
    }

    UpdateGizmo(targetPosition, camera);

    const TransformComponent* transform = targetUnit->GetTransform();
    const Matrix targetWorldMatrix = transform->GetWorldMatrix();

    Vector3 xAxis(targetWorldMatrix.m00, targetWorldMatrix.m01, targetWorldMatrix.m02);
    Vector3 yAxis(targetWorldMatrix.m10, targetWorldMatrix.m11, targetWorldMatrix.m12);
    Vector3 zAxis(targetWorldMatrix.m20, targetWorldMatrix.m21, targetWorldMatrix.m22);

    xAxis = xAxis.GetNormalized();
    yAxis = yAxis.GetNormalized();
    zAxis = zAxis.GetNormalized();

    Matrix noScaleWorld = Matrix::Identity;

    noScaleWorld.m00 = xAxis.x;
    noScaleWorld.m01 = xAxis.y;
    noScaleWorld.m02 = xAxis.z;

    noScaleWorld.m10 = yAxis.x;
    noScaleWorld.m11 = yAxis.y;
    noScaleWorld.m12 = yAxis.z;

    noScaleWorld.m20 = zAxis.x;
    noScaleWorld.m21 = zAxis.y;
    noScaleWorld.m22 = zAxis.z;

    noScaleWorld.m30 = targetWorldMatrix.m30;
    noScaleWorld.m31 = targetWorldMatrix.m31;
    noScaleWorld.m32 = targetWorldMatrix.m32;

    const Matrix gizmoScaleMatrix =
        Matrix::MakeScale(Vector3(1.0f, 1.0f, 1.0f));

    const Matrix targetGizmoWorld = gizmoScaleMatrix * noScaleWorld;

    matrices[0][0] = Matrix::MakeRotationZ(-90.0f) * targetGizmoWorld;
    matrices[0][1] = targetGizmoWorld;
    matrices[0][2] = Matrix::MakeRotationX(90.0f) * targetGizmoWorld;

    matrices[1][0] = Matrix::MakeRotationY(90.0f) * targetGizmoWorld;
    matrices[1][1] = Matrix::MakeRotationX(90.0f) * targetGizmoWorld;
    matrices[1][2] = targetGizmoWorld;

    matrices[2] = matrices[0];

    UpdateRenderObject();
}

void GizmoController::StartDragging(const Vector2& clickPos, int selectedAxis, CameraComponent& camera)
{
    TransformComponent* cameraTransform = camera.GetOwner()->GetTransform();

    isDragging = true;
    SetSelectedAxis(selectedAxis);

    const Vector3 cameraWorldFarPos = camera.ScreenToWorld(Vector3(clickPos.x, clickPos.y, 1.0f));
    dragStartWorldPos = cameraTransform->GetPosition();
    dragStartLineDir = cameraWorldFarPos - dragStartWorldPos;
    targetInitPosition = targetUnit->GetTransform()->GetPosition();
    targetInitRotation = targetUnit->GetTransform()->GetRotation();
    targetInitScale = targetUnit->GetTransform()->GetScale();
    dragAxisDir = GetCurrAxisDir();
}

void GizmoController::EndDragging()
{
    isDragging = false;
    SetSelectedAxis(-1);

    dragStartWorldPos = Vector3::Zero;
    dragStartLineDir = Vector3::Zero;
    targetInitPosition = Vector3::Zero;
    targetInitRotation = Vector3::Zero;
    targetInitScale = Vector3::One;
    dragAxisDir = Vector3::Zero;
}

const TransformComponent* GizmoController::GetTargetTransform() const
{
    if (!targetUnit || !targetUnit->GetTransform())
        return nullptr;

    return targetUnit->GetTransform();
}

void GizmoController::UpdateGizmo(const Vector2& targetPosition, CameraComponent& camera)
{
    if (!IsDragging())
        return;

    switch (mode)
    {
    case GizmoMode::Translation:
        UpdateTranslation(targetPosition, camera);
        break;
    case GizmoMode::Rotation:
        UpdateRotation(targetPosition, camera);
        break;
    case GizmoMode::Scale:
        UpdateScale(targetPosition, camera);
        break;
    default:
        break;
    }
}

void GizmoController::UpdateTranslation(const Vector2& targetPosition, CameraComponent& camera)
{
    TransformComponent* cameraTransform = camera.GetOwner()->GetTransform();
    Vector3 cameraForward = cameraTransform->GetForward();
    Vector3 axisDir = dragAxisDir;

    Vector3 currCameraFarPos = camera.ScreenToWorld(Vector3(targetPosition.x, targetPosition.y, 1.0f));
    Vector3 currLineDir = currCameraFarPos - dragStartWorldPos;

    Vector3 planeNormal = Vector3::Cross(axisDir, Vector3::Cross(cameraForward, axisDir).GetNormalized()).GetNormalized();

    Vector3 startHitPoint;
    Vector3 currHitPoint;

    Math::IntersectLinePlane(
        dragStartWorldPos, dragStartLineDir,
        targetUnit->GetTransform()->GetPosition(), planeNormal,
        startHitPoint);
    Math::IntersectLinePlane(
        dragStartWorldPos, currLineDir,
        targetUnit->GetTransform()->GetPosition(), planeNormal,
        currHitPoint);

    float startAxisPos = Vector3::Dot(startHitPoint - targetInitPosition, axisDir);
    float currAxisPos = Vector3::Dot(currHitPoint - targetInitPosition, axisDir);

    float translationAmount = currAxisPos - startAxisPos;

    Vector3 movedPos = targetInitPosition + axisDir * translationAmount;
    targetUnit->GetTransform()->SetPosition(movedPos);
}

void GizmoController::UpdateRotation(const Vector2& targetPosition, CameraComponent& camera)
{
    Vector3 startHitPoint;
    if (!GetDragPlaneHitPoint(dragStartLineDir, startHitPoint))
        return;

    Vector3 currCameraFarPos = camera.ScreenToWorld(Vector3(targetPosition.x, targetPosition.y, 1.0f));
    Vector3 currHitPoint;
    if (!GetDragPlaneHitPoint(currCameraFarPos - dragStartWorldPos, currHitPoint))
        return;

    Vector3 startVector = (startHitPoint - targetInitPosition).GetNormalized();
    Vector3 currVector = (currHitPoint - targetInitPosition).GetNormalized();
    if (startVector.GetSquaredLength() < Math::EPSILON || currVector.GetSquaredLength() < Math::EPSILON)
        return;

    float sinAngle = Vector3::Dot(dragAxisDir, Vector3::Cross(startVector, currVector));
    float cosAngle = Vector3::Dot(startVector, currVector);
    float angleDegrees = Math::RadToDeg(std::atan2(sinAngle, cosAngle));

    Vector3 rotation = targetInitRotation;
    switch (selectedAxis)
    {
    case static_cast<int>(Axis::X):
        rotation.x += angleDegrees;
        break;
    case static_cast<int>(Axis::Y):
        rotation.y += angleDegrees;
        break;
    case static_cast<int>(Axis::Z):
        rotation.z += angleDegrees;
        break;
    default:
        return;
    }

    targetUnit->GetTransform()->SetRotation(rotation);
}

void GizmoController::UpdateScale(const Vector2& targetPosition, CameraComponent& camera)
{
    TransformComponent* cameraTransform = camera.GetOwner()->GetTransform();

    Vector3 planeNormal = Vector3::Cross(
                              dragAxisDir,
                              Vector3::Cross(cameraTransform->GetForward(), dragAxisDir).GetNormalized())
                              .GetNormalized();

    Vector3 startHitPoint;
    if (!Math::IntersectLinePlane(dragStartWorldPos, dragStartLineDir, targetInitPosition, planeNormal, startHitPoint))
        return;

    Vector3 currCameraFarPos = camera.ScreenToWorld(Vector3(targetPosition.x, targetPosition.y, 1.0f));
    Vector3 currHitPoint;
    if (!Math::IntersectLinePlane(dragStartWorldPos, currCameraFarPos - dragStartWorldPos, targetInitPosition, planeNormal, currHitPoint))
        return;

    float amount = Vector3::Dot(currHitPoint - startHitPoint, dragAxisDir);
    constexpr float minimumScale = 0.01f;

    Vector3 scale = targetInitScale;
    switch (selectedAxis)
    {
    case static_cast<int>(Axis::X):
        scale.x = std::max(minimumScale, targetInitScale.x + amount);
        break;
    case static_cast<int>(Axis::Y):
        scale.y = std::max(minimumScale, targetInitScale.y + amount);
        break;
    case static_cast<int>(Axis::Z):
        scale.z = std::max(minimumScale, targetInitScale.z + amount);
        break;
    default:
        return;
    }

    targetUnit->GetTransform()->SetScale(scale);
}

void GizmoController::UpdateRenderObject()
{
    if (!renderObject)
        return;

    GizmoObjectState state = {};
    state.worldMatrices.clear();
    state.worldMatrices.push_back(GetWorldMatrix(0));
    state.worldMatrices.push_back(GetWorldMatrix(1));
    state.worldMatrices.push_back(GetWorldMatrix(2));
    state.colorTints.clear();

    int32 currAxis = selectedAxis;
    if (currAxis == -1)
    {
        currAxis = hoveredAxis;
    }

    state.colorTints.push_back(Color::Red);
    state.colorTints.push_back(Color::Green);
    state.colorTints.push_back(Color::Blue);

    if (currAxis != -1)
    {
        state.colorTints[currAxis] = Color::Yellow;
    }

    state.mesh = GetMesh();
    state.material = material;

    renderObject->SetEnabled(true);
    renderObject->Update(state);
}

bool GizmoController::GetDragPlaneHitPoint(const Vector3& lineDir, Vector3& outHitPoint) const
{
    return Math::IntersectLinePlane(dragStartWorldPos, lineDir, targetInitPosition, dragAxisDir, outHitPoint);
}

Vector3 GizmoController::GetCurrAxisDir()
{
    if (selectedAxis == -1)
        return Vector3::Zero;

    Vector3 axisDir = Vector3::Zero;
    switch (selectedAxis)
    {
    case 0:
        axisDir = Vector3::Right;
        break;
    case 1:
        axisDir = Vector3::Forward;
        break;
    case 2:
        axisDir = Vector3::Up;
        break;
    }

    axisDir = GetTargetTransform()->TransformVectorNoScale(axisDir);
    return axisDir;
}

} // namespace URay

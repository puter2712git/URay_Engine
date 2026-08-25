#pragma once

#include "Core/Math/Matrix.h"
#include "Core/Math/Vector2.h"
#include "Core/Math/Vector3.h"

#include <array>
#include <cstdint>

namespace URay
{

class Unit;
class Mesh;
class Material;
class AssetSystem;
class TransformComponent;
class CameraComponent;

namespace Render
{
class DrawCommandBuilder;
}

enum class Axis : uint8_t
{
    X,
    Y,
    Z,

    Count
};

enum class GizmoMode : uint8_t
{
    Translation,
    Rotation,
    Scale,

    Count
};

class GizmoController
{
public:
    GizmoController(AssetSystem& assetSystem);

public:
    void Update(const Vector2& targetPosition, CameraComponent& camera);
    void Draw(Render::DrawCommandBuilder& builder);

    void StartDragging(const Vector2& clickPos, int selectedAxis, CameraComponent& camera);
    void EndDragging();

    Unit* GetTarget() const { return targetUnit; }
    void SetTarget(Unit* unit) { targetUnit = unit; }

    void SetHoveredAxis(int axis) { hoveredAxis = axis; }

    int GetSelectedAxis() const { return selectedAxis; }
    void SetSelectedAxis(int axis) { selectedAxis = axis; }

    bool IsDragging() const { return isDragging; }

    GizmoMode GetMode() const { return mode; }
    int GetModeIndex() const { return static_cast<int>(mode); }
    void SetMode(GizmoMode mode) { this->mode = mode; }

    Mesh* GetMesh() { return meshes[static_cast<size_t>(mode)]; }

    const Matrix& GetWorldMatrix(size_t axis) const { return matrices[GetModeIndex()][axis]; }

    const TransformComponent* GetTargetTransform() const;

private:
    void UpdateGizmo(const Vector2& targetPosition, CameraComponent& camera);

    void UpdateTranslation(const Vector2& targetPosition, CameraComponent& camera);
    void UpdateRotation(const Vector2& targetPosition, CameraComponent& camera);
    void UpdateScale(const Vector2& targetPosition, CameraComponent& camera);

    Vector3 GetCurrAxisDir();
    bool GetDragPlaneHitPoint(const Vector3& lineDir, Vector3& outHitPoint) const;

private:
    const float scaleFactor = 0.1f;
    const float minScale = 0.25f;
    const float maxScale = 10.0f;

    GizmoMode mode = GizmoMode::Translation;

    int hoveredAxis = -1;
    int selectedAxis = -1;

    bool isDragging = false;

    Vector3 targetInitPosition = Vector3::Zero;
    Vector3 targetInitRotation = Vector3::Zero;
    Vector3 targetInitScale = Vector3::One;
    Vector3 dragStartWorldPos = Vector3::Zero;
    Vector3 dragStartLineDir = Vector3::Zero;
    Vector3 dragAxisDir = Vector3::Zero;

    Unit* targetUnit = nullptr;

    std::array<Mesh*, static_cast<size_t>(GizmoMode::Count)> meshes;
    std::array<std::array<Matrix, static_cast<size_t>(Axis::Count)>, static_cast<size_t>(GizmoMode::Count)> matrices;

    Material* material = nullptr;
};

} // namespace URay

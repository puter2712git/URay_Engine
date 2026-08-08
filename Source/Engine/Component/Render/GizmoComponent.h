#pragma once

#include "Engine/Component/Render/RenderComponent.h"
#include "Engine/Component/TransformComponent.h"

#include "Core/Math/Matrix.h"
#include "Core/Math/Vector2.h"

#include <array>
#include <cstdint>

namespace URay
{

class MeshAsset;
class Material;
class Unit;

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

class GizmoComponent : public RenderComponent
{
    URAY_CLASS(GizmoComponent, RenderComponent)

public:
    GizmoComponent();

public:
    void Update(float deltaTime) override;
    void SubmitCommand(DrawCommandBuilder& builder) override;

    void StartDragging(const Vector2& clickPos, int selectedAxis);
    void EndDragging();

    Unit* GetTarget() const { return targetUnit; }
    void SetTarget(Unit* unit) { targetUnit = unit; }

    void SetHoveredAxis(int axis) { hoveredAxis = axis; }

    int GetSelectedAxis() const { return selectedAxis; }
    void SetSelectedAxis(int axis) { selectedAxis = axis; }

    bool IsDragging() const { return isDragging; }
    void SetDragging(bool dragging) { isDragging = dragging; }

    GizmoMode GetCurrMode() const { return currMode; }
    int GetCurrModeIndex() const { return static_cast<int>(currMode); }
    void SetMode(GizmoMode mode) { currMode = mode; }

    MeshAsset* GetCurrMesh() { return meshes[static_cast<size_t>(currMode)]; }

    const Matrix& GetCurrMatrix(size_t axis) const { return matrices[GetCurrModeIndex()][axis]; }

    const TransformComponent* GetTargetTransform() const;

private:
    void UpdateGizmo();

    void UpdateGizmoTranslation();
    void UpdateGizmoRotation();
    void UpdateGizmoScale();

    Vector3 GetCurrAxisDir();
    bool GetDragPlaneHitPoint(const Vector3& lineDir, Vector3& outHitPoint) const;

private:
    GizmoMode currMode = GizmoMode::Translation;

    int hoveredAxis = -1;

    int selectedAxis = -1;
    bool isDragging = false;

    Vector3 targetInitPos = Vector3::Zero;
    Vector3 targetInitRotation = Vector3::Zero;
    Vector3 targetInitScale = Vector3::One;
    Vector3 dragStartWorldPos = Vector3::Zero;
    Vector3 dragStartLineDir = Vector3::Zero;
    Vector3 dragAxisDir = Vector3::Zero;

    Unit* targetUnit = nullptr;

    std::array<MeshAsset*, static_cast<size_t>(GizmoMode::Count)> meshes;
    std::array<std::array<Matrix, static_cast<size_t>(Axis::Count)>, static_cast<size_t>(GizmoMode::Count)> matrices;

    Material* material = nullptr;
};

} // namespace URay

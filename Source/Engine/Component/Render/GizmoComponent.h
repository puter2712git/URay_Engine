#pragma once

#include "Engine/Component/Render/RenderComponent.h"
#include "Engine/Component/TransformComponent.h"

#include "Core/Math/Matrix.h"

#include <array>
#include <cstdint>

namespace URay
{

class Mesh;
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
public:
    GizmoComponent();

public:
    void Update(float deltaTime) override;
    void SubmitCommand(DrawCommandBuilder& builder) override;

    Unit* GetTarget() const
    {
        return targetUnit;
    }
    void SetTarget(Unit* unit)
    {
        targetUnit = unit;
    }

    void SetHoveredAxis(int axis)
    {
        hoveredAxis = axis;
    }

    int GetSelectedAxis() const
    {
        return selectedAxis;
    }
    void SetSelectedAxis(int axis)
    {
        selectedAxis = axis;
    }

    bool IsDragging() const
    {
        return isDragging;
    }
    void SetDragging(bool dragging)
    {
        isDragging = dragging;
    }

    Mesh* GetCurrMesh()
    {
        return meshes[static_cast<size_t>(currMode)];
    }

    const Matrix& GetCurrMatrix(size_t axis) const
    {
        return matrices[axis];
    }

    const TransformComponent* GetTargetTransform() const;

private:
    GizmoMode currMode = GizmoMode::Translation;
    int hoveredAxis = -1;

    int selectedAxis = -1;
    bool isDragging = false;

    Unit* targetUnit = nullptr;

    std::array<Mesh*, static_cast<size_t>(GizmoMode::Count)> meshes;
    std::array<Matrix, static_cast<size_t>(Axis::Count)> matrices;

    Material* material = nullptr;
};

} // namespace URay

#pragma once

#include "Engine/Component/Render/RenderComponent.h"

#include <array>
#include <cstdint>

namespace URay
{

class Mesh;
class Material;
class Unit;

enum class Axis : uint8_t
{
    X = 1,
    Y,
    Z
};

enum class GizmoMode : uint8_t
{
    Translation,
    Rotation,
    Scale,
};

class GizmoComponent : public RenderComponent
{
public:
    GizmoComponent();

public:
    void SubmitCommand(DrawCommandBuilder& builder) override;

    void SetTarget(Unit* unit)
    {
        targetUnit = unit;
    }

private:
    GizmoMode currMode = GizmoMode::Translation;
    Unit* targetUnit = nullptr;

    std::array<Mesh*, 3> meshes;
    Material* material = nullptr;
};

} // namespace URay

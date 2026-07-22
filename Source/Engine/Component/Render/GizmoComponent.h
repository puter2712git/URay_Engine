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
    void SubmitCommand(DrawCommandBuilder& builder) override;

    void SetTarget(Unit* unit)
    {
        targetUnit = unit;
    }

private:
    GizmoMode currMode = GizmoMode::Translation;
    Unit* targetUnit = nullptr;

    std::array<Mesh*, static_cast<size_t>(GizmoMode::Count)> meshes;
    Material* material = nullptr;
};

} // namespace URay

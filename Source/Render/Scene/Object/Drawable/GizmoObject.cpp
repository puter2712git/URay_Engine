#include "GizmoObject.h"

#include "Render/DrawCommand/DrawCommandBuilder.h"

#include "Engine/Asset/Mesh/Mesh.h"

namespace URay::Render
{

GizmoObject::GizmoObject(const GizmoObjectState& state)
{
    Update(state);
}

GizmoObject::~GizmoObject() = default;

void GizmoObject::Update(const GizmoObjectState& state)
{
    worldMatrices = state.worldMatrices;
    mesh = state.mesh;
    material = state.material;
    colorTints = state.colorTints;
}

void GizmoObject::Submit(DrawCommandBuilder& builder) const
{
    if (!mesh)
        return;

    size_t count = worldMatrices.size();

    for (size_t i = 0; i < count; ++i)
    {
        builder.BuildGizmo(
            { .worldMatrix = worldMatrices[i],
              .colorTint = colorTints[i],
              .mesh = mesh,
              .material = material });
    }
}

} // namespace URay::Render

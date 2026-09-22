#include "GizmoObject.h"

#include "Render/Rendering/DrawCommand/DrawCommandBuilder.h"

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
    this->state = state;
    NotifyUpdated();
}

void GizmoObject::Submit(DrawCommandBuilder& builder) const
{
    if (!state.mesh)
        return;

    size_t count = state.worldMatrices.size();

    for (size_t i = 0; i < count; ++i)
    {
        builder.BuildGizmo(
            { .worldMatrix = state.worldMatrices[i],
              .colorTint = state.colorTints[i],
              .mesh = state.mesh,
              .material = state.material });
    }
}

} // namespace URay::Render

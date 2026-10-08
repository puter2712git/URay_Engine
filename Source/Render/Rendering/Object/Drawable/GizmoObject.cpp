#include "GizmoObject.h"

#include <Render/Rendering/DrawCommand/DrawCommandBuilder.h>

#include <Engine/Asset/Mesh/Mesh.h>

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
    if (!state.material)
        return;

    for (const GizmoObjectState::Handle& handle : state.handles)
    {
        if (!handle.mesh)
            continue;
        builder.BuildGizmo(
            { .worldMatrix = handle.worldMatrix,
              .colorTint = handle.colorTint,
              .mesh = handle.mesh,
              .material = state.material });
    }
}

} // namespace URay::Render

#include "GizmoComponent.h"

#include "Engine/Engine.h"
#include "Engine/Mesh/MeshManager.h"

#include "Render/DrawCommand/DrawCommandContext.h"

namespace URay
{

GizmoComponent::GizmoComponent()
{
    meshes[static_cast<size_t>(GizmoMode::Translation)] = gEngine->GetMeshManager()->GetMesh("cylinder");
}

void GizmoComponent::SubmitCommand(DrawCommandBuilder& builder)
{
    MeshCommandContext context = {};
}

} // namespace URay

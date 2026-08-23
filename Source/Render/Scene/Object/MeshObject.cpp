#include "MeshObject.h"

namespace URay::RHI
{

MeshObject::MeshObject(const Mesh& mesh, const Matrix& worldMatrix)
{
    vertices = mesh.GetVertices();
    indices = mesh.GetIndices();
    sections = mesh.GetSections();

    this->worldMatrix = worldMatrix;
    worldBounds = mesh.GetLocalBounds().Transform(worldMatrix);
}

MeshObject::~MeshObject() = default;

void MeshObject::Update(const MeshObjectState& state)
{
    worldMatrix = state.worldMatrix;

    vertices = state.mesh->GetVertices();
    indices = state.mesh->GetIndices();
    sections = state.mesh->GetSections();

    materials = state.materials;

    worldBounds = state.mesh->GetLocalBounds().Transform(worldMatrix);
}

} // namespace URay::RHI

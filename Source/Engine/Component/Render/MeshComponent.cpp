#include "MeshComponent.h"

#include "Engine/Mesh/Mesh.h"

namespace URay
{

MeshComponent::MeshComponent()
{
    mesh = new Mesh();
}

void MeshComponent::SubmitCommand()
{
}

} // namespace URay

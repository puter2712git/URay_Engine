#include "BoxComponent.h"

#include "Engine/Mesh/Mesh.h"

namespace URay
{

BoxComponent::BoxComponent()
{
    mesh = new Mesh();
}

void BoxComponent::SubmitCommand()
{
}

} // namespace URay

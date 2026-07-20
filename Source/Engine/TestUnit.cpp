#include "TestUnit.h"

#include "Engine/Component/Render/MeshComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"

#include "Mesh/MeshManager.h"

namespace URay
{

TestUnit::TestUnit()
{
    Mesh* mesh = gEngine->GetMeshManager()->GetMesh("box");

    TransformComponent* transform = new TransformComponent();

    MeshComponent* meshComponent = new MeshComponent();
    meshComponent->SetMesh(mesh);

    AddComponent(transform);
    AddComponent(meshComponent);
}

} // namespace URay

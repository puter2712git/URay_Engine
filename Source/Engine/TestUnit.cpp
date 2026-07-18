#include "TestUnit.h"

#include "Engine/Component/Render/MeshComponent.h"
#include "Engine/Engine.h"

#include "Mesh/MeshManager.h"

namespace URay
{

TestUnit::TestUnit()
{
    Mesh* mesh = gEngine->GetMeshManager()->GetMesh("box");

    MeshComponent* meshComponent = new MeshComponent();
    meshComponent->SetMesh(mesh);

    AddComponent(meshComponent);
}

} // namespace URay

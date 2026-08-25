#include "Engine/Component/Render/MeshComponent.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Asset/Material/MaterialManager.h"
#include "Engine/Asset/Mesh/Mesh.h"
#include "Engine/Object/Class/Class.h"
#include "Engine/Scene/Unit.h"

#include "Render/DrawCommand/DrawCommandBuilder.h"
#include "Render/DrawCommand/DrawCommandContext.h"
#include "Render/Scene/Object/MeshObject.h"

#include <algorithm>

namespace URay
{

using namespace Render;

URAY_REGISTER_CLASS(MeshComponent)
URAY_REGISTER_COMPONENT(MeshComponent)

MeshComponent::MeshComponent()
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    Mesh* defaultMesh = assetSystem.FindMesh("Box");
    SetMesh(defaultMesh);
}

void MeshComponent::RegisterClass()
{
    Super::RegisterClass();

    StaticClass()->AddProperty({ .type = PropertyType::Mesh,
                                 .name = "Mesh",
                                 .offset = offsetof(MeshComponent, mesh),
                                 .size = sizeof(Mesh*),
                                 .OnChangedCallback = [](Object* owner, const Property&)
                                 {
                                     MeshComponent* meshComp = static_cast<MeshComponent*>(owner);
                                     meshComp->SetMesh(meshComp->GetMesh());
                                 } });
}

Render::RenderObject* MeshComponent::CreateRenderObject()
{
    Unit* owner = GetOwner();
    if (!owner)
        return nullptr;

    TransformComponent* transform = owner->GetTransform();

    Render::MeshObjectState objectState = {};
    objectState.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    objectState.mesh = mesh;
    objectState.materials = materials;

    renderObject = new Render::MeshObject(objectState);
    return renderObject;
}

void MeshComponent::SetMesh(Mesh* newMesh)
{
    mesh = newMesh;
    materials = mesh ? mesh->GetDefaultMaterials()
                     : std::vector<Material*>();

    MarkDirty();
}

void MeshComponent::SetMaterial(Material* newMaterial, size_t index)
{
    if (materials.size() <= index)
    {
        materials.resize(index + 1);
    }

    if (materials[index] == newMaterial)
        return;

    materials[index] = newMaterial;

    MarkDirty();
}

void MeshComponent::UpdateRenderObject()
{
    Unit* owner = GetOwner();
    if (!owner)
        return;

    TransformComponent* transform = owner->GetTransform();

    MeshObjectState state = {};
    state.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    state.mesh = mesh;
    state.materials = materials;

    MeshObject* meshObject = static_cast<MeshObject*>(renderObject);
    meshObject->Update(state);
}

} // namespace URay

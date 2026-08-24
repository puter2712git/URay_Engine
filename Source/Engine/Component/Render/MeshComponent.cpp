#include "MeshComponent.h"

#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Material/MaterialManager.h"
#include "Engine/Mesh/Mesh.h"
#include "Engine/Mesh/MeshManager.h"
#include "Engine/Object/Class/Class.h"
#include "Engine/Unit.h"

#include "Render/DrawCommand/DrawCommandBuilder.h"
#include "Render/DrawCommand/DrawCommandContext.h"
#include "Render/Scene/Object/MeshObject.h"

#include <algorithm>

namespace URay
{

using namespace RHI;

URAY_REGISTER_CLASS(MeshComponent)
URAY_REGISTER_COMPONENT(MeshComponent)

MeshComponent::MeshComponent()
{
    Mesh* defaultMesh = gEngine->GetMeshManager()->GetMesh("Box");
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

void MeshComponent::Update(float deltaTime)
{
    Super::Update(deltaTime);

    if (renderObject && renderObject->IsDirty())
    {
        renderObject->SetDirty(false);

        Unit* owner = GetOwner();
        if (!owner)
            return;

        TransformComponent* transform = owner->GetTransform();

        renderObject->Update(RHI::MeshObjectState{
            .worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity,
            .mesh = mesh,
            .materials = materials,
        });
    }
}

void MeshComponent::OnAttached()
{
    Unit* owner = GetOwner();
    owner->RegisterTransformUpdateCallback([this]()
                                           {
        if (renderObject)
        {
            TransformComponent* transform = GetOwner()->GetTransform();

            renderObject->Update(RHI::MeshObjectState{
                .worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity,
                .mesh = mesh,
                .materials = materials,
            });
    } });
}

void MeshComponent::OnDetached()
{
}

RHI::RenderObject* MeshComponent::CreateRenderObject()
{
    Unit* owner = GetOwner();
    if (!owner)
        return nullptr;

    TransformComponent* transform = owner->GetTransform();

    RHI::MeshObjectState objectState = {};
    objectState.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    objectState.mesh = mesh;
    objectState.materials = materials;

    renderObject = new RHI::MeshObject(objectState);
    return renderObject;
}

void MeshComponent::SetMesh(Mesh* newMesh)
{
    if (mesh == newMesh)
        return;

    mesh = newMesh;
    materials = mesh ? mesh->GetDefaultMaterials()
                     : std::vector<Material*>();

    if (renderObject)
    {
        renderObject->SetDirty(true);
    }
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

    if (renderObject)
    {
        renderObject->SetDirty(true);
    }
}

} // namespace URay

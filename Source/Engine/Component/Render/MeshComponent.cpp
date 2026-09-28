#include "Engine/Component/Render/MeshComponent.h"

#include "Engine/Asset/AssetDatabase.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/EngineAsset.h"
#include "Engine/Asset/Mesh/Mesh.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Object/Class/Class.h"
#include "Engine/Scene/Unit.h"

#include "Render/Rendering/DrawCommand/DrawCommandBuilder.h"
#include "Render/Rendering/DrawCommand/DrawCommandContext.h"
#include "Render/Rendering/Object/Drawable/MeshObject.h"

#include <algorithm>

namespace URay
{

URAY_REGISTER_CLASS(MeshComponent)
URAY_REGISTER_COMPONENT(MeshComponent)

MeshComponent::MeshComponent()
{
    meshUUID = EngineAsset::CubeMesh;
    materialUUIDs = { EngineAsset::MeshMaterial };
}

void MeshComponent::RegisterClass()
{
    StaticClass()->AddProperty(
        { .type = PropertyType::Mesh,
          .name = "Mesh",
          .offset = offsetof(MeshComponent, meshUUID),
          .size = sizeof(UUID) });
    StaticClass()->AddProperty(
        { .type = PropertyType::Bool,
          .name = "Casts Shadow",
          .offset = offsetof(MeshComponent, castsShadow),
          .size = sizeof(bool) });
}

Render::RenderObject* MeshComponent::CreateRenderObject()
{
    Unit* owner = GetOwner();
    if (!owner)
        return nullptr;

    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();

    TransformComponent* transform = owner->GetTransform();

    std::vector<Material*> materials;
    for (const UUID& uuid : materialUUIDs)
    {
        Material* material = assetDatabase.Find<Material>(uuid);
        materials.push_back(material);
    }

    Render::MeshObjectState objectState = {};
    objectState.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    objectState.mesh = assetDatabase.Find<Mesh>(meshUUID);
    objectState.materials = materials;
    objectState.castsShadow = castsShadow;

    renderObject = new Render::MeshObject(objectState);
    return renderObject;
}

void MeshComponent::SetMeshUUID(const UUID& newMeshUUID)
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();

    meshUUID = newMeshUUID;

    Mesh* mesh = assetDatabase.Find<Mesh>(meshUUID);
    materialUUIDs = mesh ? mesh->GetDefaultMaterials()
                         : std::vector<UUID>();

    MarkDirty();
}

void MeshComponent::SetMaterial(const UUID& newMaterialUUID, size_t index)
{
    if (materialUUIDs.size() <= index)
    {
        materialUUIDs.resize(index + 1);
    }

    if (materialUUIDs[index] == newMaterialUUID)
        return;

    materialUUIDs[index] = newMaterialUUID;

    MarkDirty();
}

void MeshComponent::UpdateRenderObject()
{
    Unit* owner = GetOwner();
    if (!owner)
        return;

    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();

    TransformComponent* transform = owner->GetTransform();

    std::vector<Material*> materials;
    for (const UUID& uuid : materialUUIDs)
    {
        Material* material = assetDatabase.Find<Material>(uuid);
        materials.push_back(material);
    }

    Render::MeshObjectState objectState = {};
    objectState.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    objectState.mesh = assetDatabase.Find<Mesh>(meshUUID);
    objectState.materials = materials;
    objectState.castsShadow = castsShadow;

    Render::MeshObject* meshObject = static_cast<Render::MeshObject*>(renderObject);
    meshObject->Update(objectState);
}

} // namespace URay

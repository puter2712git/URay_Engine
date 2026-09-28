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
    meshHandle = EngineAsset::CubeMesh;
    materialHandles = { EngineAsset::MeshMaterial };
}

void MeshComponent::RegisterClass()
{
    StaticClass()->AddProperty(
        { .type = PropertyType::Mesh,
          .name = "Mesh",
          .offset = offsetof(MeshComponent, meshHandle),
          .size = sizeof(AssetHandle) });
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
    for (const AssetHandle& handle : materialHandles)
    {
        Material* material = assetDatabase.Find<Material>(handle);
        materials.push_back(material);
    }

    Render::MeshObjectState objectState = {};
    objectState.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    objectState.mesh = assetDatabase.Find<Mesh>(meshHandle);
    objectState.materials = materials;
    objectState.castsShadow = castsShadow;

    renderObject = new Render::MeshObject(objectState);
    return renderObject;
}

void MeshComponent::SetMeshHandle(const AssetHandle& newMeshHandle)
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();

    meshHandle = newMeshHandle;

    Mesh* mesh = assetDatabase.Find<Mesh>(meshHandle);
    materialHandles = mesh ? mesh->GetDefaultMaterials()
                           : std::vector<AssetHandle>();

    MarkDirty();
}

void MeshComponent::SetMaterial(const AssetHandle& newMaterialHandle, size_t index)
{
    if (materialHandles.size() <= index)
    {
        materialHandles.resize(index + 1);
    }

    if (materialHandles[index] == newMaterialHandle)
        return;

    materialHandles[index] = newMaterialHandle;

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
    for (const AssetHandle& handle : materialHandles)
    {
        Material* material = assetDatabase.Find<Material>(handle);
        materials.push_back(material);
    }

    Render::MeshObjectState objectState = {};
    objectState.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    objectState.mesh = assetDatabase.Find<Mesh>(meshHandle);
    objectState.materials = materials;
    objectState.castsShadow = castsShadow;

    Render::MeshObject* meshObject = static_cast<Render::MeshObject*>(renderObject);
    meshObject->Update(objectState);
}

} // namespace URay

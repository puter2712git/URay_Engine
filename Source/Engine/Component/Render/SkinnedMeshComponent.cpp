#include "SkinnedMeshComponent.h"

#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Mesh/SkinnedMesh.h>
#include <Engine/Component/TransformComponent.h>
#include <Engine/Engine.h>
#include <Engine/Scene/Unit.h>
#include <Render/Rendering/DrawCommand/DrawCommandBuilder.h>
#include <Render/Rendering/DrawCommand/DrawCommandContext.h>
#include <Render/Rendering/Object/Drawable/SkinnedMeshObject.h>

namespace URay
{

URAY_REGISTER_CLASS(SkinnedMeshComponent)
URAY_REGISTER_COMPONENT(SkinnedMeshComponent)

SkinnedMeshComponent::SkinnedMeshComponent() = default;

void SkinnedMeshComponent::RegisterClass()
{
    StaticClass()->AddProperty(
        { .type = PropertyType::SkinnedMesh,
          .name = "Skinned Mesh",
          .offset = offsetof(SkinnedMeshComponent, skinnedMeshHandle),
          .size = sizeof(AssetHandle),
          .OnChangedCallback = [&](Object& owner, const Property& property)
          {
              SkinnedMeshComponent* skinnedMeshComp = static_cast<SkinnedMeshComponent*>(&owner);
              skinnedMeshComp->SetSkinnedMeshHandle(skinnedMeshComp->GetSkinnedMeshHandle());
          } });
    StaticClass()->AddProperty(
        { .type = PropertyType::Bool,
          .name = "Casts Shadow",
          .offset = offsetof(SkinnedMeshComponent, castsShadow),
          .size = sizeof(bool) });
}

Render::RenderObject* SkinnedMeshComponent::CreateRenderObject()
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

    Render::SkinnedMeshObjectState objectState = {};
    objectState.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    objectState.skinnedMesh = assetDatabase.Find<SkinnedMesh>(skinnedMeshHandle);
    objectState.materials = materials;
    objectState.castsShadow = castsShadow;

    renderObject = new Render::SkinnedMeshObject(objectState);
    return renderObject;
}

void SkinnedMeshComponent::SetSkinnedMeshHandle(const AssetHandle& handle)
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();

    skinnedMeshHandle = handle;

    SkinnedMesh* skinnedMesh = assetDatabase.Find<SkinnedMesh>(skinnedMeshHandle);
    materialHandles = skinnedMesh
                          ? skinnedMesh->GetDefaultMaterials()
                          : std::vector<AssetHandle>{};

    MarkDirty();
}

void SkinnedMeshComponent::SetMaterial(const AssetHandle& newMaterialHandle, size_t index)
{
    if (index >= materialHandles.size())
    {
        materialHandles.resize(index + 1);
    }

    if (materialHandles[index] == newMaterialHandle)
        return;

    materialHandles[index] = newMaterialHandle;

    MarkDirty();
}

void SkinnedMeshComponent::SetMaterials(const std::vector<AssetHandle>& newMaterialHandles)
{
    materialHandles = newMaterialHandles;
    MarkDirty();
}

void SkinnedMeshComponent::UpdateRenderObject()
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

    Render::SkinnedMeshObjectState objectState = {};
    objectState.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    objectState.skinnedMesh = assetDatabase.Find<SkinnedMesh>(skinnedMeshHandle);
    objectState.materials = materials;
    objectState.castsShadow = castsShadow;

    Render::SkinnedMeshObject* skinnedMeshObject = static_cast<Render::SkinnedMeshObject*>(renderObject);
    skinnedMeshObject->Update(objectState);
}

} // namespace URay

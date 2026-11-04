#include "SpriteComponent.h"

#include <Core/Log/LogSystem.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetFactory.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/EngineAsset.h>
#include <Engine/Asset/Material/Material.h>
#include <Engine/Asset/Material/MaterialInstance.h>
#include <Engine/Asset/Mesh/Mesh.h>
#include <Engine/Asset/Texture/Texture.h>
#include <Engine/Component/TransformComponent.h>
#include <Engine/Engine.h>
#include <Engine/Scene/Unit.h>
#include <Render/Rendering/Object/Drawable/MeshObject.h>

#include <utility>

namespace URay
{

URAY_REGISTER_CLASS(SpriteComponent)
URAY_REGISTER_COMPONENT(SpriteComponent)

SpriteComponent::SpriteComponent()
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();

    textureHandle = EngineAsset::WhiteTexture;
    quadMeshHandle = EngineAsset::QuadMesh;
    materialHandle = EngineAsset::SpriteMaterial;
}

void SpriteComponent::RegisterClass()
{
    StaticClass()->AddProperty(
        { .type = PropertyType::Texture,
          .name = "Texture",
          .offset = offsetof(SpriteComponent, textureHandle),
          .size = sizeof(AssetHandle) });
}

Render::RenderObject* SpriteComponent::CreateRenderObject()
{
    Unit* owner = GetOwner();
    if (!owner)
        return nullptr;

    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();

    TransformComponent* transform = owner->GetTransform();

    MaterialInstance* materialInstance = GetOrCreateMaterialInstance();
    if (!materialInstance)
        return nullptr;

    Render::MeshObjectState objectState = {};
    objectState.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    objectState.mesh = assetDatabase.Find<Mesh>(quadMeshHandle);
    objectState.materials = { materialInstance };

    renderObject = new Render::MeshObject(objectState);
    return renderObject;
}

void SpriteComponent::UpdateRenderObject()
{
    Unit* owner = GetOwner();
    if (!owner)
        return;

    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();

    TransformComponent* transform = owner->GetTransform();

    MaterialInstance* materialInstance = GetOrCreateMaterialInstance();
    if (!materialInstance)
        return;

    Render::MeshObjectState objectState = {};
    objectState.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    objectState.mesh = assetDatabase.Find<Mesh>(quadMeshHandle);
    objectState.materials = { materialInstance };

    Render::MeshObject* meshObject = static_cast<Render::MeshObject*>(renderObject);
    meshObject->Update(objectState);
}

MaterialInstance* SpriteComponent::GetOrCreateMaterialInstance()
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDB = assetSystem.GetDatabase();

    Material* parent = assetDB.Find<Material>(materialHandle);
    if (!parent)
    {
        URAY_LOG("[SpriteComponent] Parent material was not found.");
        return nullptr;
    }

    if (!parent->IsInitialized() && !parent->Initialize())
    {
        URAY_LOG("[SpriteComponent] Failed to initialize parent material.");
        return nullptr;
    }

    MaterialInstance* existing = assetDB.Find<MaterialInstance>(materialInstanceHandle);

    if (existing && existing->GetParent() == parent)
    {
        if (!existing->SetTexture("textureImage", textureHandle))
        {
            URAY_LOG("[SpriteComponent] Failed to set texture override.");
            return nullptr;
        }

        return existing;
    }

    auto instance = assetSystem.GetFactory().CreateMaterialInstance(materialHandle);

    if (!instance)
    {
        URAY_LOG("[SpriteComponent] Failed to create material instance.");
        return nullptr;
    }

    if (!instance->SetTexture("textureImage", textureHandle))
    {
        URAY_LOG("[SpriteComponent] Failed to set texture override.");
        return nullptr;
    }

    const AssetHandle newHandle = instance->GetHandle();

    if (!assetDB.Insert(std::move(instance)))
    {
        URAY_LOG("[SpriteComponent] Failed to register material instance.");
        return nullptr;
    }

    materialInstanceHandle = newHandle;
    return assetDB.Find<MaterialInstance>(materialInstanceHandle);
}

} // namespace URay

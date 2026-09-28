#include "Engine/Component/Render/SpriteComponent.h"

#include "Engine/Asset/AssetDatabase.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/EngineAsset.h"
#include "Engine/Asset/Material/Material.h"
#include "Engine/Asset/Mesh/Mesh.h"
#include "Engine/Asset/Texture/Texture.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Unit.h"

#include "Render/Rendering/Object/Drawable/MeshObject.h"

namespace URay
{

using namespace Render;

URAY_REGISTER_CLASS(SpriteComponent)
URAY_REGISTER_COMPONENT(SpriteComponent)

SpriteComponent::SpriteComponent()
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();

    textureUUID = EngineAsset::WhiteTexture;
    quadMeshUUID = EngineAsset::QuadMesh;
    materialUUID = EngineAsset::SpriteMaterial;
}

void SpriteComponent::RegisterClass()
{
    StaticClass()->AddProperty(
        { .type = PropertyType::Texture,
          .name = "Texture",
          .offset = offsetof(SpriteComponent, textureUUID),
          .size = sizeof(Texture*) });
}

Render::RenderObject* SpriteComponent::CreateRenderObject()
{
    Unit* owner = GetOwner();
    if (!owner)
        return nullptr;

    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();

    TransformComponent* transform = owner->GetTransform();

    Render::MeshObjectState objectState = {};
    objectState.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    objectState.mesh = assetDatabase.Find<Mesh>(quadMeshUUID);
    objectState.materials = { assetDatabase.Find<Material>(materialUUID) };

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

    Render::MeshObjectState objectState = {};
    objectState.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    objectState.mesh = assetDatabase.Find<Mesh>(quadMeshUUID);
    objectState.materials = { assetDatabase.Find<Material>(materialUUID) };

    Render::MeshObject* meshObject = static_cast<Render::MeshObject*>(renderObject);
    meshObject->Update(objectState);
}

} // namespace URay

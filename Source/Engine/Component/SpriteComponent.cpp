#include "Engine/Component/SpriteComponent.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Material/Material.h"
#include "Engine/Mesh/Mesh.h"
#include "Engine/Unit.h"

#include "Render/Scene/Object/MeshObject.h"

namespace URay
{

using namespace Render;

URAY_REGISTER_CLASS(SpriteComponent)
URAY_REGISTER_COMPONENT(SpriteComponent)

SpriteComponent::SpriteComponent()
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();

    texture = assetSystem.FindTexture("DefaultWhite");
    quadMesh = assetSystem.FindMesh("Quad");
    material = assetSystem.FindMaterial("Sprite");
    material->SetTexture(texture);
}

void SpriteComponent::RegisterClass()
{
    Super::RegisterClass();

    StaticClass()->AddProperty({ .type = PropertyType::Texture,
                                 .name = "Texture",
                                 .offset = offsetof(SpriteComponent, texture),
                                 .size = sizeof(Texture*),
                                 .OnChangedCallback = [](Object* owner, const Property& property)
                                 {
                                     SpriteComponent* spriteComp = static_cast<SpriteComponent*>(owner);
                                     spriteComp->material->SetTexture(spriteComp->texture);
                                 } });
}

Render::RenderObject* SpriteComponent::CreateRenderObject()
{
    Unit* owner = GetOwner();
    if (!owner)
        return nullptr;

    TransformComponent* transform = owner->GetTransform();

    Render::MeshObjectState objectState = {};
    objectState.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    objectState.mesh = quadMesh;
    objectState.materials = { material };

    renderObject = new Render::MeshObject(objectState);
    return renderObject;
}

void SpriteComponent::UpdateRenderObject()
{
    Unit* owner = GetOwner();
    if (!owner)
        return;

    TransformComponent* transform = owner->GetTransform();

    Render::MeshObjectState state = {};
    state.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    state.mesh = quadMesh;
    state.materials = { material };

    Render::MeshObject* meshObject = static_cast<Render::MeshObject*>(renderObject);
    meshObject->Update(state);
}

} // namespace URay

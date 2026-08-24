#include "Engine/Component/SpriteComponent.h"

#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Material/Material.h"
#include "Engine/Material/MaterialManager.h"
#include "Engine/Mesh/Mesh.h"
#include "Engine/Mesh/MeshManager.h"
#include "Engine/Texture/TextureManager.h"
#include "Engine/Unit.h"

#include "Render/Scene/Object/MeshObject.h"

namespace URay
{

using namespace RHI;

URAY_REGISTER_CLASS(SpriteComponent)
URAY_REGISTER_COMPONENT(SpriteComponent)

SpriteComponent::SpriteComponent()
{
    TextureManager* textureManager = gEngine->GetTextureManager();
    texture = textureManager->GetTexture("Test");

    MeshManager* meshManager = gEngine->GetMeshManager();
    quadMesh = meshManager->GetMesh("Quad");

    material = gEngine->GetMaterialManager()->GetOrCreate("Sprite");
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

RHI::RenderObject* SpriteComponent::CreateRenderObject()
{
    Unit* owner = GetOwner();
    if (!owner)
        return nullptr;

    TransformComponent* transform = owner->GetTransform();

    RHI::MeshObjectState objectState = {};
    objectState.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    objectState.mesh = quadMesh;
    objectState.materials = { material };

    renderObject = new RHI::MeshObject(objectState);
    return renderObject;
}

} // namespace URay

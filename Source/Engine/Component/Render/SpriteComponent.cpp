#include "SpriteComponent.h"

#include "Engine/Material/Material.h"
#include "Engine/Material/MaterialManager.h"
#include "Engine/Mesh/MeshManager.h"
#include "Engine/Texture/TextureManager.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Unit.h"

#include "Render/DrawCommand/DrawCommandBuilder.h"
#include "Render/DrawCommand/DrawCommandContext.h"

namespace URay
{

URAY_REGISTER_CLASS(SpriteComponent)
URAY_REGISTER_COMPONENT(SpriteComponent)

SpriteComponent::SpriteComponent()
{
    TextureManager* textureManager = gEngine->GetTextureManager();
    texture = textureManager->GetTexture("Test");

    MeshManager* meshManager = gEngine->GetMeshManager();
    quadMesh = meshManager->GetMesh("Quad");

    material = gEngine->GetMaterialManager()->GetOrCreate("Sprite");
    material->SetTextureAsset(texture);
}

void SpriteComponent::RegisterClass()
{
    Super::RegisterClass();

    StaticClass()->AddProperty({ PropertyType::Texture, "Texture",
                                 offsetof(SpriteComponent, texture), sizeof(TextureAsset*),
                                 [](Object* owner, const Property& property)
                                 {
                                     SpriteComponent* spriteComp = static_cast<SpriteComponent*>(owner);
                                     spriteComp->material->SetTextureAsset(spriteComp->texture);
                                 } });
}

void SpriteComponent::SubmitCommand(DrawCommandBuilder& builder)
{
    TransformComponent* transform = GetOwner()->GetTransform();

    MeshCommandContext context = {};
    context.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    context.meshAsset = quadMesh;
    context.material = material;

    builder.BuildFromMesh(context);
}

} // namespace URay
